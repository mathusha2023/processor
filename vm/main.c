#include "log.h"
#include "executor.h"

int main(void)
{
    restart_log();

    einit();
    epush(10);
    epush(20);
    eadd();
    epush(1);
    epush(19);
    eadd();
    ediv();
    eout();
    ehlt();

    einit();
    epush(2);
    esqrt();
    eout();
    ehlt();

    return 0;
}