#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "stack/stack.h"

typedef enum ExecError
{
    EXECUTOR_OK,
    EXECUTOR_END_PROGRAM,
    EXECUTOR_DIVISION_BY_ZERO,
    EXECUTOR_SQRT_FROM_NEGATIVE_NUMBER,
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

ExecutorError epush_number(VM *vm);

ExecutorError eadd(VM *vm);
ExecutorError esub(VM *vm);
ExecutorError emul(VM *vm);
ExecutorError ediv(VM *vm);

ExecutorError eabs(VM *vm);
ExecutorError esqrt(VM *vm);

ExecutorError eout(VM *vm);
ExecutorError eoutc(VM *vm);

ExecutorError epop_register(VM *vm);
ExecutorError epush_register(VM *vm);

ExecutorError ejmp(VM *vm);
ExecutorError ejmp_condition(VM *vm);

ExecutorError ecall(VM *vm);
ExecutorError eret(VM *vm);

ExecutorError ein(VM *vm);

ExecutorError edora(VM *vm);

ExecutorError ernd(VM *vm);

ExecutorError epush_memory_int(VM *vm);
ExecutorError epush_memory_register(VM *vm);
ExecutorError epop_memory_int(VM *vm);
ExecutorError epop_memory_register(VM *vm);

ExecutorError edec(VM *vm);
ExecutorError einc(VM *vm);

const char *get_exec_error(ExecError error);

#endif // EXECUTOR_H
