#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

// #define DISABLE_LOGS

typedef int asm_el_t;

#define RED_COLOR "\x1b[31m"
#define GREEN_COLOR "\x1b[32m"
#define BLUE_COLOR "\x1b[34m"
#define YELLOW_COLOR "\x1b[33m"
#define CYAN_COLOR "\x1b[36m"
#define GREY_COLOR "\033[90m"
#define RESET_COLOR "\x1b[0m"

#define INPUT_FILE_NAME_FLAG "--input"
#define OUTPUT_FILE_NAME_FLAG "--output"

#define free_ptr(p) \
    {               \
        free(p);    \
        p = NULL;   \
    }

const char DEFAULT_INPUT_FILE_NAME[] = "program.txt";
const char DEFAULT_OUTPUT_FILE_NAME[] = "res.baa";
const char DEFAULT_DEBUG_OUTPUT_FILE_NAME[] = "res-debug.baa.txt";
const char LOGFILE_NAME[] = "log.txt";

const size_t MAX_COMMAND_LENGTH = 100;
const size_t MAX_CODE_LENGTH = 1000;
const size_t MAX_COMMAND_ARG_LENGTH = 30;
const size_t MAX_LABELS_COUNT = 100;

const char COMMENT_CHAR = ';';

#endif // CONFIG_H
