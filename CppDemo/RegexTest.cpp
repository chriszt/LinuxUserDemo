#include "RegexTest.h"
#include <string>
#include <iostream>
#include <regex>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>

////////////////////////////////////////////////////////////

void RegexTest00()
{
    std::string text = "Date:2023-08-10";
    std::string pattern = "Date.(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);
    std::match_results<std::string::iterator> results;
    // bool isMatch = std::regex_match(text.begin(), text.end(), results, express);
    bool isMatch = std::regex_match(text.begin(), text.end(), results, express);
    std::cout << "isMatch=" << isMatch << std::endl;
    for (auto iter = results.begin(); iter != results.end(); iter++) {
        std::cout << iter->length() << ": " << iter->str()  << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest01()
{
    std::string text = "Date:2023-08-10";
    std::string pattern = "Date.(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(text.cbegin(), text.cend(), results, express);
    std::cout << "isMatch=" << isMatch << std::endl;
    for (auto iter = results.begin(); iter != results.end(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest02()
{
    std::string text = "Date:2023-08-10";
    std::string pattern = "Date.(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);
    bool isMatch = std::regex_match(text, express);
    std::cout << "isMatch=" << isMatch << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest03()
{
    std::string text = "Date:2023-08-10";
    std::string pattern = "Date.(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);
    std::cmatch results;
    bool isMatch = std::regex_match(text.c_str(), results, express);
    std::cout << "isMatch=" << isMatch << std::endl;
    for (auto iter = results.begin(); iter != results.end(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest04()
{
    std::string text = "Date:2023-08-10";
    std::string pattern = "Date.(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(text, results, express);
    std::cout << "isMatch=" << isMatch << std::endl;
    for (size_t i = 0; i < results.size(); i++) {
        std::cout << results[i].length() << ": " << results[i].str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest10()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);
    std::match_results<std::string::iterator> results;
    bool isExist = std::regex_search(text.begin(), text.end(), results, express);
    std::cout << "isExist=" << isExist << std::endl;
    for (auto iter = results.begin(); iter != results.end(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest11()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);
    bool isExist = std::regex_search(text, express);
    std::cout << "isExist=" << isExist << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest12()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);
    std::cmatch results;
    bool isExist = std::regex_search(text.c_str(), results, express);
    std::cout << "isExist=" << isExist << std::endl;
    for (size_t i = 0; i < results.size(); i++) {
        std::csub_match sub = results[i];
        std::cout << sub.length() << ": " << sub.str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest20()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);

    std::string result(256, '0');
    std::string substitutes = "2023-08-21";

    *std::regex_replace(result.begin(), text.begin(), text.end(), express, substitutes) = '\0';
    std::cout << result.c_str() << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest21()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);

    std::string result(256, '0');
    std::string substitutes = "2023-08-21";

    result = std::regex_replace(text, express, substitutes);
    std::cout << result << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest30()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);

    // std::regex_iterator<std::string::const_iterator> begin(text.begin(), text.end(), express);
    std::sregex_iterator begin(text.begin(), text.end(), express);
    for (auto iter = begin; iter != std::sregex_iterator(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest40()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);

    // std::regex_token_iterator<std::string::const_iterator> begin(text.cbegin(), text.cend(), express, 0);
    // std::sregex_token_iterator begin(text.cbegin(), text.cend(), express, 0);
    std::sregex_token_iterator begin(text.cbegin(), text.cend(), express);
    for (auto iter = begin; iter != std::sregex_token_iterator(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest41()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);

    std::sregex_token_iterator begin(text.cbegin(), text.end(), express, -1);
    for (auto iter = begin; iter != std::sregex_token_iterator(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest42()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);

    std::sregex_token_iterator begin(text.cbegin(), text.end(), express, 3);
    for (auto iter = begin; iter != std::sregex_token_iterator(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest43()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);

    std::vector<int> vec;
    vec.push_back(-1);
    vec.push_back(2);
    std::sregex_token_iterator begin(text.cbegin(), text.end(), express, vec);
    // std::sregex_token_iterator begin(text.cbegin(), text.end(), express, {-1, 2});
    for (auto iter = begin; iter != std::sregex_token_iterator(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

void RegexTest44()
{
    std::string text = "Date:2023-08-10 ~ 2023-10-15";
    std::string pattern = "(\\d{4})-(\\d{2}-(\\d{2}))";
    std::regex express(pattern);

    int arr[] = {-1, 0};
    std::sregex_token_iterator begin(text.cbegin(), text.end(), express, arr);
    // std::sregex_token_iterator begin(text.cbegin(), text.end(), express, {-1, 0});
    for (auto iter = begin; iter != std::sregex_token_iterator(); iter++) {
        std::cout << iter->length() << ": " << iter->str() << std::endl;
    }
}

////////////////////////////////////////////////////////////

static int GetUid(const std::string &userName)
{
    std::ifstream in("/etc/passwd");
    if (!in.is_open()) {
        std::cerr << "open /etc/passwd failed" << std::endl;
        return -1;
    }
    int uid = -1;
    std::string line;
    while (std::getline(in, line)) {
        size_t pos[3];
        pos[0] = line.find(':');
        std::string uName = line.substr(0, pos[0]);
        if (uName == userName) {
            pos[1] = line.find(':', pos[0] + 1);
            pos[2] = line.find(':', pos[1] + 1);
            std::string uidStr = line.substr(pos[1] + 1, pos[2] - pos[1] - 1);
            try {
                uid = std::stoi(uidStr);
            } catch (const std::exception &e) {
                uid = -1;
            }
            break;
        }
    }
    in.close();
    return uid;
}

// ssh登陆成功
void RegexTest50()
{
    std::string msg = "MESSAGE=Accepted password for yl from 192.168.56.1 port 46590 ssh2";
    std::string pattern = "MESSAGE=Accepted password for (\\w+) from ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+) ssh2";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    std::string clientIp = results[2].str();
    std::string port = results[4].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "clientIp: " << clientIp << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

// ssh退出
void RegexTest51()
{
    std::string msg = "MESSAGE=Disconnected from user yl 192.168.56.1 port 46590";
    std::string pattern = "MESSAGE=Disconnected from user (\\w+) ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    std::string clientIp = results[2].str();
    std::string port = results[4].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "clientIp: " << clientIp << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

// ssh登陆失败
void RegexTest52()
{
    std::string msg = "MESSAGE=Failed password for yl from 192.168.56.1 port 33542 ssh2";
    std::string pattern = "MESSAGE=Failed password for (\\w+) from ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+) ssh2";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    std::string clientIp = results[2].str();
    std::string port = results[4].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "clientIp: " << clientIp << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

// ssh登陆，用户名不存在
void RegexTest53()
{
    std::string msg = "MESSAGE=Invalid user aaa from 192.168.56.1 port 60474";
    std::string pattern = "MESSAGE=Invalid user (\\w+) from ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    std::string clientIp = results[2].str();
    std::string port = results[4].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "clientIp: " << clientIp << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

// ssh登陆，用户名不存在且密码错误
void RegexTest54()
{
    std::string msg = "MESSAGE=Failed password for invalid user aaa from 192.168.56.1 port 60474 ssh2";
    std::string pattern = "MESSAGE=Failed password for invalid user (\\w+) from ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+) ssh2";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    std::string clientIp = results[2].str();
    std::string port = results[4].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "clientIp: " << clientIp << std::endl;
    std::cout << "port: " << port << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest60()
{
    std::string msg = "MESSAGE=pam_unix(gdm-password:session): session opened for user yl by (uid=0)";
    std::string pattern = "MESSAGE=pam_unix\\(gdm-password:session\\): session opened for user (\\w+) by \\(uid=0\\)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest61()
{
    std::string msg = "MESSAGE=gkr-pam: unlocked login keyring";
    std::string pattern = "MESSAGE=gkr-pam: unlocked login keyring";
    std::regex express(pattern);
    bool isMatch = std::regex_match(msg, express);
    std::cout << "isMatch: " << isMatch << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest62()
{
    std::string msg = "MESSAGE=pam_unix(gdm-password:session): session closed for user yl";
    std::string pattern = "MESSAGE=pam_unix\\(gdm-password:session\\): session closed for user (\\w+)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest63()
{
    std::string msg = "MESSAGE=pam_unix(gdm-password:auth): authentication failure; logname= uid=0 euid=0 tty=/dev/tty1 ruser= rhost=  user=yl";
    std::string pattern = "MESSAGE=pam_unix\\(gdm-password:auth\\): authentication failure; logname= uid=0 euid=0 tty=/dev/tty1 ruser= rhost=  user=(\\w+)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest64()
{
    std::string msg = "MESSAGE=accountsservice: ActUserManager: user (null) has no username (uid: -1)";
    std::string pattern = "MESSAGE=accountsservice: ActUserManager: user \\(null\\) has no username \\(uid: -1\\)";
    std::regex express(pattern);
    bool isMatch = std::regex_match(msg, express);
    std::cout << "isMatch: " << isMatch << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest70()
{
    std::string msg = "MESSAGE=pam_unix(login:session): session opened for user yl by LOGIN(uid=0)";
    std::string pattern = "MESSAGE=pam_unix\\(login:session\\): session opened for user (\\w+) by LOGIN\\(uid=0\\)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest71()
{
    std::string msg = "MESSAGE=pam_unix(login:session): session closed for user yl";
    std::string pattern = "MESSAGE=pam_unix\\(login:session\\): session closed for user (\\w+)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest72()
{
    // std::string msg = "MESSAGE=FAILED LOGIN (1) on '/dev/tty3' FOR 'UNKNOWN', Authentication failure";
    std::string msg = "MESSAGE=FAILED LOGIN (1) on '/dev/tty3' FOR 'yl', Authentication failure";
    std::string pattern = "MESSAGE=FAILED LOGIN \\(1\\) on '/dev/tty3' FOR '(\\w+)', Authentication failure";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    int uid = GetUid(userName);
    std::cout << "userName: " << userName << std::endl;
    std::cout << "uid: " << uid << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest80()
{
    std::string msg = "MESSAGE=Password auth succeeded for 'user1' from 192.168.56.1:55555";
    std::string pattern = "MESSAGE=Password auth succeeded for '(\\w+)' from ((\\d{1,3}\\.){3}\\d{1,3}):(\\d+)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    std::string ip = results[2].str();
    unsigned short port = std::stoul(results[4].str());
    std::cout << userName << ", " << ip << ":" << port << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest81()
{
    std::string msg = "MESSAGE=Exit (user1): Disconnect received";
    std::string pattern = "MESSAGE=Exit \\((\\w+)\\): Disconnect received";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    std::cout << userName << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest82()
{
    std::string msg = "MESSAGE=Bad password attempt for 'user1' from 192.168.56.1:52688";
    std::string pattern = "MESSAGE=Bad password attempt for '(\\w+)' from ((\\d{1,3}\\.){3}\\d{1,3}):(\\d+)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string userName = results[1].str();
    std::string ip = results[2].str();
    unsigned short port = std::stoul(results[4].str());
    std::cout << userName << ", " << ip << ":" << port << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest83()
{
    std::string msg = "MESSAGE=Login attempt for nonexistent user from 192.168.56.1:57164";
    std::string pattern = "MESSAGE=Login attempt for nonexistent user from ((\\d{1,3}\\.){3}\\d{1,3}):(\\d+)";
    std::regex express(pattern);
    std::smatch results;
    bool isMatch = std::regex_match(msg, results, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    std::string ip = results[1].str();
    unsigned short port = std::stoul(results[3].str());
    std::cout << ip << ":" << port << std::endl;
}

////////////////////////////////////////////////////////////

void RegexTest84()
{
    std::string msg1 = "MESSAGE=Exit before auth (user 'user1', 3 fails): Exited normally";
    std::string msg2 = "MESSAGE=Exit before auth: Exited normally";
    std::string pattern = "MESSAGE=Exit before auth";
    std::regex express(pattern);
    bool isMatch = std::regex_search(msg1, express);
    std::cout << "isMatch: " << isMatch << std::endl;
    isMatch = std::regex_search(msg2, express);
    std::cout << "isMatch: " << isMatch << std::endl;
}

////////////////////////////////////////////////////////////

struct LogMsg {
    long time;
    std::string protoName;
    pid_t protoPid;
    std::string msg;
};

static std::shared_ptr<LogMsg> ParseLogMsg(std::string msg)
{
    auto logMsg = std::make_shared<LogMsg>();
    if (!logMsg) {
        return nullptr;
    }
    auto leftSquareIndex = msg.find('[');
    if (leftSquareIndex == std::string::npos) {
        return nullptr;
    }
    auto beforeProtoIndex = msg.rfind(' ', leftSquareIndex);
    logMsg->protoName = msg.substr(beforeProtoIndex + 1, leftSquareIndex - beforeProtoIndex - 1);
    auto rightSquareIndex = msg.find(']', leftSquareIndex);
    auto pidStr = msg.substr(leftSquareIndex + 1, rightSquareIndex - leftSquareIndex - 1);
    try {
        logMsg->protoPid = std::stoi(pidStr);
    } catch (const std::exception &e) {
        std::fprintf(stderr, "convert pidStr to pid failed: %s\n", e.what());
    }
    auto msgBeginIndex = rightSquareIndex + 3;
    auto msgEnterIndex = msg.rfind('\n');
    logMsg->msg = msg.substr(msgBeginIndex, msgEnterIndex -  msgBeginIndex);
    return logMsg;
}

void RegexTest90()
{
    FILE *fp = popen("journalctl -f SYSLOG_IDENTIFIER=sshd", "r");
    if (!fp) {
        std::fprintf(stdout, "popen failed(%d): %s\n", errno, strerror(errno));
        return;
    }
    while (true) {
        char buf[1024];
        if (!std::fgets(buf, sizeof(buf), fp)) {
            continue;
        }
        std::string msg(buf);
        // std::fprintf(stdout, "%s", msg.c_str());
        auto logMsg = ParseLogMsg(msg);
        if (!logMsg) {
            continue;
        }
        std::fprintf(stdout, "%s | %d | %s\n", logMsg->protoName.c_str(), logMsg->protoPid, logMsg->msg.c_str());
    }
    pclose(fp);
}

////////////////////////////////////////////////////////////

