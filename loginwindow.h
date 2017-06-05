#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QDesktopWidget>
#include <QDebug>

namespace Ui {
class LoginWindow;
}

//class for login form which allows to get login and password that user typed
class LoginWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

    QString get_login();
    QString get_password();

private slots:

    void on_login_buttonBox_accepted();

    void on_login_buttonBox_rejected();

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
