#include "cmd_args.h"
#include <string.h>
#include "log.h"

// МОЖНО ИСПОЛЬЗОВАТЬ getopt, но пока что в учебных целях все руками
struct CmdArgs get_args(int argc, char *argv[])
{
    assert(argv);

    struct CmdArgs cmd_args = {};

    int wait_input_file_name = 0;
    int wait_output_file_name = 0;

    for (int i = 1; i < argc; i++)
    {
        flog("Cmd arg %d: '%s'", i, argv[i]);

        if (wait_input_file_name)
        {
            wait_input_file_name = 0;
            cmd_args.INPUT_FILE_NAME = argv[i];
            flog("Set input file name: '%s'", cmd_args.INPUT_FILE_NAME);
        }

        if (wait_output_file_name)
        {
            wait_output_file_name = 0;
            cmd_args.OUTPUT_FILE_NAME = argv[i];
            flog("Set output file name: '%s'", cmd_args.OUTPUT_FILE_NAME);
        }

        if (!strcmp(argv[i], INPUT_FILE_NAME_FLAG))
        {
            flog("Found %s cmd flag", INPUT_FILE_NAME_FLAG);

            if (cmd_args.INPUT_FILE_NAME)
            {
                flog("Found dublicate flag: %s", INPUT_FILE_NAME_FLAG);
                cmd_args.error = DUBLICATE_FLAG;
                break;
            }

            wait_input_file_name = 1;
        }
        else if (!strcmp(argv[i], OUTPUT_FILE_NAME_FLAG))
        {
            flog("Found %s cmd flag", OUTPUT_FILE_NAME_FLAG);

            if (cmd_args.OUTPUT_FILE_NAME)
            {
                flog("Found dublicate flag: %s", OUTPUT_FILE_NAME_FLAG);
                cmd_args.error = DUBLICATE_FLAG;
                break;
            }

            wait_output_file_name = 1;
        }
    }

    if (wait_output_file_name || wait_input_file_name)
    {
        flog("Error: arg value field not found((");
        cmd_args.error = VALUE_FIELD_NOT_FOUND;
    }

    return cmd_args;
}