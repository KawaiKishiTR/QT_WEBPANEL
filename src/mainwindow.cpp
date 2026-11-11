#include "mainwindow.h"
#include "common/KTokenM.h"
#include "common/config.h"
#include "common/KServerAPI.h"
#include <QScrollBar>
#include <QTimer>

QPixmap base64ToPixmap(const QString& base64Data);
void drawFavicon(const QString& base64Data, QLabel *lbl);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _netManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    this->setWindowTitle("Devoilers Server Webpanel (" + KTokenM::get_token_name() + ") olarak giriş yapıldı.");
    setupUI();
    drawFavicon(Config::AppConfig.FAVICON, ui->lblServerIco); // cached favico
    connect_buttons();
    
    if (KTokenM::has_permission(TokenPermission::LOGS)){
        connectToLogStream();
    }

    get_server_info();
}

void MainWindow::setupLogView() {
    // 1. Düzenleme kapatılır
    ui->txtLogs->setReadOnly(true);

    // 2. Bellek taşmasını önlemek için tampon sınırı (Örn: son 2000 satır)
    // Eğer QPlainTextEdit kullanıyorsanız:
    ui->txtLogs->setMaximumBlockCount(1000);

    // 3. Konsol fontu ve koyu tema
    QFont monoFont("Monospace");
    monoFont.setStyleHint(QFont::Monospace);
    ui->txtLogs->setFont(monoFont);

    if (!KTokenM::has_permission(TokenPermission::LOGS)) {
        ui->txtLogs->hide(); //TODO: stackedWidget ile log yerine placeholder.
    }
}

void MainWindow::connectToLogStream() {
    if (!m_webSocket) {
        m_webSocket = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);

        // Sinyalleri bağla
        connect(m_webSocket, &QWebSocket::connected, this, &MainWindow::onWsConnected);
        connect(m_webSocket, &QWebSocket::disconnected, this, &MainWindow::onWsDisconnected);
        connect(m_webSocket, &QWebSocket::textMessageReceived, this, &MainWindow::onTextMessageReceived);
        
        // Hata sinyali (Qt 6)
        connect(m_webSocket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
            ui->txtLogs->appendHtml(QString("<span style='color: red;'>[HATA] WebSocket: %1</span>")
                                    .arg(m_webSocket->errorString()));
        });
    }

    // WebSocket URL'i: ws://SERVER_IP:PORT/action/get_logtail
    QString wsUrl = QString("ws://%1:%2/minecraft/get_logtail")
                        .arg(Config::AppConfig.SERVER_HOST)
                        .arg(Config::AppConfig.SERVER_PORT);

    QNetworkRequest request((QUrl(wsUrl)));
    
    // API anahtarı başlığını ekle
    request.setRawHeader("x-api-key", Config::AppConfig.API_KEY.toUtf8());

    ui->txtLogs->appendHtml("<span style='color: gray;'>[BILGI] Sunucu log akışına bağlanılıyor...</span>");
    
    // Bağlantıyı başlat
    m_webSocket->open(request);
}

void MainWindow::onWsConnected() {
    ui->txtLogs->appendHtml("<span style='color: #4CAF50;'>[BAGLANDI] Canlı log akışı başladı.</span>");
}

void MainWindow::onWsDisconnected() {
    ui->txtLogs->appendHtml("<span style='color: orange;'>[KOPTU] Log bağlantısı kesildi.</span>");
    ui->txtLogs->appendHtml("<span style='color: gray;'>[BILGI] 15sn içinde yeniden bağlanmayı deneyecek.</span>");
    QTimer::singleShot(15000, [this](){
        connectToLogStream();
    });
}

// Sunucudan her yeni satır/mesaj geldiğinde bu slot tetiklenir:
void MainWindow::onTextMessageReceived(const QString &message) {
    // 1. Yeni satırı ekle
    ui->txtLogs->appendPlainText(message); // QTextBrowser ise: ui->txtLogs->append(message);

    // 2. İmleci ve kaydırma çubuğunu en alta indir (Auto-scroll)
    ui->txtLogs->verticalScrollBar()->setValue(ui->txtLogs->verticalScrollBar()->maximum());
}

void MainWindow::onRconSend() {
    QJsonObject payload;
    payload["command"] = ui->lineEdit->text().trimmed();

    QByteArray postData = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    QNetworkRequest request = ServerAPI::COMMAND.toRequest();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = getNetworkM()->post(request, postData);
    ServerAPI::ReplyGC gc(reply);
    ServerAPI::awaitReply(reply);

    
    if (reply->error() == QNetworkReply::NoError) {
        ui->lineEdit->clear(); // Girdi kutusunu temizle
    } else {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        std::cerr << "Komut yürütme hatası (" << statusCode << "):" << reply->errorString().toStdString();
    }
}

MainWindow::~MainWindow() {
    if (m_webSocket) {
        m_webSocket->close();
    }
    delete ui;
}

