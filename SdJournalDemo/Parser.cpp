#include "Parser.h"
#include <iostream>
#include <fstream>
#include <pwd.h>
#include <utmp.h>
#include "Utils.h"

// ssh登陆，SYSLOG_IDENTIFIER=sshd
static const std::vector<Parser::SceneArgs> SSH_SCENE{
    {Parser::REGEX_MATCH, "MESSAGE=Accepted password for (\\w+) from ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+) ssh2", 1, LOGIN_SUCCESS},
    {Parser::REGEX_MATCH, "MESSAGE=Disconnected from user (\\w+) ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+)", 2, LOGOUT},
    {Parser::REGEX_MATCH, "MESSAGE=Failed password for (\\w+) from ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+) ssh2", 1, LOGIN_FAIL},
    {Parser::REGEX_MATCH, "MESSAGE=Invalid user (\\w+) from ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+)", 1, LOGIN_FAIL},
    {Parser::REGEX_MATCH, "MESSAGE=Failed password for invalid user (\\w+) from ((\\d{1,3}\\.){3}\\d{1,3}) port (\\d+) ssh2", 1, LOGIN_FAIL},
};

// gdm登陆，SYSLOG_IDENTIFIER=gdm-password]
static const std::vector<Parser::SceneArgs> GDM_SCENE{
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(gdm-password:session\\): session opened for user (\\w+) by \\(uid=\\d\\)", 1, LOGIN_SUCCESS},
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(gdm-password:session\\): session closed for user (\\w+)", 2, LOGOUT},
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(gdm-password:auth\\): authentication failure; logname= uid=\\d euid=\\d tty=/dev/tty\\d ruser= rhost=  user=(\\w+)", 1, LOGIN_FAIL},
    {Parser::EQUAL_COMPARE, "MESSAGE=accountsservice: ActUserManager: user (null) has no username (uid: -1)", 1, LOGIN_FAIL},
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(gdm-password:auth\\): authentication failure; logname= uid=\\d euid=\\d tty=/dev/tty\\d ruser= rhost=", 1, LOGIN_FAIL},
};

// dropbear登陆，SYSLOG_IDENTIFIER=dropbear
static const std::vector<Parser::SceneArgs> DROPBEAR_SCENE{
    {Parser::REGEX_MATCH, "MESSAGE=Password auth succeeded for '(\\w+)' from ((\\d{1,3}\\.){3}\\d{1,3}):(\\d+)", 1, LOGIN_SUCCESS},
    {Parser::REGEX_MATCH, "MESSAGE=Exit \\((\\w+)\\): Disconnect received", 2, LOGOUT},
    {Parser::REGEX_MATCH, "MESSAGE=Bad password attempt for '(\\w+)' from ((\\d{1,3}\\.){3}\\d{1,3}):(\\d+)", 1, LOGIN_FAIL},
    {Parser::REGEX_MATCH, "MESSAGE=Login attempt for nonexistent user from ((\\d{1,3}\\.){3}\\d{1,3}):(\\d+)", 1, LOGIN_FAIL},
    {Parser::REGEX_SEARCH, "MESSAGE=Exit before auth", 2, LOGOUT}
};

// tty登陆，SYSLOG_IDENTIFIER=login
static const std::vector<Parser::SceneArgs> LOGIN_SCENE{
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(login:session\\): session opened for user (\\w+) by LOGIN\\(uid=\\d\\)", 1, LOGIN_SUCCESS},
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(login:session\\): session closed for user (\\w+)", 2, LOGOUT},
    {Parser::EQUAL_COMPARE, "MESSAGE=pam_unix(login:auth): check pass; user unknown", 1, LOGIN_FAIL},
    {Parser::REGEX_MATCH, "MESSAGE=FAILED LOGIN \\(\\d\\) on '/dev/tty\\d' FOR '(\\w+)', Authentication failure", 1, LOGIN_FAIL}
};

