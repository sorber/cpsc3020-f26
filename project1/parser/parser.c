#define _POSIX_C_SOURCE 200809L
#include "parser.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TOK_WORD,
    TOK_PIPE,
    TOK_IN,
    TOK_OUT,
    TOK_END
} TokenType;

typedef struct
{
    TokenType type;
    char *text;
    size_t pos;
} Token;

typedef struct
{
    Token *v;
    size_t n, cap;
} TokenList;

typedef struct
{
    char *v;
    size_t n, cap;
} Buffer;

static void parseerror_set_error(ParseError *e, size_t p, const char *msg)
{
    if (e)
    {
        e->position = p;
        snprintf(e->message, sizeof e->message, "%s", msg);
    }
}

static int array_grow(void **p, size_t *cap, size_t elem, size_t need)
{
    size_t c = *cap ? *cap : 4;
    void *q;
    if (*cap >= need)
        return PARSE_SUCCESS;
    while (c < need)
        c *= 2;
    q = realloc(*p, c * elem);
    if (!q)
        return PARSE_FAILED;
    *p = q;
    *cap = c;
    return PARSE_SUCCESS;
}

static int buffer_addchar(Buffer *b, char c)
{
    if (array_grow((void **)&b->v, &b->cap, 1, b->n + 2) < 0)
        return PARSE_FAILED;
    b->v[b->n++] = c;
    b->v[b->n] = '\0';
    return PARSE_SUCCESS;
}

static int tokenlist_addtoken(TokenList *ptokenlist, TokenType tokentype, const char *text, size_t pos)
{
    Token *x;
    if (array_grow((void **)&ptokenlist->v, &ptokenlist->cap, sizeof *ptokenlist->v, ptokenlist->n + 1) < 0)
        return PARSE_FAILED;
    x = &ptokenlist->v[ptokenlist->n++];
    x->type = tokentype;
    x->pos = pos;
    x->text = text ? strdup(text) : NULL;
    return text && !x->text ? PARSE_FAILED : PARSE_SUCCESS;
}

static void tokenlist_free(TokenList *ptokenlist)
{
    size_t i;
    for (i = 0; i < ptokenlist->n; i++)
        free(ptokenlist->v[i].text);
    free(ptokenlist->v);
    memset(ptokenlist, 0, sizeof *ptokenlist);
}

static int tokenize(const char *s, TokenList *t, ParseError *e)
{
    size_t i = 0;
    while (s[i])
    {
        size_t start;
        while (isspace((unsigned char)s[i]))
        {
            i++;
        }
        if (!s[i])
            break;
        start = i;
        if (s[i] == '|')
        {
            if (tokenlist_addtoken(t, TOK_PIPE, NULL, i++) < 0)
                goto outofmemory;
            continue;
        }
        if (s[i] == '<')
        {
            if (tokenlist_addtoken(t, TOK_IN, NULL, i++) < 0)
                goto outofmemory;
            continue;
        }
        if (s[i] == '>')
        {
            if (tokenlist_addtoken(t, TOK_OUT, NULL, i++) < 0)
                goto outofmemory;
            continue;
        }

        {
            Buffer b = {0};
            char quote = 0;
            size_t quote_pos = 0;
            int started = 0;
            while (s[i])
            {
                char c = s[i];
                if (!quote && (isspace((unsigned char)c) || c == '|' || c == '<' || c == '>'))
                    break;
                if (!quote && (c == '\'' || c == '"'))
                {
                    quote = c;
                    quote_pos = i++;
                    started = 1;
                    continue;
                }
                if (quote && c == quote)
                {
                    quote = 0;
                    i++;
                    continue;
                }
                started = 1;
                if (buffer_addchar(&b, c) < 0)
                {
                    free(b.v);
                    goto outofmemory;
                }
                i++;
            }
            if (quote)
            {
                free(b.v);
                parseerror_set_error(e, quote_pos, "unterminated quote");
                return PARSE_FAILED;
            }
            if (!started)
            {
                free(b.v);
                parseerror_set_error(e, start, "invalid token");
                return PARSE_FAILED;
            }
            if (!b.v)
                b.v = strdup("");
            if (!b.v || tokenlist_addtoken(t, TOK_WORD, b.v, start) < 0)
            {
                free(b.v);
                goto outofmemory;
            }
            free(b.v);
        }
    }
    if (tokenlist_addtoken(t, TOK_END, NULL, i) < 0)
        goto outofmemory;
    return PARSE_SUCCESS;

outofmemory:
    parseerror_set_error(e, i, "out of memory");
    return PARSE_FAILED;
}

