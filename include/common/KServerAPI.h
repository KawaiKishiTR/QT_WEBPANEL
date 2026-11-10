#pragma once

#include "config.h"
#include "common/KNetworkM.h"
#include <QNetworkRequest>
#include <iostream>


namespace ServerAPI {

class ReplyGC {
public:
    ReplyGC(QNetworkReply *reply)
    : _reply(reply) {}
    
    ~ReplyGC() {
        std::cout << "[ReplyGC] Deleting: " 
        << _reply->request().url().toString().toStdString() << " : "
        << _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
        << std::endl;
        _reply->deleteLater();}

private:
    QNetworkReply *_reply;
};

bool awaitReply(QNetworkReply *reply);

// Ortak istek oluşturucu
inline RequestExecutor make_request(std::string_view path, const QString &apiKey = Config::AppConfig.API_KEY) {
    QUrl url(Config::get_url() + QString::fromUtf8(path.data(), static_cast<qsizetype>(path.size())));
    QNetworkRequest request(url);
    request.setRawHeader("x-api-key", apiKey.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return RequestExecutor(std::move(request));
}

template <const std::string_view& PathView>
struct Route {
    static constexpr std::string_view path = PathView;

    // .toRequest() fonksiyonu
    [[nodiscard]] RequestExecutor toRequest(const QString &apiKey) const {
        return make_request(path, apiKey);
    }
    [[nodiscard]] RequestExecutor toRequest() const {
        return make_request(path, Config::AppConfig.API_KEY);
    }
};

namespace Paths {
    inline constexpr std::string_view Ping              = "/ping";

    inline constexpr std::string_view TokenInfo         = "/token/get_info";
    inline constexpr std::string_view TokenExists       = "/token/exists";

    inline constexpr std::string_view Start             = "/minecraft/start";
    inline constexpr std::string_view Stop              = "/minecraft/stop";
    inline constexpr std::string_view Restart           = "/minecraft/restart";
    inline constexpr std::string_view Command           = "/minecraft/send_command";

    inline constexpr std::string_view WS_GET_LOGTAIL    = "/action/get_logtail"; // bulunmuyor
    
    inline constexpr std::string_view ServerInfo        = "/server/query/info";
    inline constexpr std::string_view ServerPlayerInfo  = "/server/query/playerinfo";

    inline constexpr std::string_view UpdateManifests   = "/version/update_manifests";
}

// 2. Route Tanımları (Field şablonundan türeyen tipler)
using PingRoute             = Route<Paths::Ping>;
using TokenInfoRoute        = Route<Paths::TokenInfo>;
using TokenExistsRoute      = Route<Paths::TokenExists>;
using StartRoute            = Route<Paths::Start>;
using StopRoute             = Route<Paths::Stop>;
using RestartRoute          = Route<Paths::Restart>;
using CommandRoute          = Route<Paths::Command>;
using UpdateManifestsRoute  = Route<Paths::UpdateManifests>;
using ServerInfoRoute       = Route<Paths::ServerInfo>;
using ServerPlayerInfoRoute = Route<Paths::ServerPlayerInfo>;

// 3. Doğrudan çağırabilmeniz için inline constexpr değişkenler
inline constexpr PingRoute             PING;
inline constexpr TokenInfoRoute        TOKEN_INFO;
inline constexpr TokenExistsRoute      TOKEN_EXISTS;
inline constexpr StartRoute            START;
inline constexpr StopRoute             STOP;
inline constexpr RestartRoute          RESTART;
inline constexpr CommandRoute          COMMAND;
inline constexpr UpdateManifestsRoute  UPDATE_MANIFESTS;
inline constexpr ServerInfoRoute       SERVER_INFO;
inline constexpr ServerPlayerInfoRoute SERVER_PLAYERINFO;


} // namespace ServerAPI