// telnet连接，SYSLOG_IDENTIFIER=in.telnetd
static const std::string TELNET_CONNECTION_SCENE = "MESSAGE=connect from ((\\d{1,3}\\.){3}\\d{1,3}) \\((\\d{1,3}\\.){3}\\d{1,3}\\)";

// telnet调用login，SYSLOG_IDENTIFIER=login
static const std::vector<Parser::SceneArgs> TELNET_LOGIN_SCENE{
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(login:session\\): session opened for user (\\w+) by \\(uid=\\d\\)", 1, LOGIN_SUCCESS},
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(login:session\\): session closed for user (\\w+)", 2, LOGOUT},
    {Parser::EQUAL_COMPARE, "MESSAGE=pam_unix(login:auth): check pass; user unknown", 1, LOGIN_FAIL},
    {Parser::REGEX_MATCH, "MESSAGE=FAILED LOGIN \\(\\d\\) on '/dev/pts/\\d' from '.*' FOR '(\\w+)', Authentication failure", 1, LOGIN_FAIL}
};

// busybox调用login，SYSLOG_IDENTIFIER=login
static const std::vector<Parser::SceneArgs> BUSYBOX_LOGIN_SCENE{
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(login:session\\): session opened for user (\\w+) by LOGIN\\(uid=\\d\\)", 1, LOGIN_SUCCESS},
    {Parser::REGEX_MATCH, "MESSAGE=pam_unix\\(login:session\\): session closed for user (\\w+)", 2, LOGOUT},
    {Parser::EQUAL_COMPARE, "MESSAGE=pam_unix(login:auth): check pass; user unknown", 1, LOGIN_FAIL},
    {Parser::REGEX_MATCH, "MESSAGE=FAILED LOGIN \\(\\d\\) on '/dev/pts/\\d' from '.*' FOR '(\\w+)', Authentication failure", 1, LOGIN_FAIL}
};

std::unique_ptr<LOGIN_INFO> Parser::GetLoginInfo(sd_journal *j)
{
    std::unique_ptr<LOGIN_INFO> loginInfo;
    RemoteCache::ProtoType protoType = GetProtoType(j);
    switch (protoType) {
        case RemoteCache::PROTO_TYPE_SSH: {
            loginInfo = FromSsh(j);
            break;
        }
        case RemoteCache::PROTO_TYPE_GDM: {
            loginInfo = FromGdm(j);
            break;
        }
        case RemoteCache::PROTO_TYPE_LOGIN: {
            loginInfo = FromLogin(j);
            break;
        }
        case RemoteCache::PROTO_TYPE_TELNET: {
            SaveTelnetContext(j);
            break;
        }
        case RemoteCache::PROTO_TYPE_DROPBEAR: {
            loginInfo = FromDropBear(j);
            break;
        }
        default: {
            std::cerr << "unknown proto type" << std::endl;
            break;
        }
    }
    return loginInfo;
}

RemoteCache::ProtoType Parser::GetProtoType(sd_journal *j)
{
    RemoteCache::ProtoType protoType = RemoteCache::PROTO_TYPE_UNKNOWN;
    std::string indentifer = Utils::GetTextValue(j, "SYSLOG_IDENTIFIER");
    if (indentifer == "sshd") {
        protoType = RemoteCache::PROTO_TYPE_SSH;
    } else if (indentifer == "gdm-password]") {
        protoType = RemoteCache::PROTO_TYPE_GDM;
    } else if (indentifer == "login") {
        protoType = RemoteCache::PROTO_TYPE_LOGIN;
    } else if (indentifer == "in.telnetd") {
        protoType = RemoteCache::PROTO_TYPE_TELNET;
    } else if (indentifer == "dropbear") {
        protoType = RemoteCache::PROTO_TYPE_DROPBEAR;
    }
    return protoType;
}

