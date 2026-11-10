#pragma once

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


inline QNetworkAccessManager*& getNetworkM() {
    static QNetworkAccessManager *nm = nullptr;
    return nm;
}

inline void setNetworkM(QNetworkAccessManager *networkManager) {
    getNetworkM() = networkManager;
}

class RequestExecutor {
    QNetworkRequest m_request;

public:
    explicit RequestExecutor(QNetworkRequest request) 
        : m_request(std::move(request)) {}

    // GET İsteği
    [[nodiscard]] QNetworkReply* get() const {
        return getNetworkM()->get(m_request);
    }

    // POST İsteği (Verili veya Boş Body)
    [[nodiscard]] QNetworkReply* post(const QByteArray &data = "") const {
        return getNetworkM()->post(m_request, data);
    }

    // PUT İsteği
    [[nodiscard]] QNetworkReply* put(const QByteArray &data = "") const {
        return getNetworkM()->put(m_request, data);
    }

    // DELETE İsteği
    [[nodiscard]] QNetworkReply* deleteResource() const {
        return getNetworkM()->deleteResource(m_request);
    }

    // Gerekirse ham QNetworkRequest'i almak için dönüşüm operatörü
    operator QNetworkRequest() const { return m_request; }
    const QNetworkRequest& rawRequest() const { return m_request; }
};
