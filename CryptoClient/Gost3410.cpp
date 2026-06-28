/*
#include "Gost3410.h"

#include <random>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/bn.h>



const std::string Gost3410::q_hex = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD97";
const std::string Gost3410::p_hex = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD97";
const std::string Gost3410::Gx_hex = "0000000000000000000000000000000000000000000000000000000000000001";
const std::string Gost3410::Gy_hex = "8D91E471E0989CDA27DF505A51F3B0F064F3E6A5E892E9E2E3D8D08F1A94D8A2";

const unsigned char Gost3410::Sbox[256] = {
    0xFC,0xEE,0xDD,0x11,0xCF,0x6E,0x31,0x16,0xFB,0xC4,0xFA,0xDA,0x23,0xC5,0x04,0x4D,
    0xE9,0x77,0xF0,0xDB,0x93,0x2E,0x99,0xBA,0x17,0x36,0xF1,0xBB,0x14,0xCD,0x5F,0xC1,
    0xF9,0x18,0x65,0x5A,0xE2,0x5C,0xEF,0x21,0x81,0x1C,0x3C,0x42,0x8B,0x01,0x8E,0x4F,
    0x05,0x84,0x02,0xAE,0xE3,0x6A,0x8F,0xA0,0x06,0x0B,0xED,0x98,0x7F,0xD4,0xD3,0x1F,
    0xEB,0x34,0x2C,0x51,0xEA,0xC8,0x48,0xAB,0xF2,0x2A,0x68,0xA2,0xFD,0x3A,0xCE,0xCC,
    0xB5,0x70,0x0E,0x56,0x08,0x0C,0x76,0x12,0xBF,0x72,0x13,0x47,0x9C,0xB7,0x5D,0x87,
    0x15,0xA1,0x96,0x29,0x10,0x7B,0x9A,0xC7,0xF3,0x91,0x78,0x6F,0x9D,0x9E,0xB2,0xB1,
    0x32,0x75,0x19,0x3D,0xFF,0x35,0x8A,0x7E,0x6D,0x54,0xC6,0x80,0xC3,0xBD,0x0D,0x57,
    0xDF,0xF5,0x24,0xA9,0x3E,0xA8,0x43,0xC9,0xD7,0x79,0xD6,0xF6,0x7C,0x22,0xB9,0x03,
    0xE0,0x0F,0xEC,0xDE,0x7A,0x94,0xB0,0xBC,0xDC,0xE8,0x28,0x50,0x4E,0x33,0x0A,0x4A,
    0xA7,0x97,0x60,0x73,0x1E,0x00,0x62,0x44,0x1A,0xB8,0x38,0x82,0x64,0x9F,0x26,0x41,
    0xAD,0x45,0x46,0x92,0x27,0x5E,0x55,0x2F,0x8C,0xA3,0xA5,0x7D,0x69,0xD5,0x95,0x3B,
    0x07,0x58,0xB3,0x40,0x86,0xAC,0x1D,0xF7,0x30,0x37,0x6B,0xE4,0x88,0xD9,0xE7,0x89,
    0xE1,0x1B,0x83,0x49,0x4C,0x3F,0xF8,0xFE,0x8D,0x53,0xAA,0x90,0xCA,0xD8,0x85,0x61,
    0x20,0x71,0x67,0xA4,0x2D,0x2B,0x09,0x5B,0xCB,0x9B,0x25,0xD0,0xBE,0xE5,0x6C,0x52,
    0x59,0xA6,0x74,0xD2,0xE6,0xF4,0xB4,0xC0,0xD1,0x66,0xAF,0xC2,0x39,0x4B,0x63,0xB6
};

const unsigned char Gost3410::tau[64] = {
    0, 8, 16, 24, 32, 40, 48, 56,
    1, 9, 17, 25, 33, 41, 49, 57,
    2, 10, 18, 26, 34, 42, 50, 58,
    3, 11, 19, 27, 35, 43, 51, 59,
    4, 12, 20, 28, 36, 44, 52, 60,
    5, 13, 21, 29, 37, 45, 53, 61,
    6, 14, 22, 30, 38, 46, 54, 62,
    7, 15, 23, 31, 39, 47, 55, 63
};

const uint64_t Gost3410::A[64] = {
    0x8e20faa72ba0b470, 0x47107ddd9b505a38, 0xad08b0e0c3282d1c, 0xd8045870ef14980e,
    0x6c022c38f90a4c07, 0x3601161cf2052680, 0x1b8e0b0e798c13c8, 0x83478b07b2468764,
    0xda011c80f3b192ae, 0x6d00ce80de0a882e, 0x368046c059429a96, 0x1b4023a02e2d324c,
    0x0da010c0c0241926, 0x06d008603014258a, 0x036804401c044682, 0x01b202202209804c,
    0x8d401496b45c2ce0, 0x46a00a02f2e43c70, 0x23500280e2847c38, 0x11a80160d2807a1c,
    0x08d400b030940e0e, 0x046a003018cc0a07, 0x023500300cc60283, 0x011a80028cc20227,
    0x8c8c04886c06c09a, 0x464602286c06404c, 0x232301146c032026, 0x1191808a6c018092,
    0x08c8c04478018048, 0x046460223c010024, 0x023230111e008012, 0x011918088f004009,
    0x8d0c042093460958, 0x46860208a20c0a2c, 0x23430104a10c0696, 0x11a18084518c0a4a,
    0x08d0c042318606a4, 0x04686021318403d2, 0x02343010a18c01e9, 0x011a1808518c00f0,
    0x8d8c0488a08c0298, 0x46c60208a20c014c, 0x23630104a10c00a6, 0x11b18084518c0052,
    0x08d8c042318c0028, 0x046c6021a18c0014, 0x02363010a10c000a, 0x011b1808518c0004,
    0x8e1c0488230801b0, 0x470e0208a20800d8, 0x23870104a108006c, 0x11c3808451840036,
    0x08e1c0423184001a, 0x0470e021a184000c, 0x02387010a1080006, 0x011c380851840002,
    0x8e0e048821080098, 0x47070208a208004c, 0x23838104a1080026, 0x11c1c08451840012,
    0x08e0e04221840008, 0x04707021a1840004, 0x02383810a1080002, 0x011c1c0851840001
};

// ============================================================================
// КОНСТРУКТОР / ДЕСТРУКТОР
// ============================================================================

Gost3410::Gost3410(QObject *parent) : QObject(parent)
{
}

Gost3410::~Gost3410()
{
}



QString Gost3410::generatePrivateKey()
{
    std::string private_key;
    bool valid = false;

    while (!valid) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dist(0, 0xffffffffffffffffULL);

        // 256 бит = 4 части по 64 бита
        uint64_t parts[4];
        for(int i = 0; i < 4; ++i) {
            parts[i] = dist(gen);
        }

        // Преобразование в hex-строку
        private_key = "";
        for(int i = 0; i < 4; ++i) {
            char buf[17];
            snprintf(buf, sizeof(buf), "%016llx", (unsigned long long)parts[i]);
            private_key += buf;
        }

        // Проверка: key < q
        bool less = false;
        for(int i = 0; i < 64; ++i) {
            if(private_key[i] < q_hex[i]) { less = true; break; }
            if(private_key[i] > q_hex[i]) { less = false; break; }
        }

        bool is_zero = (private_key == "0000000000000000000000000000000000000000000000000000000000000000");

        valid = less && !is_zero;
    }

    return QString::fromStdString(private_key);
}



QString Gost3410::generatePublicKey(const QString &privateKeyHex)
{
    std::string private_key_hex = privateKeyHex.toStdString();

    // 1. Создать контекст кривой
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_id_GostR3410_2012_256);
    const EC_GROUP* group = EC_KEY_get0_group(ec_key);

    // 2. Загрузить приватный ключ
    BIGNUM* d = BN_new();
    BN_hex2bn(&d, private_key_hex.c_str());

    // 3. Вычислить публичный ключ Q = d * G
    EC_POINT* Q = EC_POINT_new(group);
    EC_POINT_mul(group, Q, d, NULL, NULL, NULL);

    // 4. Получить координаты Qx и Qy
    BIGNUM* Qx = BN_new();
    BIGNUM* Qy = BN_new();
    EC_POINT_get_affine_coordinates(group, Q, Qx, Qy, NULL);

    // 5. Преобразовать в hex
    char* Qx_hex = BN_bn2hex(Qx);
    char* Qy_hex = BN_bn2hex(Qy);

    std::string x_hex = Qx_hex;
    std::string y_hex = Qy_hex;

    // Дополнить нулями до 64 символов
    while(x_hex.size() < 64) x_hex = "0" + x_hex;
    while(y_hex.size() < 64) y_hex = "0" + y_hex;

    std::string public_key = x_hex + y_hex;

    // 6. Очистка
    OPENSSL_free(Qx_hex);
    OPENSSL_free(Qy_hex);
    BN_free(Qx);
    BN_free(Qy);
    BN_free(d);
    EC_POINT_free(Q);
    EC_KEY_free(ec_key);

    return QString::fromStdString(public_key);
}


QString Gost3410::computeHash(const QString &data)
{
    std::string M = data.toStdString();

    // Инициализация
    std::vector<unsigned char> h(64, 0);
    std::vector<unsigned char> N(64, 0);
    std::vector<unsigned char> sigma(64, 0);

    // Дополнение сообщения
    std::vector<unsigned char> vec_mes(M.begin(), M.end());
    vec_mes.push_back(0x80);

    size_t Q = (64 - ((vec_mes.size() + 16) % 64)) % 64;
    for(size_t i = 0; i < Q; ++i) {
        vec_mes.push_back(0x0);
    }

    // Длина сообщения в битах (128-битное число)
    uint64_t size = M.size();
    uint64_t lo = size << 3;
    uint64_t hi = size >> 61;

    for(int i = 0; i < 8; ++i) {
        vec_mes.push_back((lo >> (i * 8)) & 0xFF);
    }
    for(int i = 0; i < 8; ++i) {
        vec_mes.push_back((hi >> (i * 8)) & 0xFF);
    }

    // Обработка блоков
    for(size_t i = 0; i < vec_mes.size(); i += 64) {
        std::vector<unsigned char> block(vec_mes.begin() + i, vec_mes.begin() + i + 64);
        h = g_N(h, block, N);
        add_512(N);
        add_block(sigma, block);
    }

    // Финал
    std::vector<unsigned char> zero(64, 0);
    h = g_N(h, N, zero);
    h = g_N(h, sigma, zero);

    // Возврат 32 первых байт в hex
    std::string result;
    for(int i = 0; i < 32; ++i) {
        result += to_hex(h[i]);
    }

    return QString::fromStdString(result);
}

QString Gost3410::createSignature(const QString &hashHex, const QString &privateKeyHex)
{
    std::string hash_hex = hashHex.toStdString();
    std::string private_key_hex = privateKeyHex.toStdString();

    // 1. Создать контекст кривой
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_id_GostR3410_2012_256);
    const EC_GROUP* group = EC_KEY_get0_group(ec_key);
    BIGNUM* q = BN_new();
    EC_GROUP_get_order(group, q, NULL);

    // 2. Загрузить числа
    BIGNUM* hash = BN_new();
    BIGNUM* d = BN_new();
    BN_hex2bn(&hash, hash_hex.c_str());
    BN_hex2bn(&d, private_key_hex.c_str());

    // 3. Сгенерировать случайное k
    BIGNUM* k = BN_new();
    BN_rand_range(k, q);
    if(BN_is_zero(k)) BN_one(k);

    // 4. C = k * G
    EC_POINT* C = EC_POINT_new(group);
    EC_POINT_mul(group, C, k, NULL, NULL, NULL);

    // 5. r = C.x mod q
    BIGNUM* r = BN_new();
    BIGNUM* Cx = BN_new();
    EC_POINT_get_affine_coordinates(group, C, Cx, NULL, NULL);
    BN_mod(r, Cx, q, NULL);

    // 6. s = (k * hash + d * r) mod q
    BIGNUM* s = BN_new();
    BIGNUM* temp1 = BN_new();
    BIGNUM* temp2 = BN_new();
    BN_CTX* ctx = BN_CTX_new();

    BN_mod_mul(temp1, k, hash, q, ctx);
    BN_mod_mul(temp2, d, r, q, ctx);
    BN_mod_add(s, temp1, temp2, q, ctx);

    // 7. Преобразовать в hex
    char* r_hex = BN_bn2hex(r);
    char* s_hex = BN_bn2hex(s);

    std::string r_str = r_hex;
    std::string s_str = s_hex;

    while(r_str.size() < 64) r_str = "0" + r_str;
    while(s_str.size() < 64) s_str = "0" + s_str;

    std::string signature = r_str + s_str;

    // 8. Очистка
    OPENSSL_free(r_hex);
    OPENSSL_free(s_hex);
    BN_free(hash);
    BN_free(d);
    BN_free(k);
    BN_free(r);
    BN_free(s);
    BN_free(Cx);
    BN_free(temp1);
    BN_free(temp2);
    BN_free(q);
    BN_CTX_free(ctx);
    EC_POINT_free(C);
    EC_KEY_free(ec_key);

    return QString::fromStdString(signature);
}


bool Gost3410::verifySignature(const QString &hashHex,
                               const QString &signatureHex,
                               const QString &publicKeyHex)
{
    std::string hash_hex = hashHex.toStdString();
    std::string signature = signatureHex.toStdString();
    std::string public_key_hex = publicKeyHex.toStdString();

    if(signature.length() != 128) {
        return false;
    }

    std::string r_hex = signature.substr(0, 64);
    std::string s_hex = signature.substr(64, 64);

    // 1. Создать контекст кривой
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_id_GostR3410_2012_256);
    const EC_GROUP* group = EC_KEY_get0_group(ec_key);
    BIGNUM* q = BN_new();
    EC_GROUP_get_order(group, q, NULL);

    // 2. Загрузить r и s
    BIGNUM* r = BN_new();
    BIGNUM* s = BN_new();
    BN_hex2bn(&r, r_hex.c_str());
    BN_hex2bn(&s, s_hex.c_str());

    // 3. Проверить: 0 < r < q и 0 < s < q
    if(BN_is_zero(r) || BN_is_negative(r) || BN_cmp(r, q) >= 0) {
        BN_free(r); BN_free(s); BN_free(q); EC_KEY_free(ec_key);
        return false;
    }
    if(BN_is_zero(s) || BN_is_negative(s) || BN_cmp(s, q) >= 0) {
        BN_free(r); BN_free(s); BN_free(q); EC_KEY_free(ec_key);
        return false;
    }

    // 4. Разобрать публичный ключ
    std::string Qx_hex = public_key_hex.substr(0, 64);
    std::string Qy_hex = public_key_hex.substr(64, 64);

    // 5. Создать точку Q
    BIGNUM* Qx = BN_new();
    BIGNUM* Qy = BN_new();
    BN_hex2bn(&Qx, Qx_hex.c_str());
    BN_hex2bn(&Qy, Qy_hex.c_str());

    EC_POINT* Q = EC_POINT_new(group);
    EC_POINT_set_affine_coordinates(group, Q, Qx, Qy, NULL);

    // 6. Загрузить hash
    BIGNUM* hash = BN_new();
    BN_hex2bn(&hash, hash_hex.c_str());

    // 7. v = s^(-1) mod q
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* v = BN_new();
    BN_mod_inverse(v, s, q, ctx);

    // 8. u1 = (hash * v) mod q
    BIGNUM* u1 = BN_new();
    BN_mod_mul(u1, hash, v, q, ctx);

    // 9. u2 = (r * v) mod q
    BIGNUM* u2 = BN_new();
    BN_mod_mul(u2, r, v, q, ctx);

    // 10. C = u1 * G + u2 * Q
    EC_POINT* C = EC_POINT_new(group);
    EC_POINT_mul(group, C, u1, Q, u2, ctx);

    // 11. Проверить: C.x mod q == r
    BIGNUM* Cx = BN_new();
    EC_POINT_get_affine_coordinates(group, C, Cx, NULL, NULL);
    BIGNUM* Cx_mod_q = BN_new();
    BN_mod(Cx_mod_q, Cx, q, ctx);
    int result = (BN_cmp(Cx_mod_q, r) == 0);

    // 12. Очистка
    BN_free(r); BN_free(s); BN_free(q);
    BN_free(Qx); BN_free(Qy);
    BN_free(hash); BN_free(v);
    BN_free(u1); BN_free(u2);
    BN_free(Cx); BN_free(Cx_mod_q);
    BN_CTX_free(ctx);
    EC_POINT_free(Q);
    EC_POINT_free(C);
    EC_KEY_free(ec_key);

    return result == 1;
}



std::vector<unsigned char> Gost3410::g_N(const std::vector<unsigned char> &h,
                                         const std::vector<unsigned char> &m,
                                         const std::vector<unsigned char> &N)
{
    auto K = LPS(xor_64(h, N));
    auto result = LPS(xor_64(K, m));
    result = xor_64(result, m);
    return result;
}

std::vector<unsigned char> Gost3410::xor_64(std::vector<unsigned char> a,
                                            std::vector<unsigned char> b)
{
    std::vector<unsigned char> result(64);
    for(int i = 0; i < 64; ++i) {
        result[i] = a[i] ^ b[i];
    }
    return result;
}

std::vector<unsigned char> Gost3410::LPS(std::vector<unsigned char> data)
{
    return L_transform(P_transform(S_transform(data)));
}

std::vector<unsigned char> Gost3410::S_transform(std::vector<unsigned char> data)
{
    std::vector<unsigned char> result(64);
    for(int i = 0; i < 64; ++i) {
        result[i] = Sbox[data[i]];
    }
    return result;
}

std::vector<unsigned char> Gost3410::P_transform(std::vector<unsigned char> data)
{
    std::vector<unsigned char> result(64);
    for(int i = 0; i < 64; ++i) {
        result[i] = data[tau[i]];
    }
    return result;
}

std::vector<unsigned char> Gost3410::L_transform(std::vector<unsigned char> data)
{
    uint64_t data8byte[8];
    for(int i = 0; i < 8; ++i) {
        data8byte[i] = 0;
        for(int j = 0; j < 8; ++j) {
            data8byte[i] |= ((uint64_t)data[i * 8 + j]) << (j * 8);
        }
    }

    uint64_t result[8] = {0};
    for(int i = 0; i < 8; ++i) {
        uint64_t val = data8byte[i];
        for(int j = 0; j < 64; ++j) {
            if(val & 1) {
                for(int k = 0; k < 8; ++k) {
                    result[k] ^= A[j * 8 + k];
                }
            }
            val >>= 1;
        }
    }

    std::vector<unsigned char> output(64);
    for(int i = 0; i < 8; ++i) {
        for(int j = 0; j < 8; ++j) {
            output[i * 8 + j] = (result[i] >> (j * 8)) & 0xff;
        }
    }
    return output;
}

void Gost3410::add_512(std::vector<unsigned char> &N)
{
    uint16_t carry = 512;
    for(int i = 0; i < 64 && carry > 0; ++i) {
        uint16_t sum = N[i] + (carry & 0xFF);
        N[i] = sum & 0xFF;
        carry = (carry >> 8) + (sum >> 8);
    }
}

void Gost3410::add_block(std::vector<unsigned char> &sigma,
                         const std::vector<unsigned char> &block)
{
    uint16_t carry = 0;
    for(int i = 0; i < 64; ++i) {
        uint16_t sum = sigma[i] + block[i] + carry;
        sigma[i] = sum & 0xFF;
        carry = sum >> 8;
    }
}

std::string Gost3410::to_hex(unsigned char byte)
{
    const char hex_chars[] = "0123456789abcdef";
    std::string s;
    s += hex_chars[byte >> 4];
    s += hex_chars[byte & 0x0F];
    return s;
}
*/
