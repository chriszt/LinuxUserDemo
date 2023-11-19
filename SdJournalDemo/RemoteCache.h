#ifndef PROTO_CACHE_H
#define PROTO_CACHE_H

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <tuple>

class RemoteCache {
public:
    enum ProtoType {
        PROTO_TYPE_UNKNOWN,
        PROTO_TYPE_SSH,
        PROTO_TYPE_GDM,
        PROTO_TYPE_LOGIN,
        PROTO_TYPE_TELNET,
        PROTO_TYPE_BUSYBOX,
        PROTO_TYPE_DROPBEAR
    };
    struct RemoteItem {
        pid_t protoPid;
        pid_t authPid;
        ProtoType protoType;
        std::string remoteIp;
        std::string remotePort;
    };

    explicit RemoteCache(size_t maxCacheSize = MAX_CACHE_SIZE);

    void InsertItem(const RemoteItem &item);
    void DeleteByProtoPid(pid_t protoPid);
    void DeleteByAuthPid(pid_t authPid);
    void UpdateAuthPid(pid_t protoPid, pid_t authPid);
    std::unique_ptr<RemoteItem> GetByProtoPid(pid_t protoPid);
    std::unique_ptr<RemoteItem> GetByAuthPid(pid_t authPid);
    void DumpCache();

private:
    void LoopProc();
    void DeleteItems();

private:
    const static size_t MAX_CACHE_SIZE = 100;
    const static unsigned int SLEEP_SECOND = 30;

private:
    // Proto PID: Proto PID, Auth PID, Proto Type, Remote Socket
    // eg. in.telnetd pid: login pid, PROTO_TYPE_TELNET, 192.168.56.2:13545
    std::unordered_map<pid_t, std::unique_ptr<RemoteItem>> m_cache;
    std::mutex m_cacheMtx;
    size_t m_maxCacheSize;
    std::thread m_loopThread;
    bool m_exitLoop = false;
};

#endif
