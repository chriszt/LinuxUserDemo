#include "OtherTest.h"
#include <stdio.h>
#include <unistd.h>

/////////////////////////////////////////////////////////////

void OtherTest00()
{
    fprintf(stdout, "fd 0: %s\n", ttyname(STDIN_FILENO));
    fprintf(stdout, "fd 1: %s\n", ttyname(STDOUT_FILENO));
    fprintf(stdout, "fd 2: %s\n", ttyname(STDERR_FILENO));
}

/////////////////////////////////////////////////////////////