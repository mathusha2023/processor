#include "vm.h"
#include "config.h"
#include "log.h"

int main(int argc, char *argv[])
{
    restart_log();

    const char *program_file_name = argc > 1 ? argv[1] : DEFAULT_PROGRAM_FILE_NAME;

    VM vm = {};
    VMError error = {};

    error = init_vm(&vm);
    if (error.stack_error || error.executor_error || error.vm_error)
    {
        LOG("Error while init VM");
        print_vm_error(error);
        return error.vm_error;
    }

    error = load_program(&vm, program_file_name);
    if (error.stack_error || error.executor_error || error.vm_error)
    {
        LOG("Error while loading program");
        print_vm_error(error);
        return error.vm_error;
    }

    error = execute_program(&vm);
    if (error.stack_error || error.executor_error || error.vm_error)
    {
        LOG("Error while executing program");
        print_vm_error(error);
        return error.vm_error;
    }

    error = babah_vm(&vm);
    if (error.stack_error || error.executor_error || error.vm_error)
    {
        LOG("Error while executing program");
        print_vm_error(error);
        return error.vm_error;
    }

    return 0;
}