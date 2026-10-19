#include "executor.h"
#include <assert.h>
#include <math.h>
#include "vm.h"
#include "config.h"
#include "log.h"

ExecutorError ehlt(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == HLT);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    error.stack_error = destroy_stack(&vm->stack);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack destroing: %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = destroy_stack(&vm->ret_addresses_stack);
    if (error.stack_error)
    {
        LOG("Error while destroing ret_addresses_stack: %s", get_stack_error(error.stack_error));
        return error;
    }

    FLOG("After hlt:");
    dump_stack(&vm->stack);

    error.executor_error = EXECUTOR_END_PROGRAM;

    return error;
}

ExecutorError epush_number(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == PUSH_NUMBER);

    vm_el_t value = vm->current_command.arg1;

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
    assert(vm);
    assert(vm->current_command.cmd == ADD);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t x = 0, y = 0;

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
    assert(vm);
    assert(vm->current_command.cmd == SUB);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    // x лежит выше в стэке, то есть положен туда раньше
    vm_el_t x = 0, y = 0;

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
    assert(vm);
    assert(vm->current_command.cmd == MUL);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t x = 0, y = 0;

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

    long long res = (long long)x * (long long)y / DELTA;

    // деление на дельту во время пуша чтобы поддерживать точность
    error.stack_error = push_stack(&vm->stack, (vm_el_t)res);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", (vm_el_t)res, get_stack_error(error.stack_error));
    }

    FLOG("After mult:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError ediv(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == DIV);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    // x лежит выше в стэке, то есть положен туда раньше
    vm_el_t x = 0, y = 0;

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

    long long res = (long long)x * DELTA / y;

    error.stack_error = push_stack(&vm->stack, (vm_el_t)res);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", (vm_el_t)res, get_stack_error(error.stack_error));
    }

    FLOG("After div:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError eabs(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == ABS);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t x = 0;

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
    assert(vm);
    assert(vm->current_command.cmd == SQRT);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t x = 0;

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

    x = (vm_el_t)(DELTA * sqrt((double)x / DELTA));

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
    assert(vm);
    assert(vm->current_command.cmd == OUT);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t value = 0;

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

ExecutorError eoutc(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == OUTC);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t value = 0;

    error.stack_error = pop_stack(&vm->stack, &value);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping: %s", get_stack_error(error.stack_error));
        return error;
    }

    printf("%c", (char)(value / DELTA));

    FLOG("After out:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError epop_register(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == POP_REGISTER);

    vm_el_t register_num = vm->current_command.arg1;
    assert(0 <= register_num && register_num < (vm_el_t)vm->registers_count);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t value = 0;

    error.stack_error = pop_stack(&vm->stack, &value);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping value: %s", get_stack_error(error.stack_error));
        return error;
    }
    vm->registers[register_num] = value;

    FLOG("After pop:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError epush_register(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == PUSH_REGISTER);

    vm_el_t register_num = vm->current_command.arg1;
    assert(0 <= register_num && register_num < (vm_el_t)vm->registers_count);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    error.stack_error = push_stack(&vm->stack, vm->registers[register_num]);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", vm->registers[register_num], get_stack_error(error.stack_error));
    }

    FLOG("After push register:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError ejmp(VM *vm)
{
    assert(vm);
    assert(0 <= vm->current_command.arg1 && (size_t)vm->current_command.arg1 < vm->code_length);

    enum Commands cmd = vm->current_command.cmd;
    assert(cmd == JMP || cmd == CALL || cmd == RET || cmd == JA || cmd == JB || cmd == JAE || cmd == JBE || cmd == JE || cmd == JNE);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm->IP = (size_t)vm->current_command.arg1;
    return error;
}

ExecutorError ejmp_condition(VM *vm)
{
    assert(vm);
    assert(0 <= vm->current_command.arg1 && (size_t)vm->current_command.arg1 < vm->code_length);

    enum Commands cmd = vm->current_command.cmd;
    assert(cmd == JA || cmd == JB || cmd == JAE || cmd == JBE || cmd == JE || cmd == JNE);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t x = 0, y = 0;

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

    if (cmd == JA && x > y)
        return ejmp(vm);

    if (cmd == JB && x < y)
        return ejmp(vm);

    if (cmd == JAE && x >= y)
        return ejmp(vm);

    if (cmd == JBE && x <= y)
        return ejmp(vm);

    if (cmd == JE && x == y)
        return ejmp(vm);

    if (cmd == JNE && x != y)
        return ejmp(vm);

    return error;
}

ExecutorError ecall(VM *vm)
{
    assert(vm);
    assert(0 <= vm->current_command.arg1 && (size_t)vm->current_command.arg1 < vm->code_length);
    assert(vm->current_command.cmd == CALL);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    error.stack_error = push_stack(&vm->ret_addresses_stack, (stack_el_t)vm->IP);
    if (error.stack_error != STACK_OK)
    {
        LOG("Ecall: error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", vm->IP, get_stack_error(error.stack_error));
        return error;
    }

    return ejmp(vm);
}

ExecutorError eret(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == RET);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t return_address = 0;

    error.stack_error = pop_stack(&vm->ret_addresses_stack, &return_address);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping return address: %s", get_stack_error(error.stack_error));
        return error;
    }

    vm->IP = (size_t)return_address;

    return error;
}