std::unique_ptr<LOGIN_INFO> Parser::BuildLoginInfo(unsigned long time, const std::vector<Parser::SceneArgs>::const_iterator &iter, std::unique_ptr<Parser::MatchResult> mr, LOGIN_TYPE loginType)
{
    auto loginInfo = std::make_unique<LOGIN_INFO>();
    if (!loginInfo) {
        return nullptr;
    }
    loginInfo->type = iter->type;
    loginInfo->time = time;
    loginInfo->bLoginSuccess = iter->status == LOGIN_SUCCESS;
    loginInfo->loginType = loginType;
    loginInfo->clientIp = mr->remoteIp;
    loginInfo->clientWorkstation = Utils::GetHostName();
    loginInfo->loginUser = mr->loginUser;
    loginInfo->loginDomain = "";
    loginInfo->status = iter->status;
    loginInfo->userId = Utils::GetUid(mr->loginUser);
    loginInfo->clientPort = mr->remotePort;
    loginInfo->subjectUserId = loginInfo->userId;
    return loginInfo;
}

std::unique_ptr<LOGIN_INFO> Parser::FromSsh(sd_journal *j)
{
    std::string msg = Utils::GetText(j, "MESSAGE");
    std::unique_ptr<LOGIN_INFO> loginInfo;
    std::unique_ptr<MatchResult> mr;
    for (auto iter = SSH_SCENE.begin(); iter != SSH_SCENE.end(); iter++) {
        mr = SshRegexMatch(iter, msg);
        if (mr) {
            unsigned long time = Utils::GetTimeStamp(j);
            loginInfo = BuildLoginInfo(time, iter, std::move(mr), NETWORK);
            break;
        }
    }
    return loginInfo;
}

std::unique_ptr<Parser::MatchResult> Parser::SshRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg)
{
    std::unique_ptr<MatchResult> mr;
    std::regex express(iter->pattern);
    std::smatch results;
    if (std::regex_match(msg, results, express)) {
        std::cout << "[SSH] Log: " << msg << std::endl;
        std::cout << "[SSH] Hit: " << iter->pattern << std::endl;
        mr = std::make_unique<MatchResult>();
        if (!mr) {
            return nullptr;
        }
        mr->loginUser = results[1].str();
        mr->remoteIp = results[2].str();
        mr->remotePort = results[4].str();
    }
    return mr;
}

std::unique_ptr<LOGIN_INFO> Parser::FromGdm(sd_journal *j)
{
    std::string msg = Utils::GetText(j, "MESSAGE");
    std::unique_ptr<LOGIN_INFO> loginInfo;
    std::unique_ptr<MatchResult> mr;
    for (auto iter = GDM_SCENE.begin(); iter != GDM_SCENE.end(); iter++) {
        if (iter->method == Parser::EQUAL_COMPARE) {
            mr = GdmEqualCompare(iter, msg);
        } else if (iter->method == REGEX_MATCH) {
            mr = GdmRegexMatch(iter, msg);
        }
        if (mr) {
            unsigned long time = Utils::GetTimeStamp(j);
            loginInfo = BuildLoginInfo(time, iter, std::move(mr), SERVICE);
            break;
        }
    }
    return loginInfo;
}

std::unique_ptr<Parser::MatchResult> Parser::GdmEqualCompare(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg)
{
    if (iter->pattern == msg) {
        std::cout << "[GDM] Log: " << msg << std::endl;
        std::cout << "[GDM] Hit: " << iter->pattern << std::endl;
        return std::make_unique<MatchResult>();
    }
    return nullptr;
}

std::unique_ptr<Parser::MatchResult> Parser::GdmRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg)
{
    std::unique_ptr<MatchResult> mr;
    std::regex express(iter->pattern);
    std::smatch results;
    if (std::regex_match(msg, results, express)) {
        std::cout << "[GDM] Log: " << msg << std::endl;
        std::cout << "[GDM] Hit: " << iter->pattern << std::endl;
        mr = std::make_unique<MatchResult>();
        if (!mr) {
            return nullptr;
        }
        mr->loginUser = results.size() == 2 ? results[1].str() : "";
    }
    return mr;
}

