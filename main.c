#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef GIT_VERSION
#define GIT_VERSION "undefined"
#endif

// opt_parse parses known CLI options (starting with hyphens) 
// and returns index of the first non-option in argv.
// If error is encountered, a negative integer is returned.
// All options are expected to be placed before non-options.
// If sole "--" is encountered, options parsing is stopped 
// and further args are considered non-options.
int opt_parse(
    char** argv, 
    bool* isHelp,
    bool* isVersion,
    bool* isDebug,
    char** assign
) {
    int i = 1;
    for (; argv[i] != nullptr; i++) {
        if (argv[i][0] != '-') {
            return i;
        }
        if (strcmp(argv[i], "-") == 0) {
            return i;
        }
        else if (strcmp(argv[i], "--") == 0) {
            return i+1;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            *isHelp = true;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            *isVersion = true;
        }
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            *isDebug = true;
        }
        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--assign") == 0) {
            if (argv[i+1] == nullptr) {
                fprintf(stderr, "error: value is not provided for -a / --assign\n");
                return -1;
            }
            *assign = argv[i++];
        } else {
            fprintf(stderr, "error: unknown option %s\n", argv[i]);
            return -1;
        }
    }
    return i;
}
int main([[maybe_unused]] int argc, char** argv) {
    bool isHelp = false;
    bool isVersion = false;
    bool isDebug = false;
    char* assign = nullptr;
    int argIdx = opt_parse(argv, &isHelp, &isVersion, &isDebug, &assign);

    if (isDebug) {
        printf("isHelp = %s\n", isHelp ? "true": "false");
        printf("isVersion = %s\n", isVersion ? "true": "false");
        printf("isDebug = %s\n", isDebug ? "true": "false");
        printf("assign = %s\n", assign == nullptr ? "null" : assign);
        printf("argIdx = %d\n", argIdx);
    }

    if (argIdx < 0) {
        fprintf(stderr, "usage: printx [-h, --help | -v, --version | -a VAR] FORMAT [ARGUMENT]...\n");
        return EXIT_FAILURE;
    }

    if (isHelp) {
        printf(
            "Usage:\n"
            "   printx [OPTIONS] FORMAT [ARGUMENT]...\n"
            "\n"
            "OPTIONS:\n"
            "   -h, --help         print this help\n"
            "   -v, --version      print version\n"
            "   -d, --debug        enable debug logging\n"
            "   -a, --assing VAR   assign result to variable VAR\n"
            "\n"
            "FORMAT sequences:\n"
            "   %%s   ARGUMENT is printed as string\n"
            "   %%%%   a single %%\n"
        );

        return EXIT_SUCCESS;
    }

    if (isVersion) {
        printf("%s\n", GIT_VERSION);
    }

    return EXIT_SUCCESS;
}