void MainWindow::setupUI() {
    setupLogView();

    if (KTokenM::has_permission(TokenPermission::START)) {
        ui->stackStart->setCurrentWidget(ui->pageStartBtn);
    } else {ui->stackStart->setCurrentWidget(ui->pageStartLock);}

    if (KTokenM::has_permission(TokenPermission::RESTART)) {
        ui->stackRestart->setCurrentWidget(ui->pageRestartBtn);
    } else {ui->stackRestart->setCurrentWidget(ui->pageRestartLock);}

    if (KTokenM::has_permission(TokenPermission::STOP)) {
        ui->stackStop->setCurrentWidget(ui->pageStopBtn);
    } else {ui->stackStop->setCurrentWidget(ui->pageStopLock);}

    if (!KTokenM::has_permission(TokenPermission::ADMIN)) {
        ui->btnUpdateManifest->hide();
    }

    if (!KTokenM::has_permission(TokenPermission::RCON)) {
        ui->lineEdit->hide();
        ui->btnSendCommand->hide();
    }
}

void MainWindow::connect_buttons() {
    connect(ui->btnStart, &QPushButton::clicked, [this](){
        QNetworkReply *reply = ServerAPI::START.toRequest().post();
        ServerAPI::ReplyGC lock(reply);
        ServerAPI::awaitReply(reply);
        reply->deleteLater();
    });

    connect(ui->btnStop, &QPushButton::clicked, [this](){
        QNetworkReply *reply = ServerAPI::STOP.toRequest().post();
        ServerAPI::ReplyGC lock(reply);
        ServerAPI::awaitReply(reply);
        reply->deleteLater();
    });

    connect(ui->btnRestart, &QPushButton::clicked, [this](){
        QNetworkReply *reply = ServerAPI::RESTART.toRequest().post();
        ServerAPI::ReplyGC lock(reply);
        ServerAPI::awaitReply(reply);
        reply->deleteLater();
    });

    connect(ui->btnUpdateManifest, &QPushButton::clicked, [this](){
        QNetworkReply *reply = ServerAPI::UPDATE_MANIFESTS.toRequest().post();
        ServerAPI::ReplyGC lock(reply);
        ServerAPI::awaitReply(reply);
        reply->deleteLater();
    });

    connect(ui->btnSendCommand, &QPushButton::clicked, [this](){
        onRconSend();
    });
}

void MainWindow::get_server_info() {
    QNetworkReply *reply = ServerAPI::SERVER_INFO.toRequest().get();
    ServerAPI::ReplyGC lock(reply);
    ServerAPI::awaitReply(reply);
    QByteArray rawData = reply->readAll(); 
    onServerInfoReceived(rawData);
}

void MainWindow::onServerInfoReceived(QByteArray &rawData) {

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    QJsonObject jsonObj = doc.object();

    // Json parsing
    QString faviconBase64 = jsonObj.value(QStringLiteral("favicon")).toString();
    QString version       = jsonObj.value(QStringLiteral("version")).toString();
    QString hostname      = jsonObj.value(QStringLiteral("hostname")).toString();
    // QString maxplayers    = jsonObj.value(QStringLiteral("maxplayers")).toString();
    // comes empty

    ui->lblServerInfo->setText(version + " " + hostname);
    drawFavicon(faviconBase64, ui->lblServerIco);
}

QPixmap base64ToPixmap(const QString& base64Data) {
    if (base64Data.isEmpty()) {
        return QPixmap();
    }

    // 1. "data:image/png;base64," gibi önekler varsa ayıkla
    qsizetype commaIdx = base64Data.indexOf(u',');
    QStringView cleanData = (commaIdx != -1) 
        ? QStringView(base64Data).mid(commaIdx + 1) 
        : QStringView(base64Data);

    // 2. Base64'ten ham baytlara (Raw Bytes) çözümle
    QByteArray imageBytes = QByteArray::fromBase64(cleanData.toUtf8());

    // 3. QPixmap içerisine yükle
    QPixmap pixmap;
    // Formatı açıkça "PNG" belirtmek hata payını sıfıra indirir
    bool success = pixmap.loadFromData(imageBytes, "PNG");

    if (!success || pixmap.isNull()) {
        qWarning() << "[Hata] Favicon verisi Pixmap'e yuklenemedi!";
        return QPixmap();
    }

    return pixmap;
}

void drawFavicon(const QString& base64Data, QLabel *lbl) {
    lbl->setPixmap(base64ToPixmap(base64Data));
}

/*
# ----------------------------
#  DYNAMIC ENDPOINT (Polling İçin)
# ----------------------------
@protected_router.get("/server/playerinfo", response_model=PlayerInfo)
async def get_server_playerinfo():
    stat = get_stat()
    return {
        "numplayers": len(stat.players),
        "playerlist": stat.players
    }
*/