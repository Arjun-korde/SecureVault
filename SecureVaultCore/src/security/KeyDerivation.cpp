#include "KeyDerivation.h"
#include "SecurityConfig.h"
#include "CryptoUtils.h"

#include <QCryptographicHash>
#include <QRandomGenerator>

QByteArray KeyDerivation::generateEncryptionSalt()
{
    return CryptoUtils::generateRandomBytes(SecurityConfig::ENCRYPTION_SALT_SIZE_BYTES);
}

QByteArray KeyDerivation::deriveEncryptionKey(
    const QString& masterPassword,
    const QByteArray& encryptionSalt
)
{
    if (masterPassword.isEmpty() || encryptionSalt.isEmpty()) {
        return {};
    }

    QByteArray passwordBytes = masterPassword.toUtf8();

    QByteArray derivedKey = CryptoUtils::pbkdf2Sha256(
        passwordBytes,
        encryptionSalt,
        SecurityConfig::KEY_DERIVATION_ITERATIONS,
        SecurityConfig::AES_KEY_SIZE_BYTES
    );

    return derivedKey;
}