ExecutorError ein(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == IN);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    printf(">>> ");

    vm_el_t value = 0;
    scanf(VM_EL_SPECIFICATOR, &value);

    error.stack_error = push_stack(&vm->stack, value * DELTA);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", value * DELTA, get_stack_error(error.stack_error));
    }

    FLOG("After in:");
    dump_stack(&vm->stack);

    return error;
}

ExecutorError edora(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == DORA);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    printf("VSE POTOMU CHTO DORA DURA!!!!\n");

    return error;
}

ExecutorError ernd(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == RND);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t random_value = rand();

    error.stack_error = push_stack(&vm->stack, random_value);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", random_value, get_stack_error(error.stack_error));
    }

    return error;
}

ExecutorError epush_memory_int(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == PUSH_MEMORY_INT);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t mem_addr = vm->current_command.arg1;

    error.stack_error = push_stack(&vm->stack, vm->RAM[mem_addr]);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s", vm->RAM[mem_addr], get_stack_error(error.stack_error));
    }

    return error;
}

ExecutorError epush_memory_register(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == PUSH_MEMORY_REGISTER);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t register_num = vm->current_command.arg1;
    assert(0 <= register_num && register_num < (vm_el_t)vm->registers_count);

    error.stack_error = push_stack(&vm->stack, vm->RAM[vm->registers[register_num] / DELTA]);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack pushing value " STACK_EL_SPECIFICATOR ": %s",
            vm->RAM[vm->registers[register_num] / DELTA],
            get_stack_error(error.stack_error));
    }

    return error;
}

ExecutorError epop_memory_int(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == POP_MEMORY_INT);

    vm_el_t mem_adr = vm->current_command.arg1;

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t value = 0;

    error.stack_error = pop_stack(&vm->stack, &value);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping value: %s", get_stack_error(error.stack_error));
        return error;
    }

    vm->RAM[mem_adr] = value;
    return error;
}

ExecutorError epop_memory_register(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == POP_MEMORY_REGISTER);

    vm_el_t register_num = vm->current_command.arg1;
    assert(0 <= register_num && register_num < (vm_el_t)vm->registers_count);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm_el_t value = 0;

    error.stack_error = pop_stack(&vm->stack, &value);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in stack popping value: %s", get_stack_error(error.stack_error));
        return error;
    }

    vm->RAM[vm->registers[register_num] / DELTA] = value;
    return error;
}

ExecutorError edec(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == DEC);

    vm_el_t register_num = vm->current_command.arg1;
    assert(0 <= register_num && register_num < (vm_el_t)vm->registers_count);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm->registers[register_num] -= DELTA;

    return error;
}

ExecutorError einc(VM *vm)
{
    assert(vm);
    assert(vm->current_command.cmd == INC);

    vm_el_t register_num = vm->current_command.arg1;
    assert(0 <= register_num && register_num < (vm_el_t)vm->registers_count);

    ExecutorError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK};

    vm->registers[register_num] += DELTA;

    return error;
}

const char *get_exec_error(ExecError error)
{
    return STR_EXEC_ERRORS[error];
}