#pragma once

#include <QMainWindow>
#include "NetworkManager.hpp"
#include "LoginDialog.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnected();
    void onDisconnected();
    void onLoginSuccess(QString token);
    void onLoginFailed(QString error);
    void onRegisterSuccess(QString token);
    void onRegisterFailed(QString error);
    void onActionLogin_triggered();
    void onActionRegister_triggered();

private:
    void showLoginDialog(bool registration);
    void connectToServer();

    Ui::MainWindow *ui;
    NetworkManager* networkManager_;
    QString authToken_;
    bool isLoggedIn_;
};
