#ifndef SHELL_PARSER_H
#define SHELL_PARSER_H

#include <stddef.h>

typedef enum
{
    REDIRECT_NONE,
    REDIRECT_FILE,
    REDIRECT_TCP
} RedirectType;

typedef struct
{
    RedirectType type;
    char *path;
    char *host;
    unsigned short port;
} Redirect;

typedef struct
{
    char **argv;
    size_t argc;
    Redirect input;
    Redirect output;
} Command;

typedef struct
{
    Command *commands;
    size_t command_count;
} Pipeline;

#define MAXERROR 1024
typedef struct
{
    size_t position;
    char message[MAXERROR];
} ParseError;

int pipeline_parse(const char *line, Pipeline *pipeline, ParseError *error);
void pipeline_free(Pipeline *pipeline);
void pipeline_print(const Pipeline *pipeline);

#define PARSE_SUCCESS (0)
#define PARSE_FAILED (-1)

#endif
