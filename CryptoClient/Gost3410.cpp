#include "Gost3410.h"
#include "ALL.h"  // Код одногруппника (не трогаем)
#include <QDebug>

Gost3410::Gost3410(QObject *parent)
    : QObject(parent)
{
    if (!gcry_check_version(GCRYPT_VERSION)) {
        qCritical() << "[Gost3410] libgcrypt version mismatch";
    }
    gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

    qDebug() << "[Gost3410] Initialized, libgcrypt version:" << GCRYPT_VERSION;
}

Gost3410::~Gost3410()
{
}

QString Gost3410::generatePrivateKey()
{
    std::string priv = GOST::make_private_key();
    return QString::fromStdString(priv).toUpper();
}


QString Gost3410::generatePublicKey(const QString &privateKeyHex)
{
    std::string priv = privateKeyHex.toLower().toStdString();
    std::string pub = GOST::make_public_key(priv);
    return QString::fromStdString(pub).toUpper();
}


QString Gost3410::computeHash(const QString &data)
{
    std::string input = data.toStdString();
    std::string hash = GOST::make_hash(input);
    return QString::fromStdString(hash).toUpper();
}


QString Gost3410::createSignature(const QString &hashHex, const QString &privateKeyHex)
{
    std::string hash = hashHex.toLower().toStdString();
    std::string priv = privateKeyHex.toLower().toStdString();
    std::string sig = GOST::make_signature(hash, priv);
    return QString::fromStdString(sig).toUpper();
}


bool Gost3410::verifySignature(const QString &hashHex,
                               const QString &signatureHex,
                               const QString &publicKeyHex)
{
    Q_UNUSED(publicKeyHex);
    std::string hash = hashHex.toLower().toStdString();
    std::string sig = signatureHex.toLower().toStdString();
    return GOST::verify_signature(hash, sig, "");
}