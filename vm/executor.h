#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "stack/stack.h"

typedef enum ExecError
{
    EXECUTOR_OK,
    EXECUTOR_DIVISION_BY_ZERO,
    EXECUTOR_SQRT_FROM_NEGATIVE_NUMBER
} ExecError;

static const char *STR_EXEC_ERRORS[] = {
    "EXECUTOR_OK",
    "EXECUTOR_DIVISION_BY_ZERO",
    "EXECUTOR_SQRT_FROM_NEGATIVE_NUMBER",
};

// ограничения - 32 значения StackError и 8 значений ExecError
typedef struct ExecutorError
{
    StackError stack_error : 5;
    ExecError executor_error : 3;
} ExecutorError;

// структура из vm.h, сам файл не инклудится из за ошибок включения
struct VM;

// API для взаимодействия с процессором
ExecutorError einit(VM *vm);
ExecutorError ehlt(VM *vm);

ExecutorError epush(VM *vm, stack_el_t value);

ExecutorError eadd(VM *vm);
ExecutorError esub(VM *vm);
ExecutorError emul(VM *vm);
ExecutorError ediv(VM *vm);

ExecutorError eabs(VM *vm);
ExecutorError esqrt(VM *vm);

ExecutorError eout(VM *vm);

const char *get_exec_error(ExecError error);

#endif // EXECUTOR_H
