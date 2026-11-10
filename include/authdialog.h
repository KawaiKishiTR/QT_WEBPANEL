#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>

namespace Ui {
class AuthDialog;
}

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QWidget *parent = nullptr);
    ~AuthDialog();

signals:
    void tokenTaken(const QString token);

private:
    void setupUI();
    void connect_buttons();

    Ui::AuthDialog *ui;
};

#endif // AUTHDIALOG_H
