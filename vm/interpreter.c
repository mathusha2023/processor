#include "interpreter.h"
#include <string.h>
#include "executor.h"
#include "config.h"

static InterpreterError execute_void_command(int sscanf_res, ExecutorError (*command)(void));
static InterpreterError execute_one_arg_command(int sscanf_res, ExecutorError (*command)(stack_el_t arg), stack_el_t arg);
static InterpreterError execute_hlt_command(int sscanf_res);

InterpreterError interpreter_loop(void)
{
    ExecutorError eerror = einit();
    if (eerror.executor_error != EXECUTOR_OK)
    {
        LOG("Executor error: %s", get_exec_error(eerror.executor_error));
        return INTERPRETER_EXECUTOR_ERROR;
    }
    if (eerror.stack_error != STACK_OK)
    {
        LOG("Stack error: %s", get_stack_error(eerror.stack_error));
        return INTERPRETER_STACK_ERROR;
    }

    while (1)
    {
        InterpreterError error = process_one_command();

        if (error == INTERPRETER_END_PROGRAMM)
        {
            return error;
        }

        if (error)
        {
            LOG("Error while processing command: %s", get_interpreter_error(error));
        }
    }
}

InterpreterError process_one_command(void)
{
    char buffer[MAX_COMMAND_LENGTH] = {};
    char str[MAX_COMMAND_LENGTH] = {};
    stack_el_t arg = 0;

    if (!fgets(buffer, sizeof(buffer), stdin))
    {
        LOG("Cant read command(");
        return INTERPRETER_CANT_READ;
    }

    int res = sscanf(buffer, "%s " STACK_EL_SPECIFICATOR "\n", str, &arg);

    if (res == -1)
    {
        LOG("Cant read command: input format is not corrected");
        return INTERPRETER_CANT_READ;
    }

    enum Commands command = get_command(str);

    switch (command)
    {
    case C_PUSH:
        return execute_one_arg_command(res, epush, arg);
    case C_ADD:
        return execute_void_command(res, eadd);
    case C_SUB:
        return execute_void_command(res, esub);
    case C_MULT:
        return execute_void_command(res, emult);
    case C_DIV:
        return execute_void_command(res, ediv);
    case C_ABS:
        return execute_void_command(res, eabs);
    case C_SQRT:
        return execute_void_command(res, esqrt);
    case C_OUT:
        return execute_void_command(res, eout);
    case C_HLT:
        return execute_hlt_command(res);

    case C_UNKNOWN_COMMAND:
    default:
        LOG("Unknown command: %s", buffer);
        return INTERPRETER_UNKNOWN_COMMAND;
    }
}

enum Commands get_command(char *command)
{
    if (strcmp(command, "HLT") == 0)
        return C_HLT;
    else if (strcmp(command, "PUSH") == 0)
        return C_PUSH;
    else if (strcmp(command, "ADD") == 0)
        return C_ADD;
    else if (strcmp(command, "SUB") == 0)
        return C_SUB;
    else if (strcmp(command, "MULT") == 0)
        return C_MULT;
    else if (strcmp(command, "DIV") == 0)
        return C_DIV;
    else if (strcmp(command, "ABS") == 0)
        return C_ABS;
    else if (strcmp(command, "SQRT") == 0)
        return C_SQRT;
    else if (strcmp(command, "OUT") == 0)
        return C_OUT;
    else
    {
        LOG("Unknown command: %s", command);
        return C_UNKNOWN_COMMAND;
    }
}

const char *get_interpreter_error(InterpreterError error)
{
    return STR_INTERPRETER_ERRORS[error];
}

static InterpreterError execute_void_command(int sscanf_res, ExecutorError (*command)(void))
{
    if (sscanf_res != 1)
    {
        LOG("Incorrect command format: invalid args count: need 0");
        return INTERPRETER_INCORRECT_ARGS;
    }

    ExecutorError error = command();

    if (error.executor_error != EXECUTOR_OK)
    {
        LOG("Executor error: %s", get_exec_error(error.executor_error));
        return INTERPRETER_EXECUTOR_ERROR;
    }
    if (error.stack_error != STACK_OK)
    {
        LOG("Stack error: %s", get_stack_error(error.stack_error));
        return INTERPRETER_STACK_ERROR;
    }

    return INTERPRETER_OK;
}

static InterpreterError execute_one_arg_command(int sscanf_res, ExecutorError (*command)(stack_el_t arg), stack_el_t arg)
{
    if (sscanf_res != 2)
    {
        LOG("Incorrect command format: invalid args count: need 1");
        return INTERPRETER_INCORRECT_ARGS;
    }

    ExecutorError error = command(arg);

    if (error.executor_error != EXECUTOR_OK)
    {
        LOG("Executor error: %s", get_exec_error(error.executor_error));
        return INTERPRETER_EXECUTOR_ERROR;
    }
    if (error.stack_error != STACK_OK)
    {
        LOG("Stack error: %s", get_stack_error(error.stack_error));
        return INTERPRETER_STACK_ERROR;
    }

    return INTERPRETER_OK;
}

static InterpreterError execute_hlt_command(int sscanf_res)
{
    if (sscanf_res != 1)
    {
        LOG("Incorrect command format: invalid args count - need 0");
        return INTERPRETER_INCORRECT_ARGS;
    }

    ExecutorError error = ehlt();

    if (error.executor_error != EXECUTOR_OK)
    {
        LOG("Executor error: %s", get_exec_error(error.executor_error));
        return INTERPRETER_EXECUTOR_ERROR;
    }
    if (error.stack_error != STACK_OK)
    {
        LOG("Stack error: %s", get_stack_error(error.stack_error));
        return INTERPRETER_STACK_ERROR;
    }

    return INTERPRETER_END_PROGRAMM;
}