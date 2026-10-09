#include "vm.h"
#include "config.h"
#include "log.h"

int main(void)
{
    restart_log();

    VM vm = {};
    VMError error = {};

    error = init_vm(&vm);
    if (error.stack_error || error.executor_error || error.vm_error)
    {
        LOG("Error while init VM");
        print_vm_error(error);
        return error.vm_error;
    }

    error = load_program(&vm, DEFAULT_PROGRAM_FILE_NAME);
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