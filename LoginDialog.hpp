#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getLogin() const;
    QString getPassword() const;
    bool isRegistration() const;
    void setRegistrationMode(bool registration);

private slots:
    void on_btnLogin_clicked();
    void on_btnRegister_clicked();
    void on_btnToggleMode_clicked();

private:
    Ui::LoginDialog *ui;
    bool registrationMode_;
};
