#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent): QDialog(parent), ui(new Ui::LoginWindow) {
    ui->setupUi(this);

    this->setModal(true);
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    ui->password_lineEdit->setEchoMode(QLineEdit::Password);
    ui->login_lineEdit->setText("controller1");
    ui->password_lineEdit->setText("controller1psswrd");
}

LoginWindow::~LoginWindow() {
    delete ui;
}

QString LoginWindow::get_login() {
    return ui->login_lineEdit->text();
}

QString LoginWindow::get_password() {
    return ui->password_lineEdit->text();
}


void LoginWindow::on_login_buttonBox_accepted()
{
    this->accept();
}

void LoginWindow::on_login_buttonBox_rejected()
{
    this->reject();
}

