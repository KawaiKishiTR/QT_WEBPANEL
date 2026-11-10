#ifndef STARTERWINDOW_H
#define STARTERWINDOW_H

#include <QMainWindow>

namespace Ui {
class StarterWindow;
}

class StarterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StarterWindow(QWidget *parent = nullptr);
    ~StarterWindow();

private:
    void connect_buttons();

    Ui::StarterWindow *ui;
};

#endif // STARTERWINDOW_H
