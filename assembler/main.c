#include "log.h"
#include "cmd_args.h"
#include "file_wrapper.h"
#include "assembler.h"

int main(int argc, char *argv[])
{
    restart_log();

    log("Beginning compilation...");

    struct CmdArgs cmd_args = get_args(argc, argv);

    if (cmd_args.error)
    {
        log("Error in cmd line args: code %d", cmd_args.error);
        return 1;
    }

    const char *input_file_name = cmd_args.INPUT_FILE_NAME ? cmd_args.INPUT_FILE_NAME : DEFAULT_INPUT_FILE_NAME;
    const char *output_file_name = cmd_args.OUTPUT_FILE_NAME ? cmd_args.OUTPUT_FILE_NAME : DEFAULT_OUTPUT_FILE_NAME;

    flog("Input file name: '%s'", input_file_name);
    flog("Output file name: '%s'", output_file_name);

    struct FileWrapper wrapper = init_file_wrapper(input_file_name);

    if (wrapper.error)
    {
        log("Error while init file wrapper: %d", wrapper.error);
        return wrapper.error;
    }

    if (asm_data(&wrapper, output_file_name))
    {
        log("Can not assemble data from file((");
        wrapper.dispose(&wrapper);
        return 1;
    }

    wrapper.dispose(&wrapper);

    log("Compilation successful! Output file saved ad '%s'", output_file_name);

    return 0;
}
