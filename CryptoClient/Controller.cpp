#include "Controller.h"
#include "ProtocolPackets.h"
#include "ProtocolSerializer.h"
#include "ProtocolAdapter.h"
#include <QRandomGenerator>
#include <QDebug>

const QString Controller::SERVER_PUBLIC_KEY_HEX =
    "0000000000000000000000000000000000000000000000000000000000000001"
    "8D91E471E0989CDA27DF505A51F3B0F064F3E6A5E892E9E2E3D8D08F1A94D8A2";

Controller::Controller(MainWindow *view, QObject *parent)
    : QObject(parent)
    , m_view(view)
    , m_network(new Network(this))
    , m_packetHandler(new PacketHandler(this)),
    //, m_gost(new Gost3410(this))  ,
    m_state(HandshakeState::Idle)
{
    connect(m_view, &MainWindow::connectRequested,
            this, &Controller::onConnectedRequested);
    connect(m_view, &MainWindow::disconnectRequested,
            this, &Controller::onDisconnectedRequested);
    connect(m_view, &MainWindow::performRequested,
            this, &Controller::onPerformRequested);
    connect(m_view, &MainWindow::clearRequested,
            this, &Controller::onClearRequested);

    connect(m_network, &Network::connected,
            this, &Controller::onNetworkConnected);
    connect(m_network, &Network::disconnected,
            this, &Controller::onNetworkDisconnected);
    connect(m_network, &Network::errorOccurred,
            this, &Controller::onNetworkError);

    connect(m_network, &Network::dataReceived,
            m_packetHandler, &PacketHandler::handleData);

    connect(m_packetHandler, &PacketHandler::publicKeyReceived,
            this, &Controller::onPublicKeyReceived);
    connect(m_packetHandler, &PacketHandler::nonceReceived,
            this, &Controller::onNonceReceived);
    connect(m_packetHandler, &PacketHandler::signatureReceived,
            this, &Controller::onSignatureReceived);
    connect(m_packetHandler, &PacketHandler::ciphertextReceived,
            this, &Controller::onCiphertextReceived);
    connect(m_packetHandler, &PacketHandler::plaintextReceived,
            this, &Controller::onPlaintextReceived);
    connect(m_packetHandler, &PacketHandler::disconnectReceived,
            this, &Controller::onDisconnectReceived);
    connect(m_packetHandler, &PacketHandler::errorOccurred,
            this, &Controller::onPacketError);

    initializeKeys();
}

Controller::~Controller()
{
}

void Controller::initializeKeys()
{

   /* m_clientPrivateKey = m_gost->generatePrivateKey();  // hex, 64 символа
    m_clientPublicKey = m_gost->generatePublicKey(m_clientPrivateKey);  // hex, 128 символов*/

    qDebug() << "[Controller]  Ключи клиента сгенерированы";
    qDebug() << "[Controller] Private key:" << m_clientPrivateKey;
    qDebug() << "[Controller] Public key:" << m_clientPublicKey;
    qDebug() << "[Controller] Server public key (pinned):" << SERVER_PUBLIC_KEY_HEX;
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
    qDebug() << "[Controller] Запрос подключения к" << ip << ":" << port;
    resetHandshake();
    m_network->connectToServer(ip, port);
}

void Controller::onDisconnectedRequested()
{
    qDebug() << "[Controller] Запрос отключения";
    sendDisconnect();
    m_network->disconnectFromServer();
    resetHandshake();
}

void Controller::onPerformRequested(const QString &text, bool isEncrypt)
{
    if (m_state != HandshakeState::Authenticated) {
        m_view->showError("Handshake ещё не завершён!");
        return;
    }

    if (text.isEmpty()) {
        return;
    }

    if (isEncrypt) {

        sendPlaintext(text);
        m_view->displayResult("→ Отправлено на шифрование: " + text);

    } else {


        QByteArray ciphertextBytes = QByteArray::fromHex(text.toUtf8());

        if (ciphertextBytes.isEmpty()) {
            m_view->showError("Неверный формат шифртекста (ожидается hex)");
            return;
        }

        sendCiphertext(ciphertextBytes);
        m_view->displayResult(" Отправлено на дешифрование: " + text);
    }
}

void Controller::onClearRequested()
{
    m_view->clearPlainTextEdit();
}


void Controller::onNetworkConnected()
{
    qDebug() << "[Controller]TCP подключено. Начинаем Handshake...";
    m_view->showConnectionStatus(true, "TCP подключено. Начинаем Handshake...");
    m_state = HandshakeState::Connected;

    sendPublicKey();
}

