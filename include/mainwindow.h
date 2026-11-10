#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *_netManager;
    QWebSocket *m_webSocket = nullptr;

    // websocket - logs
    void setupLogView();
    void connectToLogStream();
    void onWsConnected();
    void onWsDisconnected();
    void onTextMessageReceived(const QString &message);

    void setupUI();
    void connect_buttons();
    void onRconSend();
    void _connect_raise_permission();
    void get_server_info();
    void get_token_info();
    void onServerInfoReceived(QByteArray &rawData);
};
#endif // MAINWINDOW_H