std::unique_ptr<LOGIN_INFO> Parser::FromDropBear(sd_journal *j)
{
    std::string msg = Utils::GetText(j, "MESSAGE");
    std::unique_ptr<LOGIN_INFO> loginInfo;
    std::unique_ptr<MatchResult> mr;
    pid_t protoPid = Utils::GetPid(j);
    for (auto iter = DROPBEAR_SCENE.begin(); iter != DROPBEAR_SCENE.end(); iter++) {
        // 1. 匹配
        if (iter->method == REGEX_MATCH) {
            mr = DropBearRegexMatch(iter, msg, protoPid);
        } else if (iter->method == REGEX_SEARCH) {
            DropBearRegexSearch(iter, msg, protoPid);
            return nullptr;
        }
        // 2. 封装
        if (mr) {
            unsigned long time = Utils::GetTimeStamp(j);
            loginInfo = BuildLoginInfo(time, iter, std::move(mr), NETWORK);
            break;
        }
    }
    return loginInfo;
}

std::unique_ptr<Parser::MatchResult> Parser::DropBearRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t protoPid)
{
    std::unique_ptr<Parser::MatchResult> mr;
    std::regex express(iter->pattern);
    std::smatch results;
    if (std::regex_match(msg, results, express)) {
        std::cout << "[DROPBEAR] Log: " << msg << std::endl;
        std::cout << "[DROPBEAR] Hit: " << iter->pattern << std::endl;
        auto regexIndex = std::distance(DROPBEAR_SCENE.begin(), iter);
        mr = DropBearGetMatchResult(regexIndex, results, protoPid);
        if (!mr) {
            return nullptr;
        }
        if (regexIndex == 1) {
            m_remoteCache->DeleteByProtoPid(protoPid);
        } else {
            RemoteCache::RemoteItem item{protoPid, 0, RemoteCache::PROTO_TYPE_DROPBEAR, mr->remoteIp, mr->remotePort};
            m_remoteCache->InsertItem(item);
        }
        m_remoteCache->DumpCache();
    }
    return mr;
}

void Parser::DropBearRegexSearch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t protoPid)
{
    std::regex express(iter->pattern);
    if (std::regex_search(msg, express)) {
        std::cout << "[DROPBEAR] Log: " << msg << std::endl;
        std::cout << "[DROPBEAR] Hit: " << iter->pattern << std::endl;
        m_remoteCache->DeleteByProtoPid(protoPid);
        m_remoteCache->DumpCache();
    }
}

std::unique_ptr<Parser::MatchResult> Parser::DropBearGetMatchResult(int regexIndex, const std::smatch &results, pid_t protoPid)
{
    auto mr = std::make_unique<Parser::MatchResult>();
    if (!mr) {
        return nullptr;
    }
    if (regexIndex == 0 || regexIndex == 2) {
        mr->loginUser = results[1].str();
        mr->remoteIp = results[2].str();
        mr->remotePort = results[4].str();
    } else if (regexIndex == 1) {
        mr->loginUser = results[1].str();
        auto remoteItem = m_remoteCache->GetByProtoPid(protoPid);
        if (!remoteItem) {
            return nullptr;
        }
        mr->remoteIp = remoteItem->remoteIp;
        mr->remotePort = remoteItem->remotePort;
    } else if (regexIndex == 3) {
        mr->remoteIp = results[1].str();
        mr->remotePort = results[3].str();
    }
    return mr;
}

std::unique_ptr<LOGIN_INFO> Parser::FromLogin(sd_journal *j)
{
    std::unique_ptr<LOGIN_INFO> loginInfo;
    LOGIN_TYPE type = GetLoginType(j);
    if (type == SERVICE) { // tty
        loginInfo = FromTty(j);
    } else if (type == NETWORK) {
        if (IsExpectedProto(j, "in.telnetd", RemoteCache::PROTO_TYPE_TELNET)) {
            loginInfo = FromTelnet(j);
        } else if (IsExpectedProto(j, "busybox", RemoteCache::PROTO_TYPE_BUSYBOX)) {
            loginInfo = FromBusyBox(j);
        }
    }
    return loginInfo;
}

