#include "SdJournalTest.h"
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <syslog.h>
#include <systemd/sd-journal.h>
#include <systemd/sd-daemon.h>
#include "Utils.h"
#include "Parser.h"
#include "LoginDef.h"

static void DumpDetail(sd_journal *j)
{
    const void *data;
    size_t len;
    SD_JOURNAL_FOREACH_DATA(j, data, len) {
        std::fprintf(stdout, "%s(%zu)\n", (const char *)data, len);
    };
    std::fprintf(stdout, "\n-----------------------\n");
}

////////////////////////////////////////////////////////////

void Test00()
{
    int fd = sd_journal_stream_fd("test", LOG_INFO, 1);
    FILE *log = fdopen(fd, "w");
    fprintf(log, "Hello World!\n");
    fprintf(log, SD_EMERG "This is a emerg\n");
    fprintf(log, SD_ALERT "This is a alert\n");
    fprintf(log, SD_CRIT "This is a crit\n");
    fprintf(log, SD_ERR "This is a err\n");
    fprintf(log, SD_WARNING "This is a warning\n");
    fprintf(log, SD_NOTICE "This is a notice\n");
    fprintf(log, SD_INFO "This is a info\n");
    fprintf(log, SD_DEBUG "This is a debug\n");
}

////////////////////////////////////////////////////////////

void Test01()
{
    sd_journal_print(LOG_EMERG, "Hello World");
    sd_journal_print(LOG_ALERT, "Hello World");
    sd_journal_print(LOG_CRIT, "Hello World");
    sd_journal_print(LOG_ERR, "Hello World");
    sd_journal_print(LOG_WARNING, "Hello World");
    sd_journal_print(LOG_NOTICE, "Hello World");
    sd_journal_print(LOG_INFO, "Hello World");
    sd_journal_print(LOG_DEBUG, "Hello World");
}

////////////////////////////////////////////////////////////

void Test02()
{
    int r;
    sd_journal *j;
    r = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    SD_JOURNAL_FOREACH(j) {
        const char *d;
        size_t l;
        r = sd_journal_get_data(j, "MESSAGE", (const void **)&d, &l);
        if (r < 0) {
            continue;
        }
        std::printf("%.*s\n", (int) l, d);
    }
    sd_journal_close(j);
}

////////////////////////////////////////////////////////////

void Test03()
{
    int r;
    sd_journal *j;
    r = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (r < 0) {
        fprintf(stderr, "Failed to open journal: %s\n", strerror(-r));
        return;
    }
    for (;;)  {
        const void *d;
        size_t l;
        r = sd_journal_next(j);
        if (r < 0) {
            fprintf(stderr, "Failed to iterate to next entry: %s\n", strerror(-r));
            break;
        }
        if (r == 0) {
            /* Reached the end, let's wait for changes, and try again */
            r = sd_journal_wait(j, (uint64_t) -1);
            if (r < 0) {
                fprintf(stderr, "Failed to wait for changes: %s\n", strerror(-r));
                break;
            }
            continue;
        }
        r = sd_journal_get_data(j, "MESSAGE", &d, &l);
        if (r < 0) {
            fprintf(stderr, "Failed to read message field: %s\n", strerror(-r));
            continue;
        }
        printf("%.*s\n", (int) l, (const char*) d);
    }
    sd_journal_close(j);
}

////////////////////////////////////////////////////////////

