#ifndef CMD_ARGS_H
#define CMD_ARGS_H

enum CmdError
{
    OK = 0,
    DUBLICATE_FLAG,
    VALUE_FIELD_NOT_FOUND
};

struct CmdArgs
{
    enum CmdError error;
    const char *INPUT_FILE_NAME;
    const char *OUTPUT_FILE_NAME;
};

struct CmdArgs get_args(int argc, char *argv[]);

#endif // CMD_ARGS_H