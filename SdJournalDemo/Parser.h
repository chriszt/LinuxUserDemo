#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include <functional>
#include <regex>
#include <systemd/sd-journal.h>
#include "LoginDef.h"
#include "RemoteCache.h"

class Parser {
public:
    enum COMPARE_METHOD {
        EQUAL_COMPARE,
        REGEX_MATCH,
        REGEX_SEARCH
    };
    struct SceneArgs {
        COMPARE_METHOD method;
        std::string pattern;
        int type;
        LOGIN_STATUS status;
    };
    struct MatchResult {
        std::string loginUser;
        std::string remoteIp;
        std::string remotePort;
    };    

    std::unique_ptr<LOGIN_INFO> GetLoginInfo(sd_journal *j);

private:
    RemoteCache::ProtoType GetProtoType(sd_journal *j);
    std::unique_ptr<LOGIN_INFO> BuildLoginInfo(unsigned long time, const std::vector<Parser::SceneArgs>::const_iterator &iter, std::unique_ptr<Parser::MatchResult> mr, LOGIN_TYPE loginType);

    std::unique_ptr<LOGIN_INFO> FromSsh(sd_journal *j);
    std::unique_ptr<MatchResult> SshRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg);
    
    std::unique_ptr<LOGIN_INFO> FromGdm(sd_journal *j);
    std::unique_ptr<MatchResult> GdmEqualCompare(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg);
    std::unique_ptr<MatchResult> GdmRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg);

    std::unique_ptr<LOGIN_INFO> FromDropBear(sd_journal *j);
    std::unique_ptr<MatchResult> DropBearRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t protoPid);
    void DropBearRegexSearch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t protoPid);
    std::unique_ptr<MatchResult> DropBearGetMatchResult(int regexIndex, const std::smatch &results, pid_t protoPid);

    std::unique_ptr<LOGIN_INFO> FromLogin(sd_journal *j);
    LOGIN_TYPE GetLoginType(sd_journal *j);
    bool IsExpectedProto(sd_journal *j, const std::string &protoName, RemoteCache::ProtoType protoType);
    
    std::unique_ptr<LOGIN_INFO> FromTty(sd_journal *j);
    std::unique_ptr<MatchResult> TtyEqualCompare(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid);
    std::unique_ptr<MatchResult> TtyRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid);

    bool SaveTelnetContext(sd_journal *j);
    std::unique_ptr<LOGIN_INFO> FromTelnet(sd_journal *j);
    std::unique_ptr<MatchResult> TelnetEqualCompare(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid);
    std::unique_ptr<MatchResult> TelnetRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid);
    std::tuple<std::string, std::string> TelnetGetMatchResult(int regexIndex, pid_t loginPid);
    
    std::unique_ptr<LOGIN_INFO> FromBusyBox(sd_journal *j);
    std::unique_ptr<MatchResult> BusyBoxEqualCompare(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid);
    std::unique_ptr<MatchResult> BusyBoxRegexMatch(const std::vector<Parser::SceneArgs>::const_iterator &iter, const std::string &msg, pid_t loginPid);
    std::tuple<std::string, std::string> BusyBoxGetMatchResult(int regexIndex, pid_t loginPid);
    
private:
    std::unique_ptr<RemoteCache> m_remoteCache = std::make_unique<RemoteCache>();
};

#endif
