#include "authdialog.h"
#include "common/KTokenM.h"
#include "ui_authdialog.h"
#include <QFile>
#include <iostream>


AuthDialog::AuthDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AuthDialog)
{
    ui->setupUi(this);
    setupUI();
    connect_buttons();


}

void AuthDialog::connect_buttons() {
    connect(ui->btnSubmit, &QPushButton::clicked, [this](){
        QString takenToken = ui->keyEdit->text();
        std::cout << "[AuthDial] tokenTaken: " << takenToken.toStdString() << std::endl;
        if (!KTokenM::token_exists(takenToken)) {
            ui->lblStatus->setText("Geçersiz Token Girdiniz!");
            ui->keyEdit->setText("");
            return;
        }

        Config::AppConfig.API_KEY = takenToken;
        Config::saveBinaryConfig();

        QApplication::exit(Config::EXIT_CODE_REBOOT);

    });
}

void AuthDialog::setupUI() {
    setWindowTitle("Yetki Yükseltme");

    QPixmap originalPixmap = QPixmap(":/dock_logo.png");
    QPixmap scaledPixmap = originalPixmap.scaled(
        originalPixmap.size() / 2,
        Qt::KeepAspectRatio
    );
    ui->lblIco->setPixmap(scaledPixmap);
}

AuthDialog::~AuthDialog() {
    delete ui;
}