LOGIN_TYPE Parser::GetLoginType(sd_journal *j)
{
    LOGIN_TYPE type;
    std::string cmdLine = Utils::GetText(j, "_CMDLINE");
    if (cmdLine == "_CMDLINE=/bin/login -p --") {
        type = SERVICE; // tty
    } else {
        type = NETWORK;
    }
    return type;
}

bool Parser::IsExpectedProto(sd_journal *j, const std::string &protoName, RemoteCache::ProtoType protoType)
{
    pid_t loginPid = Utils::GetPid(j);
    bool isMeet;
    pid_t protoPid = Utils::GetPpidByPid(loginPid);
    if (protoPid) {
        std::string name = Utils::GetNameByPid(protoPid);
        isMeet = name == protoName;
    } else { // login进程已经关闭，即/proc/<login pid>/目录不存在
        auto remoteItem = m_remoteCache->GetByAuthPid(loginPid);
        isMeet = remoteItem ? remoteItem->protoType == protoType : false;
    }
    return isMeet;
}

std::unique_ptr<LOGIN_INFO> Parser::FromTty(sd_journal *j)
{
    std::string msg = Utils::GetText(j, "MESSAGE");
    pid_t loginPid = Utils::GetPid(j);
    std::unique_ptr<LOGIN_INFO> loginInfo;
    std::unique_ptr<MatchResult> mr;
    for (auto iter = LOGIN_SCENE.begin(); iter != LOGIN_SCENE.end(); iter++) {
        if (iter->method == Parser::EQUAL_COMPARE) {
            mr = TtyEqualCompare(iter, msg, loginPid);
        } else if (iter->method == REGEX_MATCH) {
            mr = TtyRegexMatch(iter, msg, loginPid);
        }
        if (mr) {
            unsigned long time = Utils::GetTimeStamp(j);
            loginInfo = BuildLoginInfo(time, iter, std::move(mr), SERVICE);
            break;
        }
    }
    return loginInfo;
}

std::unique_ptr<Parser::MatchResult> Parser::TtyEqualCompare(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid)
{
    if (iter->pattern == msg) {
        std::cout << "[TTY] Log: " << msg << std::endl;
        std::cout << "[TTY] Hit: " << iter->pattern << std::endl;
        return std::make_unique<MatchResult>();
    }
    return nullptr;
}

std::unique_ptr<Parser::MatchResult> Parser::TtyRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid)
{
    std::unique_ptr<MatchResult> mr;
    std::regex express(iter->pattern);
    std::smatch results;
    if (std::regex_match(msg, results, express)) {
        std::cout << "[TTY] Log: " << msg << std::endl;
        std::cout << "[TTY] Hit: " << iter->pattern << std::endl;
        mr = std::make_unique<MatchResult>();
        if (!mr) {
            return nullptr;
        }
        mr->loginUser = results.size() == 2 ? results[1].str() : "";
    }
    return mr;
}

bool Parser::SaveTelnetContext(sd_journal *j)
{
    pid_t telnetPid = Utils::GetPid(j);
    std::string msg = Utils::GetText(j, "MESSAGE");
    std::regex express(TELNET_CONNECTION_SCENE);
    std::smatch results;
    if (std::regex_match(msg, results, express)) {
        std::cout << "[TELNET] Log: " << msg << std::endl;
        std::cout << "[TELNET] Hit: " << TELNET_CONNECTION_SCENE << std::endl;
        auto activePorts = Utils::GetActivePorts(telnetPid);
        RemoteCache::RemoteItem item{telnetPid, 0, RemoteCache::PROTO_TYPE_TELNET, results[1].str(), activePorts[0]};
        m_remoteCache->InsertItem(item);
    }
    m_remoteCache->DumpCache();
    return true;
}

