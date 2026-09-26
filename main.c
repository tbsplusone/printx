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
    bool* isDebug
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
        else {
            fprintf(stderr, "error: unknown option %s\n", argv[i]);
            return -1;
        }
    }
    return i;
}

void print_usage(void) {
    fprintf(stderr, "usage: printx [-h, --help | -v, --version] FORMAT [ARGUMENT]...\n");
}

typedef struct {
    char* buff;
    int capacity;
    int length;
} DynamicArr;

DynamicArr dynamic_arr_init(int numChars) {
    DynamicArr arr;
    arr.buff = malloc(sizeof(char) * numChars);
    arr.capacity = numChars;
    arr.length = 0;
    return arr;
}

void dynamic_arr_destroy(DynamicArr *arr) {
    if (arr->buff != nullptr) {
        free(arr->buff);
        arr->buff = nullptr;
    }
    arr->capacity = 0;
    arr->length = 0;
}

void dynamic_arr_add(DynamicArr *arr, char ch) {
    if (arr->length == arr->capacity) {
        arr->capacity = arr->capacity == 0 ? 8 : arr->capacity * 2;
        arr->buff = realloc(arr->buff, sizeof(char) * arr->capacity);
    }
    arr->buff[arr->length] = ch;
    arr->length++;
}

void dynamic_arr_add_string(DynamicArr *arr, char* str) {
    // TODO: can we make it faster?
    for (; *str != '\0'; str++) {
        dynamic_arr_add(arr, *str);
    }
}

int main([[maybe_unused]] int argc, char** argv) {
    bool isHelp = false;
    bool isVersion = false;
    bool isDebug = false;
    int argIdx = opt_parse(argv, &isHelp, &isVersion, &isDebug);

    if (isDebug) {
        printf("isHelp = %s\n", isHelp ? "true": "false");
        printf("isVersion = %s\n", isVersion ? "true": "false");
        printf("isDebug = %s\n", isDebug ? "true": "false");
        printf("argIdx = %d\n", argIdx);
    }

    if (argIdx < 0) {
        print_usage();
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
            "\n"
            "FORMAT sequences:\n"
            "   \\n   prints a newline\n"
            "   \\\\   prints a single \\\n"
            "   %%s   prints ARGUMENT as string\n"
            "   %%%%   prints a single %%\n"
        );

        return EXIT_SUCCESS;
    }

    if (isVersion) {
        printf("%s\n", GIT_VERSION);
        return EXIT_SUCCESS;
    }

    if (argv[argIdx] == nullptr) {
        fprintf(stderr, "error: format not specified\n");
        print_usage();
        return EXIT_FAILURE;
    }
    char* format = argv[argIdx++];
    if (isDebug) {
        printf("format = %s\n", format);
    }

    DynamicArr output = dynamic_arr_init(1024);
    for (char* curr = format; *curr != '\0'; curr++) {
        if (*curr == '\\') {
            if (*(curr+1) == 'n') {
                dynamic_arr_add(&output, '\n');
                curr++;
                continue;
            }
            if (*(curr+1) == '\\') {
                dynamic_arr_add(&output, '\\');
                curr++;
                continue;
            }
        }
        if (*curr == '%') {
            if (*(curr+1) == 's') {
                if (argv[argIdx] != nullptr) {
                    dynamic_arr_add_string(&output, argv[argIdx++]);
                }
                curr++;
                continue;
            }
            if (*(curr+1) == '%') {
                dynamic_arr_add(&output, '%');
                curr++;
                continue;
            }
        }
        dynamic_arr_add(&output, *curr);
    }
    dynamic_arr_add(&output, '\0');
    printf("%s", output.buff);

    dynamic_arr_destroy(&output);
    return EXIT_SUCCESS;
}