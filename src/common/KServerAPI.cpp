#include "common/KServerAPI.h"
#include <QTimer>
#include <iostream>

namespace ServerAPI {

bool awaitReply(QNetworkReply *reply) {
    if (!reply) return false;

    // İstek zaten tamamlanmışsa döngüye gerek yok
    if (reply->isFinished()) {
        return reply->error() == QNetworkReply::NoError;
    }
    
    QEventLoop loop;
    
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QTimer::singleShot(5000, &loop, [&loop, reply](){
        if (loop.isRunning()) {
            reply->abort();
        }
        loop.quit();
    });

    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {return true;}
    if (reply->error() == QNetworkReply::OperationCanceledError) {
        std::cerr << "İstek kullanıcı veya zaman aşımı tarafından iptal edildi (abort)." << std::endl;
    } else {
        std::cerr << "Bir ağ hatası oluştu: " << reply->errorString().toStdString() << std::endl;
    }
    
    return false;
}

} // namespace ServerAPI