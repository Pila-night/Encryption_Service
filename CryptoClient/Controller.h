#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include "MainWindow.h"
#include "Network.h"
#include "PacketHandler.h"
#include "Gost3410.h"

class Controller : public QObject
{
    Q_OBJECT

public:
    explicit Controller(MainWindow *view, QObject *parent = nullptr);
    ~Controller();

private slots:
    void onConnectedRequested(const QString &ip, const quint16 &port);
    void onDisconnectedRequested();
    void onPerformRequested(const QString &text, bool isEncrypt);
    void onClearRequested();

    void onNetworkConnected();
    void onNetworkDisconnected();
    void onNetworkError(const QString &errorMessage);

    void onPublicKeyReceived(const QByteArray &key);
    void onNonceReceived(const QByteArray &nonce);
    void onSignatureReceived(const QByteArray &signature);
    void onCiphertextReceived(const QByteArray &payload);
    void onPlaintextReceived(const QByteArray &payload);
    void onDisconnectReceived();
    void onPacketError(const QString &errorMessage);

private:
    MainWindow *m_view;
    Network *m_network;
    PacketHandler *m_packetHandler;
    //Gost3410 *m_gost;

    enum class HandshakeState {
        Idle,                    // Не подключен
        Connected,               // TCP подключен
        WaitingForServerKey,     // Отправили свой PublicKey, ждём PublicKey сервера
        SentNonce,               // Отправили Nonce_C, ждём Signature_S
        WaitingForServerNonce,   // Получили Signature_S, ждём Nonce_S
        Authenticated,           // Handshake пройден
        Disconnected             // Отключен
    };

    HandshakeState m_state;

    // === Данные для Handshake ===
    QString m_clientPrivateKey;   // Приватный ключ клиента (hex, 64 символа)
    QString m_clientPublicKey;    // Публичный ключ клиента (hex, 128 символов)
    QByteArray m_serverPublicKey; // Публичный ключ сервера (raw bytes, 64 байта)
    QByteArray m_clientNonce;     // Nonce_C (raw bytes, 64 байта)
    QByteArray m_serverNonce;     // Nonce_S (raw bytes, 64 байта)


    static const QString SERVER_PUBLIC_KEY_HEX;

    void initializeKeys();
    void sendPublicKey();
    void sendNonce();
    void sendSignature(const QByteArray &nonceToSign);
    void sendPlaintext(const QString &text);
    void sendCiphertext(const QByteArray &data);
    void sendDisconnect();

    bool verifyServerSignature(const QByteArray &signature);
    void resetHandshake();
};

#endif // CONTROLLER_H