void Test04()
{
    sd_journal *j = nullptr;
    int ret = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0) {
        std::fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "open journal succeeded: j=0x%p\n", j);

    // int fd = sd_journal_get_fd(j);
    // std::fprintf(stdout, "get fd=%d\n", fd);

    // size_t sz = 0;
    // ret = sd_journal_set_data_threshold(j, sz);
    // if (ret < 0) {
    //     std::fprintf(stderr, "set threshold failed(%d): %s\n", -ret, strerror(-ret));
    //     exit(EXIT_FAILURE);
    // }
    // std::fprintf(stderr, "set threshold to %zu\n", sz);

    // size_t len;
    // ret = sd_journal_get_data_threshold(j, &len);
    // if (ret < 0) {
    //     std::fprintf(stderr, "get threshold failed(%d): %s\n", -ret, strerror(-ret));
    //     exit(EXIT_FAILURE);
    // }
    // std::fprintf(stdout, "get threshold succeeded: len=%zu\n", len);

    // ret = sd_journal_seek_tail(j);
    // if (ret < 0) {
    //     std::fprintf(stderr, "seek tail failed(%d): %s\n", -ret, strerror(-ret));
    //     exit(EXIT_FAILURE);
    // }
    // std::fprintf(stdout, "seek tail succeeded\n");

    // ret = sd_journal_previous(j);
    // if (ret < 0) {
    //     std::fprintf(stderr, "move to previous failed(%d): %s\n", -ret, strerror(-ret));
    //     exit(EXIT_FAILURE);
    // }
    // std::fprintf(stdout, "move to previous succeeded: ret=%d\n", ret);

    ret += sd_journal_add_match(j, "_SYSTEMD_UNIT=avahi-daemon.service", 0);
    ret += sd_journal_add_match(j, "PRIORITY=4", 0);
    ret += sd_journal_add_match(j, "PRIORITY=5", 0);
    ret += sd_journal_add_match(j, "PRIORITY=6", 0);
    // ret += sd_journal_add_disjunction(j);
    // ret += sd_journal_add_conjunction(j);
    if (ret < 0) {
        std::fprintf(stderr, "add match failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }

    while (true) {
        ret = sd_journal_next(j);
        if (ret < 0) {
            std::fprintf(stderr, "move to next entry failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        if (ret == 0) {
            std::fprintf(stdout, "no more entry existd, waiting for the new one...\n");
            int r = sd_journal_wait(j, -1);
            if (r < 0) {
                std::fprintf(stderr, "wait for new entry failed(%d): %s\n", -r, strerror(-r));
                break;
            } else if (r == SD_JOURNAL_NOP) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_NOP\n");
            }  else if (r == SD_JOURNAL_APPEND) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_APPEND\n");
            }  else if (r == SD_JOURNAL_INVALIDATE) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_INVALIDATE\n");
            }
            continue;
        }
        // std::fprintf(stdout, "move to next entry succeeded: ret=%d\n", ret);
        

        // char *cursor = nullptr;
        // ret = sd_journal_get_cursor(j, &cursor);
        // if (ret < 0) {
        //     std::fprintf(stderr, "get cursor failed(%d): %s\n", -ret, strerror(-ret));
        //     break;
        // }
        // std::fprintf(stdout, "get cursor succeeded: cursor=%s\n", cursor);

        const char *data = nullptr;
        size_t len = 0;
        std::string msg = "[";
        // _SOURCE_REALTIME_TIMESTAMP
        ret = sd_journal_get_data(j, "_SOURCE_REALTIME_TIMESTAMP", (const void **)&data, &len);
        if (ret < 0) {
            // std::fprintf(stderr, "get data failed(%d): %s\n", -ret, strerror(-ret));
            // SD_JOURNAL_FOREACH_DATA(j, data, len) {
            //     std::fprintf(stdout, "%s\n", (const char *)data);
            // }
            data = "";
        }
        // msg += Utils::SourceRealtimeTimestamp((const char *)data);
        // msg += data;
        msg += "] ";

        // MESSAGE
        ret = sd_journal_get_data(j, "MESSAGE", (const void **)&data, &len);
        if (ret < 0) {
            // std::fprintf(stderr, "get data failed(%d): %s\n", -ret, strerror(-ret));
            data = (const char *)"";
        }
        // msg += Utils::Message((const char *)data);
        msg += data;
        std::cout << msg << std::endl;

        // sd_journal_restart_data(j);
        // while (true) {
        //     char *data = nullptr;
        //     size_t len = 0;
        //     // ret = sd_journal_enumerate_data(j, (const void **)&data, &len);
        //     ret = sd_journal_enumerate_available_data(j, (const void **)&data, &len);
        //     if (ret < 0) {
        //         std::fprintf(stderr, "enum data failed(%d): %s\n", -ret, strerror(-ret));
        //         break;
        //     } else if (ret == 0) {
        //         break;
        //     }
        //     std::fprintf(stdout, "%s\n", data);
        // }

        // const char *fields = nullptr;
        // SD_JOURNAL_FOREACH_FIELD(j, fields) {
        //     std::fprintf(stdout, "%s\n", fields);
        // }

        // const void *data;
        // size_t len;
        // SD_JOURNAL_FOREACH_UNIQUE(j, data, len) {
        //     std::fprintf(stdout, "%s\n", (const char *)data);
        // }

        // std::fprintf(stdout, "\n---------------------------\n");
        // usleep(100 * 1000);
    }
    

    // while (true) {
    //     ret = sd_journal_next(j);
    //     if (ret < 0) {
    //         std::fprintf(stderr, "iterate the next entry failed(%d): %s\n", -ret, strerror(-ret));
    //         break;
    //     }
    //     if (ret == 0) {
    //         std::fprintf(stdout, "iterate finished, waiting a new message\n");
    //         int r = sd_journal_wait(j, -1);
    //         std::fprintf(stdout, "a new message arrived\n");
    //         if (r < 0) {
    //             std::fprintf(stderr, "wait for journal failed(%d): %s\n", -r, strerror(-r));
    //             break;
    //         } else if (r == SD_JOURNAL_NOP) {
    //             std::fprintf(stdout, "wait for journal: SD_JOURNAL_NOP\n");
    //         } else if (r == SD_JOURNAL_APPEND) {
    //             std::fprintf(stdout, "wait for journal: SD_JOURNAL_APPEND\n");
    //         } else if (r == SD_JOURNAL_INVALIDATE) {
    //             std::fprintf(stdout, "wait for journal: SD_JOURNAL_INVALIDATE\n");
    //         }
    //         continue;
    //     }
    //     char *cursor = nullptr;
    //     ret = sd_journal_get_cursor(j, &cursor);
    //     std::fprintf(stdout, "ret=%d, %s, cursor=%s\n", -ret, strerror(-ret), cursor);
    //     char *data;
    //     size_t len;
    //     ret = sd_journal_get_data(j, "MESSAGE", (const void **)&data, &len);
    //     if (ret < 0) {
    //         std::fprintf(stderr, "get data failed(%d): %s\n", -ret, strerror(-ret));
    //         break;
    //     }
    //     std::fprintf(stdout, "[%zu] %s\n", len, data);
    // }

    sd_journal_close(j);
    std::fprintf(stdout, "close journal succeeded\n");
}

