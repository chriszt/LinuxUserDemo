#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <pwd.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>

static const int BUF_SIZE = 50;

std::string Utils::GetText(sd_journal *j, const std::string &key)
{
    const char *data;
    size_t len;
    int ret = sd_journal_get_data(j, key.c_str(), (const void **)&data, &len);
    if (ret < 0) {
        data = "";
    }
    return std::string(data, len);
}

std::string Utils::GetTextValue(sd_journal *j, const std::string &key)
{
    std::string text = GetText(j, key);
    return text.substr(text.find('=') + 1);
}

unsigned long Utils::GetTimeStamp(sd_journal *j)
{
    std::string usecStr = GetTextValue(j, "_SOURCE_REALTIME_TIMESTAMP");
    unsigned long usec = 0;
    try {
        usec = std::stoul(usecStr);
    } catch (const std::exception &e) {
        std::cerr << "convert usStr to us failed: " << e.what() << std::endl;
    }
    return usec;
}

std::string Utils::GetTimeStampPretty(sd_journal *j)
{
    unsigned long usec = GetTimeStamp(j);
    time_t sec = usec / 1000000;
    struct tm tp;
    if (localtime_r(&sec, &tp) == nullptr) {
        return "";
    }
    char buf[BUF_SIZE]{0};
    strftime(buf, BUF_SIZE, "%Y-%m-%d %H:%M:%S", &tp);
    std::string result(buf);
    long ms = (usec % 1000000) / 1000;
    result += "." + std::to_string(ms);
    return result;
}

pid_t Utils::GetPid(sd_journal *j)
{
    std::string pidStr = GetTextValue(j, "_PID");
    pid_t pid = 0;
    try {
        pid = std::stoi(pidStr);
    } catch (const std::exception &e) {
        std::cerr << "convert pidStr to pid failed: " << e.what() << std::endl;
    }
    return pid;
}

std::string Utils::GetUid(const std::string &userName)
{
    struct passwd *pwd = getpwnam(userName.c_str());
    return pwd ? std::to_string(pwd->pw_uid) : "";
}

std::string Utils::GetHostName()
{
    std::ifstream in("/etc/hostname");
    if (!in.is_open()) {
        std::cerr << "open /etc/hostname failed" << std::endl;
        return "";
    }
    std::string hostName;
    std::getline(in, hostName);
    in.close();
    return hostName;
}

std::string Utils::GetNameByPid(pid_t pid)
{
    std::string procPath = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream in(procPath);
    if (!in.is_open()) {
        std::fprintf(stderr, "open %s failed\n", procPath.c_str());
        return "";
    }
    std::string line;
    if (!std::getline(in, line)) {
        in.close();
        return "";
    }
    std::string procName = line.substr(line.find('\t') + 1);
    in.close();
    return procName;
}

pid_t Utils::GetPpidByPid(pid_t pid)
{
    std::string procPath = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream in(procPath);
    if (!in.is_open()) {
        std::fprintf(stderr, "open %s failed\n", procPath.c_str());
        return 0;
    }
    std::string line;
    pid_t ppid;
    while (std::getline(in, line)) {
        if (line.find("PPid:") == std::string::npos) {
            continue;
        }
        try {
            ppid = std::stoi(line.substr(line.find('\t') + 1));
        } catch (const std::exception &e) {
            in.close();
            return 0;
        }
    }
    in.close();
    return ppid;
}

bool Utils::CheckProcessExist(pid_t pid)
{
    std::string procPath = "/proc/" + std::to_string(pid) + "/";
    std::ifstream in(procPath);
    if (!in.is_open()) {
        std::fprintf(stderr, "open %s failed\n", procPath.c_str());
        return false;
    }
    in.close();
    return true;
}

bool Utils::CheckRemotePortExist(pid_t pid, const std::string &port)
{
    auto activePorts = GetActivePorts(pid);
    auto iter = std::find(activePorts.begin(), activePorts.end(), port);
    return iter != activePorts.end();
}

std::vector<std::string> Utils::GetActivePorts(pid_t pid)
{
    std::vector<std::string> ports;
    auto connStates = GetActiveConnectionStates(pid);
    for (auto &state : connStates) {
        if (state) {
            ports.push_back(state->remotePort);
        }
    }
    return ports;
}

std::vector<std::pair<std::string, std::string>> Utils::GetActiveSockets(pid_t pid)
{
    std::vector<std::pair<std::string, std::string>> sockets;
    auto connStates = GetActiveConnectionStates(pid);
    for (auto &state : connStates) {
        if (state) {
            sockets.emplace_back(state->remoteIp, state->remotePort);
        }
    }
    return sockets;
}

std::set<std::string> Utils::GetSocketInodes(pid_t pid)
{
    std::string fdDirPath = "/proc/" + std::to_string(pid) + "/fd/";
    DIR *fdDir = opendir(fdDirPath.c_str());
    if (!fdDir) {
        std::fprintf(stderr, "open %s failed(%d): %s\n", fdDirPath.c_str(), errno, strerror(errno));
        return {};
    }
    // std::fprintf(stdout, "open %s succeeded\n", fdDirPath.c_str());

    std::set<std::string> inodes;
    struct dirent *ent;
    while ((ent = readdir(fdDir)) != NULL) {
        if (ent->d_type != DT_LNK) {
            continue;
        }
        // std::fprintf(stdout, "name=%s, type=%u\n", ent->d_name, ent->d_type);
        std::string lnkPath = fdDirPath + ent->d_name;
        char lnkCnt[1024] = {0};
        if (readlink(lnkPath.c_str(), lnkCnt, sizeof(lnkCnt)) == -1) {
            std::fprintf(stderr, "read link %s failed(%d): %s\n", lnkPath.c_str(), errno, strerror(errno));
            continue;
        }
        std::string lnkContent(lnkCnt);
        if (lnkContent.find("socket") == std::string::npos) {
            continue;
        }
        size_t begin = lnkContent.find('[') + 1;
        size_t length = lnkContent.find(']') - begin;
        std::string inode = lnkContent.substr(begin, length);
        inodes.emplace(inode);
    }

    closedir(fdDir);
    return inodes;
}

