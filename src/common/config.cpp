#include "common/config.h"
#include <iostream>


namespace Config {

QString get_running_dir() {
    // 1. Çalıştırılan dosyanın yolunu al
    QString execPath = QCoreApplication::applicationFilePath();

    // 2. QFileInfo ile sembolik bağları (symlink), Windows kısayol kalıntılarını ve göreli yolları çöz
    QFileInfo fileInfo(execPath);

    // 3. Orijinal binary dosyasının bulunduğu tam mutlak dizin yolunu döndür
    return fileInfo.canonicalPath();
}

bool saveBinaryConfig() {
    QFile file(get_config_file());

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Config dosyası açılamadı:" << file.errorString();
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);

    out << get_config_magic();

    out << AppConfig.SERVER_HOST;
    out << AppConfig.SERVER_PORT;
    out << AppConfig.API_KEY;
    out << AppConfig.FAVICON;
    
    file.close();
    return true;
}

bool loadBinaryConfig() {
QFile file(get_config_file());

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Config dosyası okunamadı:" << file.errorString();
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);

    // 1. Sihirli sayıyı kontrol et (Dosya bozuk veya farklı bir dosya mı?)
    quint32 magic;
    in >> magic;
    if (magic != get_config_magic()) {
        qDebug() << "Hata: Geçersiz veya bozuk config dosyası formatı!";
        file.close();
        return false;
    }

    in >> AppConfig.SERVER_HOST;
    in >> AppConfig.SERVER_PORT;
    in >> AppConfig.API_KEY;
    in >> AppConfig.FAVICON;

    file.close();
    return true;

}

} // namespace Config
