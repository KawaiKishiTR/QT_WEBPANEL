#include "mainwindow.h"
#include "ui_mainwindow.h"

QString SERVER_IP = "80.208.221.192:8000";
QString API_KEY   = "arkadaslarinizla_paylasacaginiz_guclu_bir_sifre";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    adjustSize();
    setFixedSize(sizeHint());

    networkManager = new QNetworkAccessManager(this);

    // on Başlat
    connect(ui->btnBaslat, &QPushButton::clicked, this, [this](){
        qDebug() << "Başlat Butonuna Tıklandı!";
        // URL Tanımı: "http://%SERVER_IP%/action/start"
        QNetworkRequest request = createRequest(QString("start"));
        QByteArray emptyData = "";

        QNetworkReply *reply = networkManager->post(request, emptyData);
        connect(reply, &QNetworkReply::finished, this, [reply, this] {
            handleReply(reply);
            ui->lblDurum->setText("Durum: Başlatıldı!");
        });
    });

    // on Durdur
    connect(ui->btnDurdur, &QPushButton::clicked, this, [this](){
        qDebug() << "Durdur Butonuna Tıklandı!";
        // URL Tanımı: "http://%SERVER_IP%/action/stop"
        QNetworkRequest request = createRequest(QString("stop"));
        QByteArray emptyData = "";

        QNetworkReply *reply = networkManager->post(request, emptyData);
        connect(reply, &QNetworkReply::finished, this, [reply, this] {
            handleReply(reply);
            ui->lblDurum->setText("Durum: Durduruldu!");
        });
    });

    // on Yeniden Başlat
    connect(ui->btnYenidenBaslat, &QPushButton::clicked, this, [this](){
        qDebug() << "Yeniden Başlat Butonuna Tıklandı!";
        // URL Tanımı: "http://%SERVER_IP%/action/restart"
        QNetworkRequest request = createRequest(QString("restart"));
        QByteArray emptyData = "";

        QNetworkReply *reply = networkManager->post(request, emptyData);
        connect(reply, &QNetworkReply::finished, this, [reply, this] {
            handleReply(reply);
            ui->lblDurum->setText("Durum: Yeniden Başlatıldı!");
        });
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleReply(QNetworkReply *reply) {
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        qDebug() << "Başarılı (" << statusCode << "):" << response;
        ui->lblDurum->setText("Durum: Başarılı");
    } else {
        qDebug() << "Hata (" << statusCode << "):" << reply->errorString();
        ui->lblDurum->setText("Durum: Hata!");
    }
    reply->deleteLater();
}

QNetworkRequest MainWindow::createRequest(QString action) {
    QUrl url(QString("http://%1/action/%2").arg(SERVER_IP).arg(action));
    QNetworkRequest request(url);
    request.setRawHeader("accept", "*/*");
    request.setRawHeader("x-api-key", API_KEY.toUtf8());
    return request;
}