static void redirect_free(Redirect *r)
{
    free(r->path);
    free(r->host);
    memset(r, 0, sizeof *r);
}
static void command_free(Command *c)
{
    size_t i;
    for (i = 0; i < c->argc; i++)
        free(c->argv[i]);
    free(c->argv);
    redirect_free(&c->input);
    redirect_free(&c->output);
    memset(c, 0, sizeof *c);
}
void pipeline_free(Pipeline *p)
{
    size_t i;
    if (!p)
        return;
    for (i = 0; i < p->command_count; i++)
        command_free(&p->commands[i]);
    free(p->commands);
    memset(p, 0, sizeof *p);
}

static int command_add_arg(Command *c, size_t *cap, const char *s)
{
    char *copy;
    if (array_grow((void **)&c->argv, cap, sizeof *c->argv, c->argc + 2) < PARSE_SUCCESS)
        return PARSE_FAILED;
    copy = strdup(s);
    if (!copy)
        return PARSE_FAILED;
    c->argv[c->argc++] = copy;
    c->argv[c->argc] = NULL;
    return PARSE_SUCCESS;
}

static int command_add(Pipeline *p, size_t *cap, Command *c)
{
    if (array_grow((void **)&p->commands, cap, sizeof *p->commands, p->command_count + 1) < 0)
        return PARSE_FAILED;
    p->commands[p->command_count++] = *c;
    memset(c, 0, sizeof *c);
    return PARSE_SUCCESS;
}

static int parse_port(const char *s, unsigned short *port, ParseError *e, size_t pos)
{
    char *end;
    long v;
    errno = 0;
    v = strtol(s, &end, 10);
    if (!*s || errno || *end || v < 1 || v > 65535)
    {
        parseerror_set_error(e, pos, "TCP port must be between 1 and 65535");
        return PARSE_FAILED;
    }
    *port = (unsigned short)v;
    return PARSE_SUCCESS;
}

static int redirect_set(Redirect *r, const char *target, ParseError *e, size_t pos)
{
    if (r->type != REDIRECT_NONE)
    {
        parseerror_set_error(e, pos, "duplicate redirection");
        return PARSE_FAILED;
    }
    if (strncmp(target, "tcp:", 4) != 0)
    {
        r->path = strdup(target);
        if (!r->path)
        {
            parseerror_set_error(e, pos, "out of memory");
            return PARSE_FAILED;
        }
        r->type = REDIRECT_FILE;
        return PARSE_SUCCESS;
    }
    {
        const char *a = target + 4, *colon, *port_text;
        size_t len;
        if (*a == '[')
        {
            const char *close = strchr(a, ']');
            if (!close || close[1] != ':')
            {
                parseerror_set_error(e, pos, "use tcp:[IPv6-address]:port");
                return PARSE_FAILED;
            }
            len = (size_t)(close - a - 1);
            a++;
            port_text = close + 2;
        }
        else
        {
            colon = strrchr(a, ':');
            if (!colon || strchr(a, ':') != colon)
            {
                parseerror_set_error(e, pos, "TCP endpoint must be tcp:host:port");
                return PARSE_FAILED;
            }
            len = (size_t)(colon - a);
            port_text = colon + 1;
        }
        if (!len)
        {
            parseerror_set_error(e, pos, "missing TCP host");
            return PARSE_FAILED;
        }
        r->host = malloc(len + 1);
        if (!r->host)
        {
            parseerror_set_error(e, pos, "out of memory");
            return PARSE_FAILED;
        }
        memcpy(r->host, a, len);
        r->host[len] = '\0';
        if (parse_port(port_text, &r->port, e, pos) < 0)
        {
            free(r->host);
            r->host = NULL;
            return PARSE_FAILED;
        }
        r->type = REDIRECT_TCP;
        return PARSE_SUCCESS;
    }
}

