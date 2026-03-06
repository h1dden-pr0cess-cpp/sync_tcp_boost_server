#include "LoginDialog.hpp"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , registrationMode_(false)
{
    ui->setupUi(this);
    ui->btnRegister->setVisible(false);
    ui->btnToggleMode->setText("Нет аккаунта? Зарегистрироваться");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::getLogin() const
{
    return ui->editLogin->text();
}

QString LoginDialog::getPassword() const
{
    return ui->editPassword->text();
}

bool LoginDialog::isRegistration() const
{
    return registrationMode_;
}

void LoginDialog::setRegistrationMode(bool registration)
{
    registrationMode_ = registration;
    if(registration)
    {
        ui->btnLogin->setVisible(false);
        ui->btnRegister->setVisible(true);
        ui->btnToggleMode->setText("Уже есть аккаунт? Войти");
        setWindowTitle("Регистрация");
    }
    else
    {
        ui->btnLogin->setVisible(true);
        ui->btnRegister->setVisible(false);
        ui->btnToggleMode->setText("Нет аккаунта? Зарегистрироваться");
        setWindowTitle("Вход");
    }
}

void LoginDialog::on_btnLogin_clicked()
{
    if(ui->editLogin->text().isEmpty() || ui->editPassword->text().isEmpty())
        return;
    registrationMode_ = false;
    accept();
}

void LoginDialog::on_btnRegister_clicked()
{
    if(ui->editLogin->text().isEmpty() || ui->editPassword->text().isEmpty())
        return;
    registrationMode_ = true;
    accept();
}

void LoginDialog::on_btnToggleMode_clicked()
{
    setRegistrationMode(!registrationMode_);
}
