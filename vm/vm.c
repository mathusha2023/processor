#include "vm.h"
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include "executor.h"
#include "murmur3.h"

static struct Command get_next_command(VM *vm);
static off_t get_file_size(FILE *file);
static inline size_t min(size_t a, size_t b);

VMError init_vm(VM *vm, vm_el_t *ram_p)
{
    assert(vm);
    assert(ram_p);

    VMError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK, .vm_error = VM_OK};

    vm->code_length = 0;
    vm->IP = 0;

    error.stack_error = init_stack(&vm->stack, START_STACK_CAPACITY);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in vm stack initialization: %s", get_stack_error(error.stack_error));
        return error;
    }

    error.stack_error = init_stack(&vm->ret_addresses_stack, START_STACK_CAPACITY);
    if (error.stack_error != STACK_OK)
    {
        LOG("Error in ret_addresses_stack initialization: %s", get_stack_error(error.stack_error));
        return error;
    }

    vm->registers_count = REGISTERS_COUNT;
    vm->RAM = ram_p;

    return error;
}

VMError load_program(VM *vm, const char *filename)
{
    assert(vm);
    assert(filename);

    VMError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK, .vm_error = VM_OK};

    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        LOG("Cant open file %s", filename);
        error.vm_error = VM_CANT_OPEN_PROGRAM_FILE;
        return error;
    }

    off_t file_size = get_file_size(file);
    if (file_size < 0)
    {
        LOG("Cant get size of file %s", filename);
        fclose(file);
        error.vm_error = VM_CANT_GET_FILE_SIZE;
        return error;
    }

    // magic numbers
    char magic_numbers_arr[MAGIC_LENGTH] = {};
    size_t read_count = fread(magic_numbers_arr, sizeof(char), MAGIC_LENGTH, file);
    if (read_count != MAGIC_LENGTH)
    {
        LOG("Error while reading program file %s: read %lu bytes but %lu expected", filename, read_count, MAGIC_LENGTH);
        fclose(file);
        error.vm_error = VM_ERROR_READING_FILE;
        return error;
    }
    for (size_t i = 0; i < MAGIC_LENGTH; i++)
    {
        if (magic_numbers_arr[i] != MAGIC[i])
        {
            LOG("File header magic numbers are not same!");
            fclose(file);
            error.vm_error = VM_BAD_MAGIC_NUMBERS;
            return error;
        }
    }

    // version
    char version_arr[VERSION_LENGTH] = {};
    read_count = fread(version_arr, sizeof(char), VERSION_LENGTH, file);
    if (read_count != VERSION_LENGTH)
    {
        LOG("Error while reading program file %s: read %lu bytes but %lu expected", filename, read_count, VERSION_LENGTH);
        fclose(file);
        error.vm_error = VM_ERROR_READING_FILE;
        return error;
    }
    for (size_t i = 0; i < VERSION_LENGTH; i++)
    {
        if (version_arr[i] > VERSION[i])
        {
            LOG("File version is higher than vm can execute!");
            fclose(file);
            error.vm_error = VM_HIGHER_VERSION;
            return error;
        }
    }

    // code length
    uint32_t code_length = 0;
    read_count = fread(&code_length, sizeof(code_length), 1, file);
    if (read_count != 1)
    {
        LOG("Error while reading program file %s: read %lu bytes but %lu expected", filename, read_count, 1);
        fclose(file);
        error.vm_error = VM_ERROR_READING_FILE;
        return error;
    }

    // hash
    uint32_t code_file_hash = 0;
    read_count = fread(&code_file_hash, sizeof(code_file_hash), 1, file);
    if (read_count != 1)
    {
        LOG("Error while reading program file %s: read %lu bytes but %lu expected", filename, read_count, 1);
        fclose(file);
        error.vm_error = VM_ERROR_READING_FILE;
        return error;
    }

    // code
    read_count = fread(vm->code, sizeof(vm_el_t), min(code_length, MAX_CODE_LENGTH), file);
    if (read_count != code_length)
    {
        LOG("Error while reading program file %s: read %lu commands but %lu expected", filename, read_count, code_length);
        fclose(file);
        error.vm_error = VM_ERROR_READING_FILE;
        return error;
    }

    fclose(file);

    uint32_t code_real_hash = 0;
    MurmurHash3_x86_32(vm->code, (int)(code_length * sizeof(vm_el_t)), SEED, &code_real_hash);
    if (code_file_hash != code_real_hash)
    {
        LOG("Error: file hash and read hash are not equal!");
        error.vm_error = VM_INVALID_HASH;
        return error;
    }

    vm->code_length = code_length;

    return error;
}

