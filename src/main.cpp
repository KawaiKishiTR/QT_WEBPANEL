#include "common/KTokenM.h"
#include "common/config.h"
#include "common/KNetworkM.h"

#include "mainwindow.h"
#include "starterwindow.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) 
{
    // 1. Ortam değişkenini oku (Örn: Config::ENV_KEY = "LAUNCHED_BY")
    QByteArray envVal = qgetenv(Config::ENV_KEY.toUtf8().constData());

    // 2. Değişken tanımlı mı ve değeri beklediğimiz anahtarla eşleşiyor mu?
    if (envVal.isEmpty() || envVal != Config::SECRET_KEY.toUtf8()) {
        std::cerr << "[GÜVENLİK] Hata: Uygulama yalnızca yetkili başlatıcı (Updater) üzerinden açılabilir!" << std::endl;
        
        // Uygulamayı HİÇ başlatmadan doğrudan 1 koduyla sonlandır
        return 1; 
    }

    int exitCode = 0;
    do {

    QApplication a(argc, argv);
    QNetworkAccessManager *nm (new QNetworkAccessManager());
    setNetworkM(nm);

    Config::loadBinaryConfig();
    if (!KTokenM::token_exists()) {
        std::cerr << "token bulunmuyor uygulama durduruldu" << std::endl;
        return 1;
    }
    KTokenM::get_token_info();
    if (KTokenM::is_starter()) {
        Config::saveBinaryConfig();
        std::cout << "[APP STARTER] starter key ile başlatılıyor" << std::endl;
        static StarterWindow w;
        w.show();
    } else {
        static MainWindow w;
        w.show();
    }
    exitCode = QApplication::exec();
    } while (exitCode == Config::EXIT_CODE_REBOOT);
}
