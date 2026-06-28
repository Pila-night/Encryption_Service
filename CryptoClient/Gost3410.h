#ifndef GOST3410_H
#define GOST3410_H

#include <QObject>
#include <QString>

class Gost3410 : public QObject
{
    Q_OBJECT

public:
    explicit Gost3410(QObject *parent = nullptr);
    ~Gost3410();

    QString generatePrivateKey();
    QString generatePublicKey(const QString &privateKeyHex);

    QString computeHash(const QString &data);

    QString createSignature(const QString &hashHex, const QString &privateKeyHex);
    bool verifySignature(const QString &hashHex,
                         const QString &signatureHex,
                         const QString &publicKeyHex);
};

#endif // GOST3410_H