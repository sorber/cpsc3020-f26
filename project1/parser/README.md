# Shell command parser for project 1

A small simplified command-line parser (written in C) for CPSC 3020.

It supports:
* commands with arguments
* single- and double-quoted arguments
* `<` and `>` file redirection
* pipelines using `|`
* TCP redirection targets such as `tcp:localhost:9000`
* bracketed IPv6 targets such as `tcp:[::1]:9000`


## to build it
make
make test

## Notes
This parser only builds a Pipeline data structure that represents the command pipeline typed in the by the user. It does not execute commands, create sockets, open files, or set up pipes for you. You have to do that part.

The demo.c program is included for testing and to show you how to call the parser from your code.

The tests.sh shell script is used by the Makefile to test the parser. It might also be useful to look at what it's doing as you think about how to implement testing of your own.

For this project you do **not** need to change the parser at all. Just use it. Your shell code should just link to it. Your shell should only call the functions and use the types that are visible in parser.h.