std::unique_ptr<LOGIN_INFO> Parser::FromTelnet(sd_journal *j)
{
    std::string msg = Utils::GetText(j, "MESSAGE");
    std::unique_ptr<LOGIN_INFO> loginInfo;
    std::unique_ptr<MatchResult> mr;
    pid_t loginPid = Utils::GetPid(j);
    for (auto iter = TELNET_LOGIN_SCENE.begin(); iter != TELNET_LOGIN_SCENE.end(); iter++) {
        std::string loginUser;
        if (iter->method == Parser::EQUAL_COMPARE) {
            mr = TelnetEqualCompare(iter, msg, loginPid);
        } else if (iter->method == REGEX_MATCH) {
            mr = TelnetRegexMatch(iter, msg, loginPid);
        }
        if (mr) {
            unsigned long time = Utils::GetTimeStamp(j);
            loginInfo = BuildLoginInfo(time, iter, std::move(mr), NETWORK);
            break;
        }
    }
    return loginInfo;
}

std::unique_ptr<Parser::MatchResult> Parser::TelnetEqualCompare(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid)
{
    std::unique_ptr<MatchResult> mr;
    if (iter->pattern == msg) {
        std::cout << "[TELNET] Log: " << msg << std::endl;
        std::cout << "[TELNET] Hit: " << iter->pattern << std::endl;
        mr = std::make_unique<MatchResult>();
        if (!mr) {
            return nullptr;
        }
        auto [remoteIp, remotePort] = TelnetGetMatchResult(std::distance(TELNET_LOGIN_SCENE.begin(), iter), loginPid);
        mr->remoteIp = remoteIp;
        mr->remotePort = remotePort;
    }
    return mr;
}

std::unique_ptr<Parser::MatchResult> Parser::TelnetRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid)
{
    std::unique_ptr<MatchResult> mr;
    std::regex express(iter->pattern);
    std::smatch results;
    if (std::regex_match(msg, results, express)) {
        std::cout << "[TELNET] Log: " << msg << std::endl;
        std::cout << "[TELNET] Hit: " << iter->pattern << std::endl;
        mr = std::make_unique<MatchResult>();
        if (!mr) {
            return nullptr;
        }
        mr->loginUser = results.size() == 2 ? results[1].str() : "";
        auto [remoteIp, remotePort] = TelnetGetMatchResult(std::distance(TELNET_LOGIN_SCENE.begin(), iter), loginPid);
        mr->remoteIp = remoteIp;
        mr->remotePort = remotePort;
    }
    return mr;
}

std::tuple<std::string, std::string> Parser::TelnetGetMatchResult(int regexIndex, pid_t loginPid)
{
    std::unique_ptr<RemoteCache::RemoteItem> remoteItem;
    if (regexIndex == 1) {
        remoteItem = m_remoteCache->GetByAuthPid(loginPid);
        if (!remoteItem) {
            return {};
        }
        m_remoteCache->DeleteByAuthPid(loginPid);
    } else {
        pid_t telnetPid = Utils::GetPpidByPid(loginPid);
        remoteItem = m_remoteCache->GetByProtoPid(telnetPid);
        if (!remoteItem) {
            return {};
        }
        m_remoteCache->UpdateAuthPid(telnetPid, loginPid);
    }
    m_remoteCache->DumpCache();
    return std::make_tuple(remoteItem->remoteIp, remoteItem->remotePort);
}

std::unique_ptr<LOGIN_INFO> Parser::FromBusyBox(sd_journal *j)
{
    std::string msg = Utils::GetText(j, "MESSAGE");
    pid_t loginPid = Utils::GetPid(j);
    std::unique_ptr<LOGIN_INFO> loginInfo;
    std::unique_ptr<MatchResult> mr;
    for (auto iter = BUSYBOX_LOGIN_SCENE.begin(); iter != BUSYBOX_LOGIN_SCENE.end(); iter++) {
        std::string loginUser;
        std::string remoteIp;
        std::string remotePort;
        if (iter->method == Parser::EQUAL_COMPARE) {
            mr = BusyBoxEqualCompare(iter, msg, loginPid);
        } else if (iter->method == REGEX_MATCH) {
            mr = BusyBoxRegexMatch(iter, msg, loginPid);
        }
        if (mr) {
            unsigned long time = Utils::GetTimeStamp(j);
            loginInfo = BuildLoginInfo(time, iter, std::move(mr), NETWORK);
            break;
        }
    }
    return loginInfo;
}

