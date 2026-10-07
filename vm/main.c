#include "interpreter.h"

int main(void)
{
    restart_log();

    interpreter_loop();

    return 0;
}