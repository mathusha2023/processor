#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "executor.h"

enum Commands
{
    C_HLT,
    C_PUSH,
    C_ADD,
    C_SUB,
    C_MULT,
    C_DIV,
    C_ABS,
    C_SQRT,
    C_OUT,
    C_UNKNOWN_COMMAND,
};

typedef enum InterpreterError
{
    INTERPRETER_OK,
    INTERPRETER_END_PROGRAMM,
    INTERPRETER_CANT_READ,
    INTERPRETER_UNKNOWN_COMMAND,
    INTERPRETER_INCORRECT_ARGS,
    INTERPRETER_EXECUTOR_ERROR,
    INTERPRETER_STACK_ERROR,
} InterpreterError;

static const char *STR_INTERPRETER_ERRORS[] = {
    "INTERPRETER_OK",
    "INTERPRETER_END_PROGRAMM",
    "INTERPRETER_CANT_READ",
    "INTERPRETER_UNKNOWN_COMMAND",
    "INTERPRETER_INCORRECT_ARGS",
    "INTERPRETER_EXECUTOR_ERROR",
    "INTERPRETER_STACK_ERROR",
};

InterpreterError interpreter_loop(void);
InterpreterError process_one_command(void);
enum Commands get_command(char *command);

const char *get_interpreter_error(InterpreterError error);

#endif // INTERPRETER_H