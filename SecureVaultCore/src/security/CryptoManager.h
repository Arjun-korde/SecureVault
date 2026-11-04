#pragma once

#include <QByteArray>

class CryptoManager
{
public:
    struct EncryptedData {
        QByteArray cipherText;
        QByteArray iv;
    };

    static EncryptedData encrypt(
        const QByteArray& plainText,
        const QByteArray& key
    );

    static QByteArray decrypt(
        const QByteArray& cipherText,
        const QByteArray& iv,
        const QByteArray& key
    );

private:
    static QByteArray generateRandomBytes(int size);
};
