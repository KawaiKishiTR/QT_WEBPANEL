#pragma once
#include "config.h"
#include <QNetworkRequest>


namespace ServerAPI {
inline QNetworkRequest create_request(QString endpoint) {
    QUrl url(Config::get_url() + endpoint);
    QNetworkRequest request(url);
    request.setRawHeader("x-api-key", Config::APP_CONFIG.API_KEY.toUtf8());
    return request;
}

inline QNetworkRequest get_ping() {return create_request("/ping");}
inline QNetworkRequest get_token_permissions() {return create_request("/get_token_permissions");}
inline QNetworkRequest get_query_data() {return create_request("/action/get_query_data");}
inline QNetworkRequest post_start() {return create_request("/action/start");}
inline QNetworkRequest post_stop() {return create_request("/action/stop");}
inline QNetworkRequest post_restart() {return create_request("/action/restart");}
inline QNetworkRequest post_command() {return create_request("/command");}
inline QNetworkRequest post_update_manifests() {return create_request("/version/update_manifests");}
inline QNetworkRequest ws_get_logtail() {return create_request("/action/get_logtail");}

}