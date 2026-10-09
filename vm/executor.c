#include "executor.h"
#include <assert.h>
#include <math.h>
#include "vm.h"
#include "config.h"
#include "log.h"

ExecutorError ehlt(VM *vm)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    error.stack_error = destroy_stack(&vm->stack);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack destroing: %s", get_stack_error(error.stack_error));
    }

    FLOG("After hlt:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError epush(VM *vm, stack_el_t value)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    error.stack_error = push_stack(&vm->stack, value * DELTA);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", value * DELTA, get_stack_error(error.stack_error));
    }

    FLOG("After push:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError eadd(VM *vm)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t x = 0, y = 0;

    error.stack_error = pop_stack(&vm->stack, &y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping first value : %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = pop_stack(&vm->stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping second value : %s", get_stack_error(error.stack_error));

        // вернуть на место забранный ранее y
        push_stack(&vm->stack, y);
        return error;
    }

    error.stack_error = push_stack(&vm->stack, x + y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x + y, get_stack_error(error.stack_error));
    }

    FLOG("After add:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError esub(VM *vm)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    // x лежит выше в стэке, то есть положен туда раньше
    stack_el_t x = 0, y = 0;

    error.stack_error = pop_stack(&vm->stack, &y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping first value : %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = pop_stack(&vm->stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping second value : %s", get_stack_error(error.stack_error));

        // вернуть на место забранный ранее y
        push_stack(&vm->stack, y);
        return error;
    }

    error.stack_error = push_stack(&vm->stack, x - y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x - y, get_stack_error(error.stack_error));
    }

    FLOG("After sub:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError emul(VM *vm)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t x = 0, y = 0;

    error.stack_error = pop_stack(&vm->stack, &y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping first value : %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = pop_stack(&vm->stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping second value : %s", get_stack_error(error.stack_error));

        // вернуть на место забранный ранее y
        push_stack(&vm->stack, y);
        return error;
    }

    // деление на дельту во время пуша чтобы поддерживать точность
    error.stack_error = push_stack(&vm->stack, x * y / DELTA);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x * y / DELTA, get_stack_error(error.stack_error));
    }

    FLOG("After mult:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError ediv(VM *vm)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    // x лежит выше в стэке, то есть положен туда раньше
    stack_el_t x = 0, y = 0;

    error.stack_error = pop_stack(&vm->stack, &y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping first value : %s", get_stack_error(error.stack_error));
        return error;
    }

    if (y == 0)
    {
        error.executor_error = EXECUTOR_DIVISION_BY_ZERO;
        LOG("Error: division by zero!");

        // вернуть на место забранный ранее y
        push_stack(&vm->stack, y);
        return error;
    }

    error.stack_error = pop_stack(&vm->stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping second value : %s", get_stack_error(error.stack_error));

        // вернуть на место забранный ранее y
        push_stack(&vm->stack, y);
        return error;
    }

    error.stack_error = push_stack(&vm->stack, x * DELTA / y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x * DELTA / y, get_stack_error(error.stack_error));
    }

    FLOG("After div:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError eabs(VM *vm)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t x = 0;

    error.stack_error = pop_stack(&vm->stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping value : %s", get_stack_error(error.stack_error));
        return error;
    }

    // магия
    x *= -1 + 2 * (x >= 0);

    error.stack_error = push_stack(&vm->stack, x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x, get_stack_error(error.stack_error));
    }

    FLOG("After abs:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError esqrt(VM *vm)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t x = 0;

    error.stack_error = pop_stack(&vm->stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping value : %s", get_stack_error(error.stack_error));
        return error;
    }

    if (x < 0)
    {
        error.executor_error = EXECUTOR_SQRT_FROM_NEGATIVE_NUMBER;
        LOG("Error: square root from negative number!");

        // вернуть на место забранный ранее x
        push_stack(&vm->stack, x);
        return error;
    }

    x = (stack_el_t)(DELTA * sqrt((double)x / DELTA));

    error.stack_error = push_stack(&vm->stack, x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x, get_stack_error(error.stack_error));
    }

    FLOG("After abs:");
    dump_stack(&vm->stack);

    return error;
}

// распечатать верхнее значение в стеке
ExecutorError eout(VM *vm)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t value = 0;

    error.stack_error = pop_stack(&vm->stack, &value);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping: %s", get_stack_error(error.stack_error));
        return error;
    }

    printf("%lg\n", (double)value / DELTA);

    FLOG("After out:");
    dump_stack(&vm->stack);

    return error;
}

const char *get_exec_error(ExecError error)
{
    return STR_EXEC_ERRORS[error];
}