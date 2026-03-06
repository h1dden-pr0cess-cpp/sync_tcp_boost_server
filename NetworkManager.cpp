#include "NetworkManager.hpp"
#include <QDataStream>
#include <QCryptographicHash>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , waitingForAuth_(false)
    , isRegistration_(false)
{
    socket_ = new QSslSocket(this);
    connect(socket_, &QSslSocket::connected,
            this, &NetworkManager::onConnected);
    connect(socket_, &QSslSocket::readyRead,
            this, &NetworkManager::onReadyRead);
    connect(socket_, &QSslSocket::sslErrors,
            this, &NetworkManager::onSslErrors);
    connect(socket_, &QSslSocket::disconnected,
            this, &NetworkManager::disconnected);
}

NetworkManager::~NetworkManager() {}

void NetworkManager::connectToServer(const QString& host, int port)
{
    socket_->connectToHostEncrypted(host, port);
}

void NetworkManager::onConnected()
{
    emit connected();
}

void NetworkManager::onSslErrors(const QList<QSslError>& errors)
{
    socket_->ignoreSslErrors();
}

QByteArray NetworkManager::encodeString(const QString& s)
{
    QByteArray data;
    QByteArray str = s.toUtf8();
    uint16_t len = str.size();
    data.append(reinterpret_cast<char*>(&len), sizeof(len));
    data.append(str);
    return data;
}

QString NetworkManager::readString(const QByteArray& data, int& offset)
{
    uint16_t len;
    memcpy(&len, data.data() + offset, sizeof(len));
    offset += sizeof(len);
    QString s = QString::fromUtf8(data.mid(offset, len));
    offset += len;
    return s;
}

void NetworkManager::sendPacket(PacketType type, const std::vector<uint8_t>& body)
{
    QByteArray buffer;
    uint32_t size = body.size();
    buffer.append((char)type);
    buffer.append(reinterpret_cast<char*>(&size), sizeof(size));
    if(size > 0)
        buffer.append(reinterpret_cast<const char*>(body.data()), size);
    socket_->write(buffer);
}

void NetworkManager::login(const QString& user, const QString& password)
{
    isRegistration_ = false;
    waitingForAuth_ = true;

    QByteArray body;
    QString hash = QString(
        QCryptographicHash::hash(
            password.toUtf8(),
            QCryptographicHash::Sha256
            ).toHex()
        );
    body.append(encodeString(user));
    body.append(encodeString(hash));
    std::vector<uint8_t> vec(body.begin(), body.end());
    sendPacket(PacketType::LoginWithPassword, vec);
}

void NetworkManager::registerUser(const QString& user, const QString& password)
{
    isRegistration_ = true;
    waitingForAuth_ = true;

    QByteArray body;
    QString hash = QString(
        QCryptographicHash::hash(
            password.toUtf8(),
            QCryptographicHash::Sha256
            ).toHex()
        );
    body.append(encodeString(user));
    body.append(encodeString(hash));
    std::vector<uint8_t> vec(body.begin(), body.end());
    sendPacket(PacketType::RegisterUser, vec);
}

void NetworkManager::listGames()
{
    sendPacket(PacketType::ListGames, {});
}

void NetworkManager::addGame(const QString& name)
{
    QByteArray body = encodeString(name);
    std::vector<uint8_t> vec(body.begin(), body.end());
    sendPacket(PacketType::AddGame, vec);
}

void NetworkManager::deleteGame(const QString& name)
{
    QByteArray body = encodeString(name);
    std::vector<uint8_t> vec(body.begin(), body.end());
    sendPacket(PacketType::DeleteGame, vec);
}

void NetworkManager::listSaves(const QString& game)
{
    QByteArray body = encodeString(game);
    std::vector<uint8_t> vec(body.begin(), body.end());
    sendPacket(PacketType::ListSaves, vec);
}

void NetworkManager::addSave(const QString& game, const QString& saveName, const QByteArray& data)
{
    QByteArray body;
    body.append(encodeString(game));
    body.append(encodeString(saveName));
    uint16_t len = data.size();
    body.append(reinterpret_cast<char*>(&len), sizeof(len));
    body.append(data);
    std::vector<uint8_t> vec(body.begin(), body.end());
    sendPacket(PacketType::AddSave, vec);
}

void NetworkManager::deleteSave(const QString& game, const QString& save)
{
    QByteArray body;
    body.append(encodeString(game));
    body.append(encodeString(save));
    std::vector<uint8_t> vec(body.begin(), body.end());
    sendPacket(PacketType::DeleteSave, vec);
}

void NetworkManager::onReadyRead()
{
    while(socket_->bytesAvailable() >= 5)
    {
        socket_->read((char*)&packetType_, 1);
        socket_->read((char*)&packetSize_, 4);
        QByteArray body = socket_->read(packetSize_);

        PacketType type = (PacketType)packetType_;

        if(type == PacketType::AuthResponse && waitingForAuth_)
        {
            waitingForAuth_ = false;

            // Парсим ответ: success (1 byte) + message + token
            int offset = 0;
            uint8_t success = body[offset++];
            QString message = readString(body, offset);
            QString token = readString(body, offset);

            if(success)
            {
                if(isRegistration_)
                    emit registerSuccess(token);
                else
                    emit loginSuccess(token);
            }
            else
            {
                if(isRegistration_)
                    emit registerFailed(message);
                else
                    emit loginFailed(message);
            }
        }
        else if(type == PacketType::ListGames)
        {
            QStringList games;
            int offset = 0;
            while(offset < body.size())
            {
                games.append(readString(body, offset));
            }
            emit gamesReceived(games);
        }
        else if(type == PacketType::ListSaves)
        {
            QStringList saves;
            int offset = 0;
            while(offset < body.size())
            {
                saves.append(readString(body, offset));
            }
            emit savesReceived(saves);
        }
    }
}
