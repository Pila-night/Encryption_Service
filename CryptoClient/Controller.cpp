#include "Controller.h"
#include "ProtocolPackets.h"
#include "ProtocolSerializer.h"
#include "ProtocolAdapter.h"
#include <QRandomGenerator>
#include <QDebug>

const QString Controller::SERVER_PUBLIC_KEY_HEX =
    "3AE19C089FBFDF4EC733A13274AABAD4EF0331777E762D0798AC2F21BF5E1B32"
    "E62B4D12EB5CADD4F3224B2C0D71F477ECF462B676DCBB421C45CEA3B12F549E";

Controller::Controller(MainWindow *view, QObject *parent)
    : QObject(parent)
    , m_view(view)
    , m_network(new Network(this))
    , m_packetHandler(new PacketHandler(this))
    , m_gost(new Gost3410(this))
    , m_state(HandshakeState::Idle)
{
    connect(m_view, &MainWindow::connectRequested, this, &Controller::onConnectedRequested);
    connect(m_view, &MainWindow::disconnectRequested, this, &Controller::onDisconnectedRequested);
    connect(m_view, &MainWindow::performRequested, this, &Controller::onPerformRequested);
    connect(m_view, &MainWindow::clearRequested, this, &Controller::onClearRequested);

    connect(m_network, &Network::connected, this, &Controller::onNetworkConnected);
    connect(m_network, &Network::disconnected, this, &Controller::onNetworkDisconnected);
    connect(m_network, &Network::errorOccurred, this, &Controller::onNetworkError);

    connect(m_network, &Network::dataReceived, m_packetHandler, &PacketHandler::feedData);

    connect(m_packetHandler, &PacketHandler::publicKeyReceived, this, &Controller::onPublicKeyReceived);
    connect(m_packetHandler, &PacketHandler::nonceReceived, this, &Controller::onNonceReceived);
    connect(m_packetHandler, &PacketHandler::signatureReceived, this, &Controller::onSignatureReceived);
    connect(m_packetHandler, &PacketHandler::ciphertextReceived, this, &Controller::onCiphertextReceived);
    connect(m_packetHandler, &PacketHandler::plaintextReceived, this, &Controller::onPlaintextReceived);
    connect(m_packetHandler, &PacketHandler::disconnectReceived, this, &Controller::onDisconnectReceived);
    connect(m_packetHandler, &PacketHandler::errorOccurred, this, &Controller::onPacketError);

    initializeKeys();
}

Controller::~Controller() {}

void Controller::initializeKeys()
{
    m_clientPrivateKey = m_gost->generatePrivateKey();
    m_clientPublicKey = m_gost->generatePublicKey(m_clientPrivateKey);

    qDebug() << "[Controller] Private key:" << m_clientPrivateKey;
    qDebug() << "[Controller] Public key:" << m_clientPublicKey;
}

void Controller::resetHandshake()
{
    m_state = HandshakeState::Idle;
    m_serverPublicKey.clear();
    m_clientNonce.clear();
    m_serverNonce.clear();
}

void Controller::onConnectedRequested(const QString &ip, const quint16 &port)
{
    qDebug() << "[Controller] Connecting to" << ip << ":" << port;
    resetHandshake();
    m_network->connectToServer(ip, port);
}

void Controller::onDisconnectedRequested()
{
    qDebug() << "[Controller] Disconnecting";
    sendDisconnect();
    m_network->disconnectFromServer();
    resetHandshake();
}

void Controller::onPerformRequested(const QString &text, bool isEncrypt)
{
    if (m_state != HandshakeState::Authenticated) {
        m_view->showError("Handshake not completed");
        return;
    }

    if (text.isEmpty()) return;

    if (isEncrypt) {
        sendPlaintext(text);
    } else {
        QByteArray ciphertextBytes = QByteArray::fromHex(text.toUtf8());
        if (ciphertextBytes.isEmpty()) {
            m_view->showError("Invalid ciphertext format (hex expected)");
            return;
        }
        sendCiphertext(ciphertextBytes);
    }
}

void Controller::onClearRequested()
{
    m_view->clearPlainTextEdit();
}

void Controller::onNetworkConnected()
{
    qDebug() << "[Controller] TCP connected";
    m_view->showConnectionStatus(true, "TCP connected. Starting handshake...");
    m_state = HandshakeState::Connected;
    sendPublicKey();
}

void Controller::onNetworkDisconnected()
{
    qDebug() << "[Controller] Connection closed";

    if (m_state != HandshakeState::Authenticated &&
        m_state != HandshakeState::Disconnected &&
        m_state != HandshakeState::Idle) {
        qCritical() << "[Controller] Server disconnected during handshake";
        m_view->showError("Server rejected connection");
    } else {
        m_view->showConnectionStatus(false, "Disconnected from server");
    }

    resetHandshake();
    m_state = HandshakeState::Disconnected;
}

void Controller::onNetworkError(const QString &errorMessage)
{
    qCritical() << "[Controller] Network error:" << errorMessage;
    m_view->showError(QString("Network error: %1").arg(errorMessage));
    m_view->showConnectionStatus(false, "Connection error");
    resetHandshake();
    m_state = HandshakeState::Disconnected;
}

void Controller::onPublicKeyReceived(const QByteArray &key)
{
    qDebug() << "[Controller] Received server PublicKey," << key.size() << "bytes";

    if (m_state != HandshakeState::WaitingForServerKey) {
        qWarning() << "[Controller] Unexpected PublicKey in state" << static_cast<int>(m_state);
        return;
    }

    m_serverPublicKey = key;
    sendNonce();
    m_state = HandshakeState::SentNonce;
}

