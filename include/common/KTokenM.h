#pragma once

#include "common/config.h"
#include <vector>
#include <QString>

class TokenPermission {
public:
    static inline const QString START   = "START";
    static inline const QString STOP    = "STOP";
    static inline const QString RESTART = "RESTART";

    static inline const QString RCON    = "RCON";
    static inline const QString LOGS    = "LOGS";

    static inline const QString ADMIN   = "ADMIN";
};

class KTokenM {
public:
    static inline QString  get_cached_token() {return s_cached_token;}
    static inline std::vector<QString>  get_cached_tags() {return s_cached_tags;}

    static inline bool token_exists() {return _token_exists(Config::AppConfig.API_KEY);}
    static inline bool token_exists(const QString &token) {return _token_exists(token);}

    static inline std::vector<QString> get_token_info() {return _get_token_info(Config::AppConfig.API_KEY);}
    static inline std::vector<QString> get_token_info(const QString &token) {return _get_token_info(token);}
    static inline QString get_token_name() {return s_cached_name;}
    
    static inline bool has_permission(const QString &perm) {return _has_permission(perm);};
    static inline bool is_starter() {return ((get_cached_tags().size() == 1) && (get_cached_tags()[0] == TokenPermission::START));}

private:
    static std::vector<QString> _get_token_info(const QString &token);
    static bool _token_exists(const QString &token);
    static bool _has_permission(const QString &perm);

    static inline QString s_cached_token;
    static inline QString s_cached_name;
    static inline std::vector<QString> s_cached_tags;
};