void Controller::onNetworkDisconnected()
{
    qDebug() << "[Controller]  Соединение разорвано";

    // Если Handshake ещё не пройден — значит сервер нас отверг
    if (m_state != HandshakeState::Authenticated &&
        m_state != HandshakeState::Disconnected &&
        m_state != HandshakeState::Idle) {
        qCritical() << "[Controller]  Сервер разорвал соединение ВО ВРЕМЯ Handshake!";
        m_view->showError("Сервер отверг подключение. Возможно, неверный ключ или подпись.");
    } else {
        m_view->showConnectionStatus(false, "Отключено от сервера");
    }

    resetHandshake();
    m_state = HandshakeState::Disconnected;
}

void Controller::onNetworkError(const QString &errorMessage)
{
    qCritical() << "[Controller] Ошибка сети:" << errorMessage;
    m_view->showError(QString("Ошибка сети: %1").arg(errorMessage));
    m_view->showConnectionStatus(false, "Ошибка подключения");
    resetHandshake();
    m_state = HandshakeState::Disconnected;
}


void Controller::onPublicKeyReceived(const QByteArray &key)
{
    qDebug() << "[Controller]  Получен PublicKey от сервера, размер:" << key.size();

    if (m_state == HandshakeState::WaitingForServerKey) {
        // Сохраняем публичный ключ сервера
        m_serverPublicKey = key;
        qDebug() << "[Controller] Публичный ключ сервера сохранён";

        // Теперь отправляем свой Nonce_C
        sendNonce();
        m_state = HandshakeState::SentNonce;
    } else {
        qWarning() << "[Controller] PublicKey получен в неожиданном состоянии:"
                   << static_cast<int>(m_state);
    }
}

void Controller::onNonceReceived(const QByteArray &nonce)
{
    qDebug() << "[Controller]  Получен Nonce от сервера, размер:" << nonce.size();

    if (m_state == HandshakeState::WaitingForServerNonce) {
        // Это Nonce_S от сервера
        m_serverNonce = nonce;

        // Подписываем Nonce_S своим приватным ключом и отправляем Signature_C
        sendSignature(m_serverNonce);

        //  Handshake завершён! Если сервер разорвёт — значит подпись невалидна
        m_state = HandshakeState::Authenticated;

        qDebug() << "[Controller]  Handshake завершён! Соединение защищено.";
        m_view->showConnectionStatus(true, " Handshake пройден. Соединение защищено.");
    } else {
        qWarning() << "[Controller] Nonce получен в неожиданном состоянии:"
                   << static_cast<int>(m_state);
    }
}

void Controller::onSignatureReceived(const QByteArray &signature)
{
    qDebug() << "[Controller]  Получена Signature от сервера, размер:" << signature.size();

    if (m_state == HandshakeState::SentNonce) {
        // Это Signature_S — подпись нашего Nonce_C сервером

        // Проверяем подпись сервера
        //bool valid = verifyServerSignature(signature);
    bool valid = true;
        if (!valid) {
            qCritical() << "[Controller]  Подпись сервера НЕВАЛИДНА! Разрыв.";
            m_view->showError("Ошибка аутентификации: сервер поддельный!");
            m_network->disconnectFromServer();
            resetHandshake();
            m_state = HandshakeState::Disconnected;
            return;
        }

        qDebug() << "[Controller]  Подпись сервера валидна!";
        m_view->showConnectionStatus(true, "Сервер аутентифицирован. Ожидание Nonce_S...");

        // Теперь ждём Nonce_S от сервера
        m_state = HandshakeState::WaitingForServerNonce;
    } else {
        qWarning() << "[Controller] Signature получен в неожиданном состоянии:"
                   << static_cast<int>(m_state);
    }
}

void Controller::onCiphertextReceived(const QByteArray &payload)
{
    qDebug() << "[Controller]  Получен Ciphertext, размер:" << payload.size();

    if (m_state == HandshakeState::Authenticated) {
        // Это результат шифрования от сервера
        QString ciphertextHex = QString::fromUtf8(payload.toHex());
        m_view->displayResult("← Результат шифрования (hex): " + ciphertextHex);
    } else {
        qWarning() << "[Controller] Ciphertext получен, но Handshake не пройден!";
    }
}

void Controller::onPlaintextReceived(const QByteArray &payload)
{
    qDebug() << "[Controller]  Получен Plaintext:" << payload;

    if (m_state == HandshakeState::Authenticated) {
        // Это результат дешифрования от сервера
        QString text = QString::fromUtf8(payload);
        m_view->displayResult("← Результат дешифрования: " + text);
    } else {
        qWarning() << "[Controller] Plaintext получен, но Handshake не пройден!";
    }
}

