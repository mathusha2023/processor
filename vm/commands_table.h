#ifndef COMMANDS_TABLE_H
#define COMMANDS_TABLE_H

#include <stddef.h>

enum Commands
{
    HLT = 0,
    PUSH_NUMBER = 1,
    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,
    ABS = 6,
    SQRT = 7,
    OUT = 8,
};

struct Command
{
    enum Commands cmd;
    size_t cmd_args_count;
    long long arg1;
    long long arg2;
};

const size_t N_COMMANDS = 9;

/*
Массив структур нужен по сути для того, чтобы легко получать количество аргументов каждой команды.
Простейшее решени - массив чисел, однако для удобства контроля был сделан массив структур.
Тогда можно сравнивать чтобы индекс массива сошелся с номером команды, что по идее должно уменьшить
количество случайных ошибок (время на их исправление)
*/
const static struct Command commands[N_COMMANDS] = {
    {.cmd = HLT},
    {.cmd = PUSH_NUMBER, .cmd_args_count = 1},
    {.cmd = ADD},
    {.cmd = SUB},
    {.cmd = MUL},
    {.cmd = DIV},
    {.cmd = ABS},
    {.cmd = SQRT},
    {.cmd = OUT},
};

#endif // COMMANDS_TABLE_H