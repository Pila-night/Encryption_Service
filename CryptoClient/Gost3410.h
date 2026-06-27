#ifndef GOST3410_H
#define GOST3410_H

#include <QObject>
#include <QString>
#include <vector>
#include <cstdint>
#include <string>

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

private:
    std::vector<unsigned char> g_N(const std::vector<unsigned char> &h,
                                   const std::vector<unsigned char> &m,
                                   const std::vector<unsigned char> &N);
    std::vector<unsigned char> xor_64(std::vector<unsigned char> a,
                                      std::vector<unsigned char> b);
    std::vector<unsigned char> LPS(std::vector<unsigned char> data);
    std::vector<unsigned char> S_transform(std::vector<unsigned char> data);
    std::vector<unsigned char> P_transform(std::vector<unsigned char> data);
    std::vector<unsigned char> L_transform(std::vector<unsigned char> data);
    void add_512(std::vector<unsigned char> &N);
    void add_block(std::vector<unsigned char> &sigma,
                   const std::vector<unsigned char> &block);
    std::string to_hex(unsigned char byte);

    static const std::string q_hex;
    static const std::string p_hex;
    static const std::string Gx_hex;
    static const std::string Gy_hex;

    static const unsigned char Sbox[256];
    static const unsigned char tau[64];
    static const uint64_t A[64];
};

#endif // GOST3410_H