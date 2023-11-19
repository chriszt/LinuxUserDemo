#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <memory>
#include <tuple>
#include <vector>
#include <set>
#include <systemd/sd-journal.h>
#include "LoginDef.h"

class Utils {
public:
    enum NET_LAYER_PROTOCOL {
        NET_LAYER_IP_V4,
        NET_LAYER_IP_V6
    };
    enum TCP_STATE {
        TCP_ESTABLISHED = 1,
        TCP_SYN_SENT,
        TCP_SYN_RECV,
        TCP_FIN_WAIT1,
        TCP_FIN_WAIT2,
        TCP_TIME_WAIT,
        TCP_CLOSE,
        TCP_CLOSE_WAIT,
        TCP_LAST_ACK,
        TCP_LISTEN,
        TCP_CLOSING,	/* Now a valid state */
        TCP_NEW_SYN_RECV,
        TCP_MAX_STATES	/* Leave at the end! */
    };

    struct ConnectionState {
        NET_LAYER_PROTOCOL netProto;
        std::string localIp;
        std::string localPort;
        std::string remoteIp;
        std::string remotePort;
        TCP_STATE state;
        std::string inode;

        void Show()
        {
            std::cout << netProto << " "
                      << localIp << ":" << localPort << " "
                      << remoteIp << ":" << remotePort << " "
                      << state << " "
                      << inode << std::endl;
        }
    };

    static std::string GetText(sd_journal *j, const std::string &key);
    static std::string GetTextValue(sd_journal *j, const std::string &key);
    // _SOURCE_REALTIME_TIMESTAMP=1689974860356593 --> 1689974860356593
    static unsigned long GetTimeStamp(sd_journal *j);
    // _SOURCE_REALTIME_TIMESTAMP=1689974860356593 --> 2023-07-22 05:27:40.356
    static std::string GetTimeStampPretty(sd_journal *j);
    static pid_t GetPid(sd_journal *j);

    static std::string GetUid(const std::string &userName);
    static std::string GetHostName();
    static std::string GetNameByPid(pid_t pid);
    static pid_t GetPpidByPid(pid_t pid);
    static bool CheckProcessExist(pid_t pid);
    static bool CheckRemotePortExist(pid_t pid, const std::string &port);
    static std::vector<std::string> GetActivePorts(pid_t pid);
    static std::vector<std::pair<std::string, std::string>> GetActiveSockets(pid_t pid);

private:
    static std::set<std::string> GetSocketInodes(pid_t pid);
    static std::vector<std::unique_ptr<Utils::ConnectionState>> GetActiveConnectionStates(pid_t pid);
    static std::unique_ptr<Utils::ConnectionState> GetActiveConnectionState(const std::string &path, const std::string &inode);
    static std::unique_ptr<Utils::ConnectionState> ParseConnectionState(const std::string &line);
    static std::tuple<std::string, std::string> GetSocket(const std::string &text);
    static std::string ConvertIp(const std::string &ip);
    static std::string ConvertIpv4(const std::string &ipv4);
    static std::string ConvertIpv6(const std::string &ipv6);
    static std::string ConvertPort(const std::string &port);
};

#endif