std::unique_ptr<Parser::MatchResult> Parser::BusyBoxEqualCompare(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid)
{
    std::unique_ptr<MatchResult> mr;
    if (iter->pattern == msg) {
        std::cout << "[BUSYBOX] Log: " << msg << std::endl;
        std::cout << "[BUSYBOX] Hit: " << iter->pattern << std::endl;
        mr = std::make_unique<MatchResult>();
        if (!mr) {
            return nullptr;
        }
        pid_t busyBoxPid = Utils::GetPpidByPid(loginPid);
        auto remoteItem = m_remoteCache->GetByProtoPid(busyBoxPid);
        if (remoteItem) {
            mr->remoteIp = remoteItem->remoteIp;
            mr->remotePort = remoteItem->remotePort;
        } else {
            auto remoteSocks = Utils::GetActiveSockets(busyBoxPid);
            mr->remoteIp = remoteSocks[0].first;
            mr->remotePort = remoteSocks[0].second;
            RemoteCache::RemoteItem item{busyBoxPid, loginPid, RemoteCache::PROTO_TYPE_BUSYBOX, mr->remoteIp, mr->remotePort};
            m_remoteCache->InsertItem(item);
            m_remoteCache->DumpCache();
        }
    }
    return mr;
}

std::unique_ptr<Parser::MatchResult> Parser::BusyBoxRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid)
{
    std::unique_ptr<MatchResult> mr;
    std::regex express(iter->pattern);
    std::smatch results;
    if (std::regex_match(msg, results, express)) {
        std::cout << "[BUSYBOX] Log: " << msg << std::endl;
        std::cout << "[BUSYBOX] Hit: " << iter->pattern << std::endl;
        mr = std::make_unique<MatchResult>();
        if (!mr) {
            return nullptr;
        }
        mr->loginUser = results[1].str();
        // std::fprintf(stdout, "****protoPid=%d, authPid=%d, loginUser=%s\n", busyBoxPid, loginPid, loginUser.c_str());
        auto regexIndex = std::distance(BUSYBOX_LOGIN_SCENE.begin(), iter);
        auto [remoteIp, remotePort] = BusyBoxGetMatchResult(regexIndex, loginPid);
        mr->remoteIp = remoteIp;
        mr->remotePort = remotePort;
    }
    return mr;
}

std::tuple<std::string, std::string> Parser::BusyBoxGetMatchResult(int regexIndex, pid_t loginPid)
{
    pid_t busyBoxPid = Utils::GetPpidByPid(loginPid);
    std::string remoteIp;
    std::string remotePort;
    if (regexIndex == 1) {
        auto remoteItem = m_remoteCache->GetByAuthPid(loginPid);
        if (remoteItem) {
            remoteIp = remoteItem->remoteIp;
            remotePort = remoteItem->remotePort;
        }
        m_remoteCache->DeleteByAuthPid(loginPid);
        m_remoteCache->DumpCache();
    } else {
        auto remoteItem = m_remoteCache->GetByProtoPid(busyBoxPid);
        if (remoteItem) {
            remoteIp = remoteItem->remoteIp;
            remotePort = remoteItem->remotePort;
        } else {
            auto remoteSocks = Utils::GetActiveSockets(busyBoxPid);
            remoteIp = remoteSocks[0].first;
            remotePort = remoteSocks[0].second;
            RemoteCache::RemoteItem item{busyBoxPid, loginPid, RemoteCache::PROTO_TYPE_BUSYBOX, remoteIp, remotePort};
            m_remoteCache->InsertItem(item);
            m_remoteCache->DumpCache();
        }
    }
    return std::make_tuple(remoteIp, remotePort);
}
