#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager_(new NetworkManager(this))
    , isLoggedIn_(false)
{
    ui->setupUi(this);

    connect(networkManager_, &NetworkManager::connected,
            this, &MainWindow::onConnected);
    connect(networkManager_, &NetworkManager::disconnected,
            this, &MainWindow::onDisconnected);
    connect(networkManager_, &NetworkManager::loginSuccess,
            this, &MainWindow::onLoginSuccess);
    connect(networkManager_, &NetworkManager::loginFailed,
            this, &MainWindow::onLoginFailed);
    connect(networkManager_, &NetworkManager::registerSuccess,
            this, &MainWindow::onRegisterSuccess);
    connect(networkManager_, &NetworkManager::registerFailed,
            this, &MainWindow::onRegisterFailed);

    connect(ui->actionLogin, &QAction::triggered,
            this, &MainWindow::onActionLogin_triggered);
    connect(ui->actionRegister, &QAction::triggered,
            this, &MainWindow::onActionRegister_triggered);

    connectToServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectToServer()
{
    networkManager_->connectToServer("localhost", 12345);
}

void MainWindow::showLoginDialog(bool registration)
{
    LoginDialog dialog(this);
    dialog.setRegistrationMode(registration);

    if(dialog.exec() == QDialog::Accepted)
    {
        QString login = dialog.getLogin();
        QString password = dialog.getPassword();

        if(dialog.isRegistration())
        {
            networkManager_->registerUser(login, password);
        }
        else
        {
            networkManager_->login(login, password);
        }
    }
}

void MainWindow::onConnected()
{
    ui->statusbar->showMessage("Подключено к серверу");
}

void MainWindow::onDisconnected()
{
    ui->statusbar->showMessage("Отключено от сервера");
    isLoggedIn_ = false;
    authToken_.clear();
}

void MainWindow::onLoginSuccess(QString token)
{
    authToken_ = token;
    isLoggedIn_ = true;
    ui->statusbar->showMessage("Вход выполнен успешно");
    QMessageBox::information(this, "Вход", "Вы успешно вошли в систему!");
}

void MainWindow::onLoginFailed(QString error)
{
    ui->statusbar->showMessage("Ошибка входа");
    QMessageBox::critical(this, "Ошибка входа", error);
}

void MainWindow::onRegisterSuccess(QString token)
{
    authToken_ = token;
    isLoggedIn_ = true;
    ui->statusbar->showMessage("Регистрация выполнена успешно");
    QMessageBox::information(this, "Регистрация", "Вы успешно зарегистрированы!");
}

void MainWindow::onRegisterFailed(QString error)
{
    ui->statusbar->showMessage("Ошибка регистрации");
    QMessageBox::critical(this, "Ошибка регистрации", error);
}

void MainWindow::onActionLogin_triggered()
{
    if(isLoggedIn_)
    {
        QMessageBox::information(this, "Уже вошли", "Вы уже авторизованы");
        return;
    }
    showLoginDialog(false);
}

void MainWindow::onActionRegister_triggered()
{
    if(isLoggedIn_)
    {
        QMessageBox::information(this, "Уже вошли", "Вы уже авторизованы");
        return;
    }
    showLoginDialog(true);
}
