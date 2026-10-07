#include "executor.h"
#include <assert.h>
#include <math.h>
#include "stack/stack.h"
#include "log.h"

static Stack stack = {};

ExecutorError einit(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    error.stack_error = init_stack(&stack, START_STACK_CAPACITY);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack initialization: %s", get_stack_error(error.stack_error));
    }

    FLOG("After init:");
    dump_stack(&stack);

    return error;
}

ExecutorError ehlt(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    error.stack_error = destroy_stack(&stack);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack destroing: %s", get_stack_error(error.stack_error));
    }

    FLOG("After hlt:");
    dump_stack(&stack);

    return error;
}

ExecutorError epush(stack_el_t value)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    error.stack_error = push_stack(&stack, value * DELTA);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", value * DELTA, get_stack_error(error.stack_error));
    }

    FLOG("After push:");
    dump_stack(&stack);

    return error;
}

ExecutorError eadd(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t x = 0, y = 0;

    error.stack_error = pop_stack(&stack, &y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping first value : %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = pop_stack(&stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping second value : %s", get_stack_error(error.stack_error));

        // вернуть на место забранный ранее y
        push_stack(&stack, y);
        return error;
    }

    error.stack_error = push_stack(&stack, x + y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x + y, get_stack_error(error.stack_error));
    }

    FLOG("After add:");
    dump_stack(&stack);

    return error;
}

ExecutorError esub(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    // x лежит выше в стэке, то есть положен туда раньше
    stack_el_t x = 0, y = 0;

    error.stack_error = pop_stack(&stack, &y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping first value : %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = pop_stack(&stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping second value : %s", get_stack_error(error.stack_error));

        // вернуть на место забранный ранее y
        push_stack(&stack, y);
        return error;
    }

    error.stack_error = push_stack(&stack, x - y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x - y, get_stack_error(error.stack_error));
    }

    FLOG("After sub:");
    dump_stack(&stack);

    return error;
}

ExecutorError emult(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t x = 0, y = 0;

    error.stack_error = pop_stack(&stack, &y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping first value : %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = pop_stack(&stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping second value : %s", get_stack_error(error.stack_error));

        // вернуть на место забранный ранее y
        push_stack(&stack, y);
        return error;
    }

    // деление на дельту во время пуша чтобы поддерживать точность
    error.stack_error = push_stack(&stack, x * y / DELTA);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x * y / DELTA, get_stack_error(error.stack_error));
    }

    FLOG("After mult:");
    dump_stack(&stack);

    return error;
}

ExecutorError ediv(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    // x лежит выше в стэке, то есть положен туда раньше
    stack_el_t x = 0, y = 0;

    error.stack_error = pop_stack(&stack, &y);
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
        push_stack(&stack, y);
        return error;
    }

    error.stack_error = pop_stack(&stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping second value : %s", get_stack_error(error.stack_error));

        // вернуть на место забранный ранее y
        push_stack(&stack, y);
        return error;
    }

    error.stack_error = push_stack(&stack, x * DELTA / y);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x * DELTA / y, get_stack_error(error.stack_error));
    }

    FLOG("After div:");
    dump_stack(&stack);

    return error;
}

ExecutorError eabs(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t x = 0;

    error.stack_error = pop_stack(&stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping value : %s", get_stack_error(error.stack_error));
        return error;
    }

    // магия
    x *= -1 + 2 * (x >= 0);

    error.stack_error = push_stack(&stack, x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x, get_stack_error(error.stack_error));
    }

    FLOG("After abs:");
    dump_stack(&stack);

    return error;
}

ExecutorError esqrt(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t x = 0;

    error.stack_error = pop_stack(&stack, &x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping value : %s", get_stack_error(error.stack_error));
        return error;
    }

    x = (stack_el_t)(DELTA * sqrt((double)x / DELTA));

    error.stack_error = push_stack(&stack, x);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", x, get_stack_error(error.stack_error));
    }

    FLOG("After abs:");
    dump_stack(&stack);

    return error;
}

// распечатать верхнее значение в стеке
ExecutorError eout(void)
{
    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    stack_el_t value = 0;

    error.stack_error = pop_stack(&stack, &value);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping: %s", get_stack_error(error.stack_error));
        return error;
    }

    printf("%lg\n", (double)value / DELTA);

    error.stack_error = push_stack(&stack, value);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing: %s", get_stack_error(error.stack_error));
    }

    FLOG("After out:");
    dump_stack(&stack);

    return error;
}

const char *get_exec_error(ExecError error)
{
    return STR_EXEC_ERRORS[error];
}