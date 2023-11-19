#include "RemoteCache.h"
#include <iostream>
#include <unistd.h>
#include "Utils.h"

RemoteCache::RemoteCache(size_t maxCacheSize) : m_maxCacheSize(maxCacheSize)
{
    m_loopThread = std::thread([this]{ LoopProc(); });
}

void RemoteCache::InsertItem(const RemoteItem &item)
{
    std::lock_guard<std::mutex> lck(m_cacheMtx);
    if (m_cache.size() >= m_maxCacheSize) {
        return;
    }
    if (m_cache.find(item.protoPid) != m_cache.end()) {
        return;
    }
    auto ptr = std::make_unique<RemoteItem>();
    if (!ptr) {
        return;
    }
    ptr->protoPid = item.protoPid;
    ptr->authPid = item.authPid;
    ptr->protoType = item.protoType;
    ptr->remoteIp = item.remoteIp;
    ptr->remotePort = item.remotePort;
    m_cache[item.protoPid] = std::move(ptr);
}

void RemoteCache::DeleteByProtoPid(pid_t protoPid)
{
    std::lock_guard<std::mutex> lck(m_cacheMtx);
    auto iter = m_cache.find(protoPid);
    if (iter != m_cache.end()) {
        m_cache.erase(protoPid);
    }
}

void RemoteCache::DeleteByAuthPid(pid_t authPid)
{
    std::lock_guard<std::mutex> lck(m_cacheMtx);
    for (auto iter = m_cache.begin(); iter != m_cache.end();) {
        if (iter->second->authPid == authPid) {
            m_cache.erase(iter++);
            return;
        }
        iter++;
    }
}

void RemoteCache::UpdateAuthPid(pid_t protoPid, pid_t authPid)
{
    std::lock_guard<std::mutex> lck(m_cacheMtx);
    auto iter = m_cache.find(protoPid);
    if (iter != m_cache.end()) {
        iter->second->authPid = authPid;
    }
}

std::unique_ptr<RemoteCache::RemoteItem> RemoteCache::GetByProtoPid(pid_t protoPid)
{
    std::lock_guard<std::mutex> lck(m_cacheMtx);
    auto iter = m_cache.find(protoPid);
    if (iter != m_cache.end()) {
        auto ptr = std::make_unique<RemoteItem>();
        if (!ptr) {
            return nullptr;
        }
        *ptr = *iter->second;
        return ptr;
    }
    return nullptr;
}

std::unique_ptr<RemoteCache::RemoteItem> RemoteCache::GetByAuthPid(pid_t authPid)
{
    std::lock_guard<std::mutex> lck(m_cacheMtx);
    for (auto &item : m_cache) {
        if (item.second->authPid == authPid) {
            auto ptr = std::make_unique<RemoteItem>();
            if (!ptr) {
                return nullptr;
            }
            *ptr = *item.second;
            return ptr;
        }
    }
    return nullptr;
}

void RemoteCache::DumpCache()
{
    std::cout << "----------REMOTE CACHE--------------" << std::endl;
    std::cout << "Proto PID: Proto PID, Auth PID, Proto Type, Remote Socket" << std::endl;
    std::lock_guard<std::mutex> lck(m_cacheMtx);
    for (auto &item : m_cache) {
        std::fprintf(stdout, "%9d: %9d, %8d, %10d, %s:%s\n",
            item.first, item.second->protoPid, item.second->authPid, item.second->protoType,
            item.second->remoteIp.c_str(), item.second->remotePort.c_str());
    }
    std::cout << "----------REMOTE CACHE END----------" << std::endl;
}

void RemoteCache::LoopProc()
{
    std::cout << "LoopProc() start" << std::endl;
    while (!m_exitLoop) {
        sleep(SLEEP_SECOND);  // 改为优雅退出
        DeleteItems();
        DumpCache();
    }
    std::cout << "LoopProc() stop" << std::endl;
}

// 1.判断Proto PID是不是init进程，如果是则保留，反之执行步骤2
// 2.查看Proto PID是否存在，不存在删除该条目，如果存在执行步骤3
// 3.通过Proto PID查看remote port是否存在，不存在删除该条目，如果存在则保留
void RemoteCache::DeleteItems()
{
    for (auto iter = m_cache.begin(); iter != m_cache.end();) {
        if (iter->first == 1) {
            iter++;
            continue;
        }
        if (!Utils::CheckProcessExist(iter->first) ||
            !Utils::CheckRemotePortExist(iter->first, iter->second->remotePort)) {
            m_cache.erase(iter++);
            continue;
        }
        iter++;
    }
}
