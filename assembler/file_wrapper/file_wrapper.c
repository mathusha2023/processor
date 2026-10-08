#include "file_wrapper.h"
#include <assert.h>
#include "config.h"
#include "files.h"
#include "my_string.h"
#include "log.h"

static void dispose(struct FileWrapper *wrapper)
{
    assert(wrapper);

    if (wrapper->buffer)
        free_ptr(wrapper->buffer);
    if (wrapper->strings)
        free_ptr(wrapper->strings);
}

struct FileWrapper init_file_wrapper(const char *filename)
{
    assert(filename);

    struct FileWrapper wrapper = {.filename = filename, .dispose = &dispose};

    if (get_file_text(&wrapper))
    {
        log("Error while saving text in wrapper struct: %d", wrapper.error);
        return wrapper;
    }

    assert(wrapper.buffer);
    assert(wrapper.file_size);
    assert(wrapper.file_n_symb);

    if (make_strings_arr(&wrapper))
    {
        log("Error while making strings array: %d", wrapper.error);
        free_ptr(wrapper.buffer);
        return wrapper;
    }

    assert(wrapper.strings);
    assert(wrapper.strings_count);

    return wrapper;
}