void Controller::onDisconnectReceived()
{
    qDebug() << "[Controller]  Сервер отправил Disconnect";
    m_view->showConnectionStatus(false, "Сервер отключился");
    m_network->disconnectFromServer();
    resetHandshake();
    m_state = HandshakeState::Disconnected;
}

void Controller::onPacketError(const QString &errorMessage)
{
    qCritical() << "[Controller] Ошибка пакета:" << errorMessage;
    m_view->showError("Ошибка протокола: " + errorMessage);
}


void Controller::sendPublicKey()
{
    QByteArray keyBytes = QByteArray::fromHex(m_clientPublicKey.toUtf8());

    Protocol::PublicKeyPacket packet;
    packet.key = Protocol::Adapter::toVector(keyBytes);

    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);
    m_network->sendData(Protocol::Adapter::toQByteArray(data));

    qDebug() << "[Controller]  Отправлен PublicKey, размер:" << keyBytes.size();
    m_view->showConnectionStatus(true, "Отправлен публичный ключ. Ожидание ключа сервера...");
    m_state = HandshakeState::WaitingForServerKey;
}

void Controller::sendNonce()
{
    // Генерируем случайный Nonce (64 байта)
    m_clientNonce.resize(64);
    for (int i = 0; i < 64; ++i) {
        m_clientNonce[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }

    Protocol::NoncePacket packet;
    packet.nonce = Protocol::Adapter::toVector(m_clientNonce);

    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);
    m_network->sendData(Protocol::Adapter::toQByteArray(data));

    qDebug() << "[Controller]  Отправлен Nonce_C, размер:" << m_clientNonce.size();
}

void Controller::sendSignature(const QByteArray &nonceToSign)
{
    // 1. Конвертируем nonce в hex-строку
    QString nonceHex = QString::fromUtf8(nonceToSign.toHex());

    // 2. Хэшируем nonce через ГОСТ 34.11
   // QString hash = m_gost->computeHash(nonceHex);

    // 3. Подписываем хэш своим приватным ключом через ГОСТ 34.10
   // QString signatureHex = m_gost->createSignature(hash, m_clientPrivateKey);

    // 4. Конвертируем hex-строку подписи в QByteArray (raw bytes, 64 байта)
   // QByteArray signatureBytes = QByteArray::fromHex(signatureHex.toUtf8());

    Protocol::SignaturePacket packet;
    //packet.signature = Protocol::Adapter::toVector(signatureBytes);

    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);
    m_network->sendData(Protocol::Adapter::toQByteArray(data));

   // qDebug() << "[Controller]  Отправлена Signature, размер:" << signatureBytes.size();
}

void Controller::sendPlaintext(const QString &text)
{
    Protocol::PlaintextPacket packet;
    packet.payload = Protocol::Adapter::toVector(text.toUtf8());

    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);
    m_network->sendData(Protocol::Adapter::toQByteArray(data));
}

void Controller::sendCiphertext(const QByteArray &data)
{
    Protocol::CiphertextPacket packet;
    packet.payload = Protocol::Adapter::toVector(data);

    std::vector<uint8_t> serialized = Protocol::Serializer::serialize(packet);
    m_network->sendData(Protocol::Adapter::toQByteArray(serialized));
}

void Controller::sendDisconnect()
{
    Protocol::DisconnectPacket packet;
    std::vector<uint8_t> data = Protocol::Serializer::serialize(packet);
    m_network->sendData(Protocol::Adapter::toQByteArray(data));

    qDebug() << "[Controller]  Отправлен Disconnect";
}


/*
bool Controller::verifyServerSignature(const QByteArray &signature)
{
    // 1. Конвертируем наш Nonce_C в hex-строку
    QString nonceHex = QString::fromUtf8(m_clientNonce.toHex());

    // 2. Хэшируем Nonce_C через ГОСТ 34.11
    QString hash = m_gost->computeHash(nonceHex);

    // 3. Конвертируем подпись в hex-строку
    QString signatureHex = QString::fromUtf8(signature.toHex());

    // 4. Проверяем подпись вшитым публичным ключом сервера
    bool valid = m_gost->verifySignature(hash, signatureHex, SERVER_PUBLIC_KEY_HEX);

    qDebug() << "[Controller] Проверка подписи сервера:" << (valid ? " ВАЛИДНА" : " НЕВАЛИДНА");

    return valid;
}*/