#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "stack/stack.h"

typedef enum ExecError
{
    EXECUTOR_OK,
    EXECUTOR_DIVISION_BY_ZERO,
} ExecError;

static const char *STR_EXEC_ERRORS[] = {
    "EXECUTOR_OK",
    "EXECUTOR_DIVISION_BY_ZERO",
};

// ограничения - 32 значения StackError и 8 значений ExecError
typedef struct ExecutorError
{
    StackError stack_error : 5;
    ExecError executor_error : 3;
} ExecutorError;

const size_t START_STACK_CAPACITY = 20;

// API для взаимодействия с процессором
ExecutorError einit(void);
ExecutorError ehlt(void);

ExecutorError epush(stack_el_t value);

ExecutorError eadd(void);
ExecutorError esub(void);
ExecutorError emult(void);
ExecutorError ediv(void);

ExecutorError eabs(void);
ExecutorError esqrt(void);

ExecutorError eout(void);

const char *get_exec_error(ExecError error);

#endif // EXECUTOR_H
