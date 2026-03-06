#pragma once

#include <QObject>
#include <QSslSocket>
#include <QThread>
#include <QFile>
#include <vector>
#include "Protocol.hpp"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void connectToServer(const QString& host, int port);
    void login(const QString& user, const QString& password);
    void registerUser(const QString& user, const QString& password);
    void listGames();
    void addGame(const QString& name);
    void deleteGame(const QString& name);
    void listSaves(const QString& game);
    void addSave(const QString& game, const QString& saveName, const QByteArray& data);
    void deleteSave(const QString& game, const QString& save);

signals:
    void connected();
    void disconnected();
    void loginSuccess(QString token);
    void loginFailed(QString error);
    void registerSuccess(QString token);
    void registerFailed(QString error);
    void gamesReceived(QStringList games);
    void savesReceived(QStringList saves);
    void uploadProgress(int percent);

private slots:
    void onConnected();
    void onReadyRead();
    void onSslErrors(const QList<QSslError>& errors);

private:
    void sendPacket(PacketType type, const std::vector<uint8_t>& body);
    QString readString(const QByteArray& data, int& offset);
    QByteArray encodeString(const QString& s);

    QSslSocket* socket_;
    uint8_t packetType_;
    uint32_t packetSize_;
    bool waitingForAuth_;
    bool isRegistration_;
};