int pipeline_parse(const char *line, Pipeline *p, ParseError *e)
{
    TokenList t = {0};
    size_t i = 0, pcap = 0;
    if (!line || !p)
        return PARSE_FAILED;
    memset(p, 0, sizeof *p);
    if (e)
        memset(e, 0, sizeof *e);
    if (tokenize(line, &t, e) < 0)
    {
        tokenlist_free(&t);
        return PARSE_FAILED;
    }

    while (t.v[i].type != TOK_END)
    {
        Command c = {0};
        size_t acap = 0;
        if (t.v[i].type == TOK_PIPE)
        {
            parseerror_set_error(e, t.v[i].pos, "expected command before '|'");
            command_free(&c);
            goto fail;
        }
        while (t.v[i].type != TOK_PIPE && t.v[i].type != TOK_END)
        {
            if (t.v[i].type == TOK_WORD)
            {
                if (command_add_arg(&c, &acap, t.v[i].text) < 0)
                {
                    parseerror_set_error(e, t.v[i].pos, "out of memory");
                    command_free(&c);
                    goto fail;
                }
                i++;
            }
            else
            {
                TokenType op = t.v[i].type;
                size_t pos = t.v[i++].pos;
                Redirect *r;
                if (t.v[i].type != TOK_WORD)
                {
                    parseerror_set_error(e, pos, "redirection requires a target");
                    command_free(&c);
                    goto fail;
                }
                r = op == TOK_IN ? &c.input : &c.output;
                if (redirect_set(r, t.v[i].text, e, t.v[i].pos) < 0)
                {
                    command_free(&c);
                    goto fail;
                }
                i++;
            }
        }
        if (!c.argc)
        {
            parseerror_set_error(e, t.v[i].pos, "redirection must belong to a command");
            command_free(&c);
            goto fail;
        }
        if (command_add(p, &pcap, &c) < 0)
        {
            parseerror_set_error(e, t.v[i].pos, "out of memory");
            command_free(&c);
            goto fail;
        }
        if (t.v[i].type == TOK_PIPE)
        {
            size_t pos = t.v[i++].pos;
            if (t.v[i].type == TOK_END)
            {
                parseerror_set_error(e, pos, "expected command after '|'");
                goto fail;
            }
        }
    }
    tokenlist_free(&t);
    return PARSE_SUCCESS;
fail:
    tokenlist_free(&t);
    pipeline_free(p);
    return PARSE_FAILED;
}

static void print_redirect(const char *label, const Redirect *r)
{
    if (r->type == REDIRECT_NONE)
        printf("  %s: default\n", label);
    else if (r->type == REDIRECT_FILE)
        printf("  %s: file %s\n", label, r->path);
    else
        printf("  %s: tcp %s:%u\n", label, r->host, r->port);
}

// just here for testing purposes
void pipeline_print(const Pipeline *p)
{
    size_t i, j;
    printf("commands=%zu\n", p->command_count);
    for (i = 0; i < p->command_count; i++)
    {
        printf("command[%zu]\n", i);
        for (j = 0; j < p->commands[i].argc; j++)
            printf("  argv[%zu]=<%s>\n", j, p->commands[i].argv[j]);
        print_redirect("input", &p->commands[i].input);
        print_redirect("output", &p->commands[i].output);
    }
}
