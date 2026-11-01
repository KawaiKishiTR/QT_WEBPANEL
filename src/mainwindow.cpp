#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server_api.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _netManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    connect(ui->btnStart, &QPushButton::clicked, [this](){
        QNetworkRequest request = ServerAPI::post_start();
        QNetworkReply *reply = _netManager->post(request, "");

        connect(reply, &QNetworkReply::finished, [reply](){
            reply->deleteLater();
        });
    });

    connect(ui->btnStop, &QPushButton::clicked, [this](){
        QNetworkRequest request = ServerAPI::post_stop();
        QNetworkReply *reply = _netManager->post(request, "");

        connect(reply, &QNetworkReply::finished, [reply](){
            reply->deleteLater();
        });
    });

    connect(ui->btnRestart, &QPushButton::clicked, [this](){
        QNetworkRequest request = ServerAPI::post_restart();
        QNetworkReply *reply = _netManager->post(request, "");

        connect(reply, &QNetworkReply::finished, [reply](){
            reply->deleteLater();
        });
    });

    connect(ui->btnUpdateManifest, &QPushButton::clicked, [this](){
        QNetworkRequest request = ServerAPI::post_update_manifests();
        QNetworkReply *reply = _netManager->post(request, "");

        connect(reply, &QNetworkReply::finished, [reply](){
            reply->deleteLater();
        });
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