VMError execute_program(VM *vm)
{
    assert(vm);
    assert(vm->code);
    assert(vm->code_length);

    VMError vm_error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK, .vm_error = VM_OK};

    while (vm->IP < vm->code_length)
    {
        get_next_command(vm);

        ExecutorError error = execute_command(vm);
        if (error.executor_error == EXECUTOR_END_PROGRAM)
            break;

        if (error.stack_error || error.executor_error)
        {
            LOG("ERROR while executing command %lu", vm->IP);

            vm_error.stack_error = error.stack_error;
            vm_error.executor_error = error.executor_error;
            vm_error.vm_error = VM_ERROR_EXECUTE_COMMAND;
            break;
        }
    }
    return vm_error;
}

VMError babah_vm(VM *vm)
{
    assert(vm);

    VMError error = {.stack_error = STACK_OK, .executor_error = EXECUTOR_OK, .vm_error = VM_OK};

    if (is_stack_empty(&vm->stack) != STACK_OK)
    {
        error.stack_error = destroy_stack(&vm->stack);
        if (error.stack_error)
        {
            LOG("Error while destroing vm: %s", get_stack_error(error.stack_error));
            return error;
        }
    }

    if (is_stack_empty(&vm->ret_addresses_stack) != STACK_OK)
    {
        if (error.stack_error)
        {
            LOG("Error while destroing ret_addresses_stack: %s", get_stack_error(error.stack_error));
            return error;
        }
    }

    return error;
}

// ExecutorError execute_command(VM *vm);
#include "command_switch.h"

const char *get_vm_error(VirtualMachineError error)
{
    return STR_VM_ERRORS[error];
}

void print_vm_error(VMError error)
{
    LOG("ERROR HAPPENS:");
    LOG("Stack error: %s", get_stack_error(error.stack_error));
    LOG("Executor error: %s", get_exec_error(error.executor_error));
    LOG("VM error: %s", get_vm_error(error.vm_error));
}

static struct Command get_next_command(VM *vm)
{
    assert(vm);

    assert(vm->IP < vm->code_length);
    enum Commands cmd = (enum Commands)vm->code[vm->IP++];

    struct Command const_command = commands[cmd];
    assert(cmd == const_command.cmd);

    size_t args_count = const_command.cmd_args_count;

    struct Command final_command = {};

    switch (args_count)
    {
    case 0:
        final_command.cmd = cmd;
        break;
    case 1:
        final_command.cmd = cmd;
        final_command.cmd_args_count = 1;

        assert(vm->IP < vm->code_length);
        final_command.arg1 = vm->code[vm->IP++];
        break;
    case 2:
        final_command.cmd = cmd;
        final_command.cmd_args_count = 2;

        assert(vm->IP < vm->code_length);
        final_command.arg1 = vm->code[vm->IP++];

        assert(vm->IP < vm->code_length);
        final_command.arg2 = vm->code[vm->IP++];
        break;
    default:
        LOG("ERROR: COMMAND FROM CONFIG HAS %lu ARGUMENTS!!!", args_count);
    }
    vm->current_command = final_command;
    return final_command;
}

// < 0 если ошибка, иначе > 0
static off_t get_file_size(FILE *file)
{
    assert(file);

    struct stat st = {};

    // произошла ошибка
    if (fstat(fileno(file), &st))
    {
        LOG("Cant get file size");
        return -1;
    }

    return st.st_size;
}

static inline size_t min(size_t a, size_t b)
{
    return a < b ? a : b;
}