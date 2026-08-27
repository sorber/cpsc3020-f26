#define _POSIX_C_SOURCE 200809L
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        Pipeline p;
        ParseError e;
        if (pipeline_parse(argv[1], &p, &e) < PARSE_SUCCESS)
        {
            fprintf(stderr, "parse error at %zu: %s\n", e.position, e.message);
            return EXIT_FAILURE;
        }
        pipeline_print(&p);
        pipeline_free(&p);
        return EXIT_SUCCESS;
    }
    else
    {
        printf("usage: %s <pipeline to parse>\n", argv[0]);
        return EXIT_FAILURE;
    }
}