std::vector<std::unique_ptr<Utils::ConnectionState>> Utils::GetActiveConnectionStates(pid_t pid)
{
    std::vector<std::unique_ptr<Utils::ConnectionState>> connStates;
    auto inodes = GetSocketInodes(pid);
    std::vector<std::string> paths{"/proc/net/tcp", "/proc/net/udp", "/proc/net/tcp6", "/proc/net/udp6"};
    for (auto inode : inodes) {
        for (auto path : paths) {
            // std::fprintf(stdout, "****path=%s, inode=%u\n", path.c_str(), inode);
            auto connState = GetActiveConnectionState(path, inode);
            if (connState) {
                connStates.push_back(std::move(connState));
            }
        }
    }
    return connStates;
}

std::unique_ptr<Utils::ConnectionState> Utils::GetActiveConnectionState(const std::string &path, const std::string &inode)
{
    std::ifstream in(path);
    if (!in.is_open()) {
        std::fprintf(stderr, "open %s failed\n", path.c_str());
        return nullptr;
    }
    std::unique_ptr<Utils::ConnectionState> connState;
    bool isFound = false;
    std::string line;
    while (std::getline(in, line)) {
        if (line.find(':') == std::string::npos) { // 跳过第1行
            continue;
        }
        connState = ParseConnectionState(line);
        if (!connState) {
            continue;
        }
        if ((connState->state == TCP_ESTABLISHED) && (connState->inode == inode)) {
            isFound = true;
            break;
        }
    }
    in.close();
    if (isFound) {
        return connState;
    }
    return nullptr;
}

std::unique_ptr<Utils::ConnectionState> Utils::ParseConnectionState(const std::string &line)
{
    auto connState = std::make_unique<Utils::ConnectionState>();
    if (!connState) {
        return nullptr;
    }
    std::stringstream lineStream(line);
    std::string field;
    int index = 0;
    while (lineStream >> field) {
        if (index == 1) { // localIp localPort
            auto [localIp, localPort] = GetSocket(field);
            connState->localIp = localIp;
            connState->localPort = localPort;
        }
        if (index == 2) { // remoteIp remotePort
            auto [remoteIp, remotePort] = GetSocket(field);
            connState->remoteIp = remoteIp;
            connState->remotePort = remotePort;
        }
        if (index == 3) { // state
            int state = 0;
            try {
                state = std::stoi(field, nullptr, 16);
            } catch (const std::exception &e) {
                std::cerr << "convert state failed: " << e.what() << std::endl;
            }
            connState->state = (TCP_STATE)state;
        }
        if (index == 9) { // inode
            connState->inode = field;
        }
        index++;
    }
    return connState;
}

std::tuple<std::string, std::string> Utils::GetSocket(const std::string &text)
{
    size_t idx = text.find(':');
    std::string ip = text.substr(0, idx);
    std::string port = text.substr(idx + 1);
    return std::make_tuple(ConvertIp(ip), ConvertPort(port));
}

std::string Utils::ConvertIp(const std::string &ip)
{
    if (ip.size() == 8) {
        return ConvertIpv4(ip);
    } else if (ip.size() == 32) {
        return ConvertIpv6(ip);
    }
    return "";
}

std::string Utils::ConvertIpv4(const std::string &ipv4)
{
    in_addr_t ipAddr = 0;
    try {
        ipAddr = std::stoul(ipv4, nullptr, 16);
    } catch (const std::exception &e) {
        std::cerr << "convert ipv4 failed: " << e.what() << ", " << ipv4 << std::endl;
        return "";
    }
    char ipBuf[INET_ADDRSTRLEN] = {0};
    if (!inet_ntop(AF_INET, &ipAddr, ipBuf, INET_ADDRSTRLEN)) {
        std::cerr << "convert ipv4 addr to array failed" << std::endl;
        return "";
    }
    return ipBuf;
}

std::string Utils::ConvertIpv6(const std::string &ipv6)
{
    in6_addr addr;
    for (size_t i = 0; i < ipv6.size(); i += 2) {
        std::string byteStr = ipv6.substr(i, 2);
        addr.s6_addr[i / 2] = std::stoul(byteStr, nullptr, 16);
    }
    char buf[INET6_ADDRSTRLEN] = {0};
    if (!inet_ntop(AF_INET6, &addr, buf, INET6_ADDRSTRLEN)) {
        std::cerr << "convert ipv6 addr to array failed" << std::endl;
        return "";
    }
    return buf;
}

std::string Utils::ConvertPort(const std::string &port)
{
    unsigned short portNum = 0;
    try {
        portNum = std::stoul(port, nullptr, 16);
    } catch (const std::exception &e) {
        std::cerr << "convert port failed: " << e.what() << std::endl;
    }
    return std::to_string(portNum);
}
