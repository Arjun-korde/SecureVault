#pragma once

#include <QByteArray>
#include <QString>
#include <cstdint>

namespace CryptoUtils {

    QByteArray generateRandomBytes(size_t size);

    QByteArray pbkdf2Sha256(
        const QByteArray& password,
        const QByteArray& salt,
        uint32_t iterations,
        size_t outputLen
    );

}
