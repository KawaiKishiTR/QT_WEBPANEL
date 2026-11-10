#include "common/KTokenM.h"
#include "common/KNetworkM.h"
#include "common/KServerAPI.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
#include <QTimer>

#include <iostream>

std::vector<QString> KTokenM::_get_token_info(const QString &token) {
    std::vector<QString> toReturn;
    QString name;
    
    QNetworkReply *reply = ServerAPI::TOKEN_INFO.toRequest(token).get();
    ServerAPI::ReplyGC lock(reply);

    if (!ServerAPI::awaitReply(reply)) {
        std::cerr << "token alınamadı: " << token.toStdString() << std::endl;
        return toReturn;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &err);
    
    do {
        if (err.error != QJsonParseError::NoError) {std::cerr << "Json Parse Hatası: " << err.errorString().toStdString() << std::endl; break;}
        if (!doc.isObject()) {std::cerr << "Json Parse Hatası: değer bir obje değil" << std::endl; break;}
        QJsonObject rootObj = doc.object();

        if (rootObj.contains(QStringLiteral("name")) && rootObj.value(QStringLiteral("name")).isString()) {
            name.assign(rootObj.value(QStringLiteral("name")).toString());
        } else {
            std::cerr << "Token Yanıtı 'name' alanına sahip değil yada string değil" << std::endl; break;
        }

        if (rootObj.contains(QStringLiteral("tags")) && rootObj.value(QStringLiteral("tags")).isArray()) {
            toReturn.reserve(rootObj.value(QStringLiteral("tags")).toArray().size());

            for (auto it : rootObj.value("tags").toArray()) {
                if (it.isString()) {
                    toReturn.push_back(it.toString());
                } else {
                    break;
                }
            }
        } else {
            std::cerr << "Token Yanıtı 'tags' alanına sahip değil yada Array değil" << std::endl; break;
        }

        s_cached_token.assign(token);
        s_cached_name.assign(name);
        s_cached_tags = toReturn;

    } while (0);

    std::cout <<  "[Token] Permissions of: " << s_cached_token.toStdString() << " is";
    for (auto it : s_cached_tags) {
        std::cout << " : " << it.toStdString();
    }
    std::cout << std::endl;

    return get_cached_tags();
}

bool KTokenM::_token_exists(const QString &token) {
    if (token == get_cached_token()) {return true;}
    
    QNetworkReply *reply = ServerAPI::TOKEN_EXISTS.toRequest(token).get();
    ServerAPI::ReplyGC lock(reply);
    ServerAPI::awaitReply(reply);

    if (reply->error() == QNetworkReply::NoError) {
        std::cout << "[Token] exists: " << token.toStdString() << std::endl;
        return true;
    }
    std::cout << "[Token] not exists: " << token.toStdString() << std::endl;
    return false;
}

bool KTokenM::_has_permission(const QString &perm) {
    for (auto it : get_cached_tags()) {
        if (it == perm) {
            return true;
        }
    }
    return false;
}