void Controller::onNonceReceived(const QByteArray &nonce)
{
    qDebug() << "[Controller] Received server Nonce," << nonce.size() << "bytes";

    if (m_state != HandshakeState::WaitingForServerNonce) {
        qWarning() << "[Controller] Unexpected Nonce in state" << static_cast<int>(m_state);
        return;
    }

    m_serverNonce = nonce;
    sendSignature(m_serverNonce);
    m_state = HandshakeState::Authenticated;

    qDebug() << "[Controller] Handshake completed";
    m_view->showConnectionStatus(true, "Handshake completed. Connection secured.");
}

void Controller::onSignatureReceived(const QByteArray &signature)
{
    qDebug() << "[Controller] Received server Signature," << signature.size() << "bytes";

    if (m_state != HandshakeState::SentNonce) {
        qWarning() << "[Controller] Unexpected Signature in state" << static_cast<int>(m_state);
        return;
    }

    bool valid = verifyServerSignature(signature);

    if (!valid) {
        qCritical() << "[Controller] Invalid server signature";
        m_view->showError("Authentication failed: invalid server");
        m_network->disconnectFromServer();
        resetHandshake();
        m_state = HandshakeState::Disconnected;
        return;
    }

    qDebug() << "[Controller] Server signature valid";
    m_view->showConnectionStatus(true, "Server authenticated. Waiting for Nonce_S...");
    m_state = HandshakeState::WaitingForServerNonce;
}

void Controller::onCiphertextReceived(const QByteArray &payload)
{
    qDebug() << "[Controller] Received Ciphertext," << payload.size() << "bytes";

    if (m_state == HandshakeState::Authenticated) {
        QString ciphertextHex = QString::fromUtf8(payload.toHex().toUpper());
        m_view->displayResult(ciphertextHex);
    }
}

void Controller::onPlaintextReceived(const QByteArray &payload)
{
    qDebug() << "[Controller] Received Plaintext," << payload.size() << "bytes";

    if (m_state == HandshakeState::Authenticated) {
        QString text = QString::fromUtf8(payload);
        m_view->displayResult(text);
    }
}

void Controller::onDisconnectReceived()
{
    qDebug() << "[Controller] Received Disconnect from server";
    m_view->showConnectionStatus(false, "Server disconnected");
    m_network->disconnectFromServer();
    resetHandshake();
    m_state = HandshakeState::Disconnected;
}

void Controller::onPacketError(const QString &errorMessage)
{
    qCritical() << "[Controller] Packet error:" << errorMessage;
    m_view->showError("Protocol error: " + errorMessage);
}

void Controller::sendPublicKey()
{
    QByteArray keyBytes = QByteArray::fromHex(m_clientPublicKey.toUtf8());

    Protocol::PublicKeyPacket packet;
    packet.key = Protocol::Adapter::toVector(keyBytes);
    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);

    m_network->sendData(Protocol::Adapter::toQByteArray(data));
    m_view->showConnectionStatus(true, "Public key sent. Waiting for server key...");
    m_state = HandshakeState::WaitingForServerKey;

    qDebug() << "[Controller] PublicKey sent," << keyBytes.size() << "bytes";
}

void Controller::sendNonce()
{
    m_clientNonce.resize(64);
    for (int i = 0; i < 64; ++i) {
        m_clientNonce[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }

    Protocol::NoncePacket packet;
    packet.nonce = Protocol::Adapter::toVector(m_clientNonce);
    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);

    m_network->sendData(Protocol::Adapter::toQByteArray(data));
    qDebug() << "[Controller] Nonce_C sent," << m_clientNonce.size() << "bytes";
}

void Controller::sendSignature(const QByteArray &nonceToSign)
{
    QString nonceHex = QString::fromUtf8(nonceToSign.toHex());

    QString hash = m_gost->computeHash(nonceHex);

    QString signatureHex = m_gost->createSignature(hash, m_clientPrivateKey);

    QByteArray signatureBytes = QByteArray::fromHex(signatureHex.toUtf8());

    Protocol::SignaturePacket packet;
    packet.signature = Protocol::Adapter::toVector(signatureBytes);
    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);

    m_network->sendData(Protocol::Adapter::toQByteArray(data));
    qDebug() << "[Controller] Signature_C sent," << signatureBytes.size() << "bytes";
}

void Controller::sendPlaintext(const QString &text)
{
    Protocol::PlaintextPacket packet;
    packet.payload = Protocol::Adapter::toVector(text.toUtf8());
    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);

    m_network->sendData(Protocol::Adapter::toQByteArray(data));
    qDebug() << "[Controller] Plaintext sent," << data.size() << "bytes";
}

void Controller::sendCiphertext(const QByteArray &data)
{
    Protocol::CiphertextPacket packet;
    packet.payload = Protocol::Adapter::toVector(data);
    std::vector<uint8_t> serialized = Protocol::Serializer::serialize(packet);

    m_network->sendData(Protocol::Adapter::toQByteArray(serialized));
    qDebug() << "[Controller] Ciphertext sent," << serialized.size() << "bytes";
}

void Controller::sendDisconnect()
{
    Protocol::DisconnectPacket packet;
    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);
    m_network->sendData(Protocol::Adapter::toQByteArray(data));
    qDebug() << "[Controller] Disconnect sent";
}

bool Controller::verifyServerSignature(const QByteArray &signature)
{
    QString nonceHex = QString::fromUtf8(m_clientNonce.toHex());
    QString hash = m_gost->computeHash(nonceHex);

    QString signatureHex = QString::fromUtf8(signature.toHex());

    bool valid = m_gost->verifySignature(hash, signatureHex, SERVER_PUBLIC_KEY_HEX);

    qDebug() << "[Controller] Signature verification:" << (valid ? "VALID" : "INVALID");
    return valid;
}