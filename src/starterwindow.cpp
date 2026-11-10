#include "starterwindow.h"
#include "authdialog.h"
#include "common/KServerAPI.h"
#include "ui_starterwindow.h"

StarterWindow::StarterWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StarterWindow)
{
    ui->setupUi(this);
    connect_buttons();
    // 1. Butonun boyutunu logonun boyutuna kilitleyin
    ui->btnUpgrade->setFixedSize(256, 256);

    // 2. Logonuzu ikon olarak atayın ve boyutunu tam 256x256 yapın
    ui->btnUpgrade->setIcon(QIcon(":/dock_logo.png")); // veya dosya yolu
    ui->btnUpgrade->setIconSize(QSize(256, 256));

}

void StarterWindow::connect_buttons() {
    connect(ui->btnStart, &QPushButton::clicked, [](){
        QNetworkReply *reply = ServerAPI::START.toRequest().post();
        ServerAPI::ReplyGC gc(reply);
        ServerAPI::awaitReply(reply);
    });

    connect(ui->btnUpgrade, &QPushButton::clicked, [this](){
        ui->btnUpgrade->setEnabled(false);

        AuthDialog dialog;
        int result = dialog.exec();

        ui->btnUpgrade->setEnabled(true);
    });
}

StarterWindow::~StarterWindow()
{
    delete ui;
}
