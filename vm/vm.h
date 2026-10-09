#ifndef VM_H
#define VM_H

#include "commands_table.h"
#include "stack/stack.h"
#include "executor.h"
#include "config.h"

typedef enum VirtualMachineError
{
    VM_OK,
    VM_CANT_OPEN_PROGRAM_FILE,
    VM_CANT_GET_FILE_SIZE,
    VM_ERROR_READING_FILE,
    VM_ERROR_EXECUTE_COMMAND,
} VirtualMachineError;

static const char *STR_VM_ERRORS[] = {
    "VM_OK",
    "VM_CANT_OPEN_PROGRAM_FILE",
    "VM_CANT_GET_FILE_SIZE",
    "VM_ERROR_READING_FILE",
    "VM_ERROR_EXECUTE_COMMAND",
};

typedef struct VMError
{
    StackError stack_error;
    ExecError executor_error;
    VirtualMachineError vm_error;
} VMError;

typedef struct VM
{
    stack_el_t code[MAX_CODE_LENGTH];
    size_t code_length;
    Stack stack;
    size_t IP;
    struct Command current_command;
} VM;

const size_t START_STACK_CAPACITY = 20;

VMError init_vm(VM *vm);
VMError load_program(VM *vm, const char *filename);
VMError execute_program(VM *vm);
VMError babah_vm(VM *vm);
ExecutorError execute_command(VM *vm);

const char *get_vm_error(VirtualMachineError error);
void print_vm_error(VMError error);

#endif // VM_H