////////////////////////////////////////////////////////////

static int WaitForChange05(sd_journal *j)
{
    uint64_t t;
    int msec;
    sd_journal_get_timeout(j, &t);
    if (t == (uint64_t)-1) {
        msec = -1;
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint64_t n = (uint64_t) ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
        msec = t > n ? (int)((t - n + 999) / 1000) : 0;
    }
    // int rfd = sd_journal_reliable_fd(j); // ==1
    struct pollfd fds;
    fds.fd = sd_journal_get_fd(j); // ==3
    fds.events = sd_journal_get_events(j);
    // std::fprintf(stdout, "rfd=%d, fd=%d\n", rfd, fds.fd);
    poll(&fds, 1, msec);
    return sd_journal_process(j);
}

void Test05()
{
    sd_journal *j = nullptr;
    int ret = sd_journal_open(&j, SD_JOURNAL_RUNTIME_ONLY);
    if (ret < 0) {
        std::fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        return;
    }
    std::fprintf(stdout, "open journal succeeded: j=0x%p\n", j);

    int fd = sd_journal_get_fd(j);
    std::fprintf(stdout, "fd=%d\n", fd);

    while (true) {
        ret = sd_journal_next(j);
        if (ret < 0) {
            std::fprintf(stderr, "iterate the next entry failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        if (ret == 0) {
            std::fprintf(stdout, "iterate finished, waiting a new message\n");
            int r = WaitForChange05(j);
            std::fprintf(stdout, "a new message arrived\n");
            if (r < 0) {
                std::fprintf(stderr, "wait for journal failed(%d): %s\n", -r, strerror(-r));
                break;
            } else if (r == SD_JOURNAL_NOP) {
                std::fprintf(stdout, "wait for journal: SD_JOURNAL_NOP\n");
            } else if (r == SD_JOURNAL_APPEND) {
                std::fprintf(stdout, "wait for journal: SD_JOURNAL_APPEND\n");
            } else if (r == SD_JOURNAL_INVALIDATE) {
                std::fprintf(stdout, "wait for journal: SD_JOURNAL_INVALIDATE\n");
            }
            continue;
        }
        char *data;
        size_t len;
        ret = sd_journal_get_data(j, "MESSAGE", (const void **)&data, &len);
        if (ret < 0) {
            std::fprintf(stderr, "get data failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        std::fprintf(stdout, "[%zu] %s\n", len, data);
    }

    sd_journal_close(j);
    std::fprintf(stdout, "close journal succeeded\n");
}

////////////////////////////////////////////////////////////

void Test06()
{
    sd_journal *j;
    int ret = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0) {
        fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "open journal succeeded\n");

    uint64_t usage;
    ret = sd_journal_get_usage(j, &usage);
    if (ret < 0) {
        fprintf(stderr, "get usage failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "get usage succeeded: usage=%lu(B), usage=%lu(MB)\n", usage, usage / 1024 /1024);

    sd_journal_close(j);
}

////////////////////////////////////////////////////////////

void TestUtils()
{
    // std::string userName = "yl";
    // std::cout << userName << " --> " << Utils::GetUid(userName) << std::endl;
    // userName = "root";
    // std::cout << userName << " --> " << Utils::GetUid(userName) << std::endl;

    // std::cout << Utils::GetHostName() << std::endl;

    // pid_t pid = getpid();
    // std::string procName = Utils::GetNameByPid(pid);
    // std::cout << pid << " --> "<< procName << std::endl;

    // pid_t ppid = Utils::GetPpidByPid(pid);
    // std::cout << pid << " parent is "<< ppid << std::endl;

    pid_t pid = 795;
    std::string port = "68";
    std::cout <<  Utils::CheckProcessExist(pid) << std::endl;
    std::cout <<  Utils::CheckRemotePortExist(pid, port) << std::endl;
}

////////////////////////////////////////////////////////////

void TestSsh()
{
    sd_journal *j = nullptr;
    int ret = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0) {
        std::fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "open journal succeeded: j=0x%p\n", j);

    ret = sd_journal_add_match(j, "SYSLOG_IDENTIFIER=sshd", 0);
    if (ret < 0) {
        std::fprintf(stderr, "add filter ssh.service failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "add filter ssh.service succeeded\n");

    ret = sd_journal_seek_tail(j);
    if (ret < 0) {
        std::fprintf(stderr, "seek tail failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "seek tail succeeded\n");

    ret = sd_journal_previous(j);
    if (ret < 0) {
        std::fprintf(stderr, "move the read pointer to previous failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "move the read pointer to previous succeeded: ret=%d\n", ret);

    while (true) {
        ret = sd_journal_next(j);
        if (ret < 0) {
            std::fprintf(stderr, "move to next entry failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        if (ret == 0) {
            std::fprintf(stdout, "no more entry existd, waiting for the new one...\n");
            int r = sd_journal_wait(j, -1);
            if (r < 0) {
                std::fprintf(stderr, "wait for new entry failed(%d): %s\n", -r, strerror(-r));
                break;
            } else if (r == SD_JOURNAL_NOP) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_NOP\n");
            } else if (r == SD_JOURNAL_APPEND) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_APPEND\n");
            } else if (r == SD_JOURNAL_INVALIDATE) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_INVALIDATE\n");
            }
            continue;
        }
        // const char *data = nullptr;
        // size_t len = 0;
        // ret = sd_journal_get_data(j, "_SOURCE_REALTIME_TIMESTAMP", (const void **)&data, &len);
        // if (ret < 0) {
        //     data = "";
        // }
        // unsigned long time = Utils::GetTimeStamp(std::string(data, len));
        // ret = sd_journal_get_data(j, "MESSAGE", (const void **)&data, &len);
        // if (ret < 0) {
        //     data = "";
        // }
        // auto loginInfo = Utils::GetLoginInfo(time, std::string(data, len));
        // if (loginInfo != nullptr) {
        //     loginInfo->Show();
        // }

        DumpDetail(j);
    }

    sd_journal_close(j);
    std::fprintf(stdout, "close journal succeeded\n");
}

////////////////////////////////////////////////////////////

void TestGdm()
{
    sd_journal *j;
    int ret = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0) {
        std::fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "open journal succeeded\n");

    ret = sd_journal_add_match(j, "SYSLOG_IDENTIFIER=gdm-password]", 0);
    if (ret < 0) {
        std::fprintf(stderr, "add match failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "add match succeeded\n");

    ret = sd_journal_seek_tail(j);
    if (ret < 0) {
        std::fprintf(stderr, "seek tail failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "seek tail succeeded\n");

    ret = sd_journal_previous(j);
    if (ret < 0) {
        std::fprintf(stderr, "move the read pointer to previous failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "move the read pointer to previous succeeded\n");

    while (true) {
        ret = sd_journal_next(j);
        if (ret < 0) {
            std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        if (ret == 0) {
            int r = sd_journal_wait(j ,-1);
            if (r < 0) {
                std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -r, strerror(-r));
                break;
            }
            if (r == SD_JOURNAL_NOP) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_NOP\n");
            } else if (r == SD_JOURNAL_APPEND) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_APPEND\n");
            } else if (r == SD_JOURNAL_INVALIDATE) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_INVALIDATE\n");
            }
            continue;
        }
        // const char *data;
        // size_t len;
        // ret = sd_journal_get_data(j, "_SOURCE_REALTIME_TIMESTAMP", (const void **)&data, &len);
        // if (ret < 0) {
        //     data = "";
        // }
        // unsigned long time = Utils::GetTimeStamp(std::string(data, len));
        // ret = sd_journal_get_data(j, "MESSAGE", (const void **)&data, &len);
        // if (ret < 0) {
        //     data = "";
        // }
        // auto loginInfo = Utils::GetLoginInfo(time, std::string(data, len));
        // if (loginInfo != nullptr) {
        //     loginInfo->Show();
        // }

        DumpDetail(j);
    }
    sd_journal_close(j);
    std::fprintf(stdout, "close journal succeeded\n");
}

////////////////////////////////////////////////////////////

void TestLogin()
{
    sd_journal *j;
    int ret = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0) {
        std::fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "open journal succeeded\n");

    ret = sd_journal_add_match(j, "SYSLOG_IDENTIFIER=login", 0);
    if (ret < 0) {
        std::fprintf(stderr, "add match failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "add match succeeded\n");

    ret = sd_journal_seek_tail(j);
    if (ret < 0) {
        std::fprintf(stderr, "seek tail failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "seek tail succeeded\n");

    ret = sd_journal_previous(j);
    if (ret < 0) {
        std::fprintf(stderr, "move the read pointer to previous failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "move the read pointer to previous succeeded\n");

    while (true) {
        ret = sd_journal_next(j);
        if (ret < 0) {
            std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        if (ret == 0) {
            int r = sd_journal_wait(j ,-1);
            if (r < 0) {
                std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -r, strerror(-r));
                break;
            }
            if (r == SD_JOURNAL_NOP) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_NOP\n");
            } else if (r == SD_JOURNAL_APPEND) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_APPEND\n");
            } else if (r == SD_JOURNAL_INVALIDATE) {
                std::fprintf(stdout, "wake up: SD_JOURNAL_INVALIDATE\n");
            }
            continue;
        }
        // const char *data;
        // size_t len;
        // ret = sd_journal_get_data(j, "_SOURCE_REALTIME_TIMESTAMP", (const void **)&data, &len);
        // if (ret < 0) {
        //     data = "";
        // }
        // unsigned long time = Utils::GetTimeStamp(std::string(data, len));
        // ret = sd_journal_get_data(j, "MESSAGE", (const void **)&data, &len);
        // if (ret < 0) {
        //     data = "";
        // }
        // auto loginInfo = Utils::GetLoginInfo(time, std::string(data, len));
        // if (loginInfo != nullptr) {
        //     loginInfo->Show();
        // }

        DumpDetail(j);
    }
    sd_journal_close(j);
    std::fprintf(stdout, "close journal succeeded\n");
}

////////////////////////////////////////////////////////////

void TestTelnet()
{
    sd_journal *j;
    int ret = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0) {
        std::fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "open journal succeeded\n");

    ret += sd_journal_add_match(j, "SYSLOG_IDENTIFIER=in.telnetd", 0);
    ret += sd_journal_add_match(j, "SYSLOG_IDENTIFIER=login", 0);
    if (ret < 0) {
        std::fprintf(stderr, "add match failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "add match succeeded\n");

    ret = sd_journal_seek_tail(j);
    if (ret < 0) {
        std::fprintf(stderr, "seek tail failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "seek tail succeeded\n");

    ret = sd_journal_previous(j);
    if (ret < 0) {
        std::fprintf(stderr, "move the read pointer to previous failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "move the read pointer to previous succeeded\n");

    Parser parser;
    while (true) {
        ret = sd_journal_next(j);
        if (ret < 0) {
            std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        if (ret == 0) {
            int r = sd_journal_wait(j ,-1);
            if (r < 0) {
                std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -r, strerror(-r));
                break;
            }
            // if (r == SD_JOURNAL_NOP) {
            //     std::fprintf(stdout, "wake up: SD_JOURNAL_NOP\n");
            // } else if (r == SD_JOURNAL_APPEND) {
            //     std::fprintf(stdout, "wake up: SD_JOURNAL_APPEND\n");
            // } else if (r == SD_JOURNAL_INVALIDATE) {
            //     std::fprintf(stdout, "wake up: SD_JOURNAL_INVALIDATE\n");
            // }
            continue;
        }
        auto info = parser.GetLoginInfo(j);
        if (info) {
            info->Show();
        }

        DumpDetail(j);
    }
    sd_journal_close(j);
    std::fprintf(stdout, "close journal succeeded\n");
}

////////////////////////////////////////////////////////////

void TestBusyBox()
{

}

////////////////////////////////////////////////////////////

void TestDropBear()
{
    sd_journal *j;
    int ret = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0) {
        std::fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "open journal succeeded\n");

    ret = sd_journal_add_match(j, "SYSLOG_IDENTIFIER=login", 0);
    if (ret < 0) {
        std::fprintf(stderr, "add match failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "add match succeeded\n");

    ret = sd_journal_seek_tail(j);
    if (ret < 0) {
        std::fprintf(stderr, "seek tail failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "seek tail succeeded\n");

    ret = sd_journal_previous(j);
    if (ret < 0) {
        std::fprintf(stderr, "move the read pointer to previous failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "move the read pointer to previous succeeded\n");

    while (true) {
        ret = sd_journal_next(j);
        if (ret < 0) {
            std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        if (ret == 0) {
            int r = sd_journal_wait(j ,-1);
            if (r < 0) {
                std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -r, strerror(-r));
                break;
            }
            // if (r == SD_JOURNAL_NOP) {
            //     std::fprintf(stdout, "wake up: SD_JOURNAL_NOP\n");
            // } else if (r == SD_JOURNAL_APPEND) {
            //     std::fprintf(stdout, "wake up: SD_JOURNAL_APPEND\n");
            // } else if (r == SD_JOURNAL_INVALIDATE) {
            //     std::fprintf(stdout, "wake up: SD_JOURNAL_INVALIDATE\n");
            // }
            continue;
        }

        DumpDetail(j);
    }
    sd_journal_close(j);
    std::fprintf(stdout, "close journal succeeded\n");
}

////////////////////////////////////////////////////////////

void TestTotal()
{
    sd_journal *j = nullptr;
    int ret = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (ret < 0) {
        std::fprintf(stderr, "open journal failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "open journal succeeded\n");

    ret += sd_journal_add_match(j, "SYSLOG_IDENTIFIER=sshd", 0);
    ret += sd_journal_add_match(j, "SYSLOG_IDENTIFIER=gdm-password]", 0);
    ret += sd_journal_add_match(j, "SYSLOG_IDENTIFIER=dropbear", 0);
    ret += sd_journal_add_match(j, "SYSLOG_IDENTIFIER=in.telnetd", 0);
    ret += sd_journal_add_match(j, "SYSLOG_IDENTIFIER=login", 0);
    if (ret < 0) {
        std::fprintf(stderr, "add filter failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "add filter succeeded\n");

    ret = sd_journal_seek_tail(j);
    if (ret < 0) {
        std::fprintf(stderr, "seek tail failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "seek tail succeeded\n");

    ret = sd_journal_previous(j);
    if (ret < 0) {
        std::fprintf(stderr, "move the read pointer to previous failed(%d): %s\n", -ret, strerror(-ret));
        exit(EXIT_FAILURE);
    }
    std::fprintf(stdout, "move the read pointer to previous succeeded\n");

    Parser parser;
    while (true) {
        ret = sd_journal_next(j);
        if (ret < 0) {
            std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -ret, strerror(-ret));
            break;
        }
        if (ret == 0) {
            int r = sd_journal_wait(j ,-1);
            if (r < 0) {
                std::fprintf(stderr, "move the read pointer to next failed(%d): %s\n", -r, strerror(-r));
                break;
            }
            continue;
        }
        auto info = parser.GetLoginInfo(j);
        if (info) {
            info->Show();
        }
        // DumpDetail(j);
    }

    sd_journal_close(j);
    std::fprintf(stdout, "close journal succeeded\n");
}

////////////////////////////////////////////////////////////

void RemoteTest()
{
    std::vector<std::string> msgs{"aaa", "bbb", "ccc"};
    for (auto &msg : msgs) {
        std::cout << msg << " ";
    }
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////
