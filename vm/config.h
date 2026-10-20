#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

// #define DISABLE_LOGS
// #define NSTKDEBUG

typedef int vm_el_t;
const char VM_EL_SPECIFICATOR[] = "%d";

#define RED_COLOR "\x1b[31m"
#define GREEN_COLOR "\x1b[32m"
#define BLUE_COLOR "\x1b[34m"
#define YELLOW_COLOR "\x1b[33m"
#define CYAN_COLOR "\x1b[36m"
#define GREY_COLOR "\033[90m"
#define RESET_COLOR "\x1b[0m"

#define free_ptr(p) \
    {               \
        free(p);    \
        p = NULL;   \
    }

// .baa format header
const size_t MAGIC_LENGTH = 4;
const char MAGIC[MAGIC_LENGTH] = {'D', 'O', 'R', 'A'};
const size_t VERSION_LENGTH = 2;
const char VERSION[VERSION_LENGTH] = {0, 1};
const uint32_t SEED = 52;

const char LOGFILE_NAME[] = "log.txt";
const char DEFAULT_PROGRAM_FILE_NAME[] = "../assembler/res.baa";
const double EPSILON = 1e-6;

// фиксированная точность вычислений
const vm_el_t DELTA = 1000;

const size_t MAX_COMMAND_LENGTH = 100;
const size_t MAX_CODE_LENGTH = 1000;
const size_t MEMORY_LENGTH = 169;

#endif // CONFIG_H