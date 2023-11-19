#include "SyslogTest.h"
#include <stdio.h>
#include <syslog.h>

////////////////////////////////////////////////////////////

void SyslogTest00()
{
    openlog(NULL, LOG_CONS, LOG_USER);
    syslog(LOG_DEBUG, "Thiis is a test\n");
    closelog();
}

////////////////////////////////////////////////////////////