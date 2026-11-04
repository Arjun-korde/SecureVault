#include <QCryptographicHash>
#include <QRandomGenerator>

#include "PasswordHasher.h"
#include "CryptoUtils.h"


PasswordHasher:: HashResult 
PasswordHasher::hashPassword(const QString& password)
{
    HashResult result;
    if (password.isEmpty()) {
        return {};
    }

    result.salt = CryptoUtils::generateRandomBytes(SecurityConfig::PASSWORD_SALT_SIZE_BYTES);

    if (result.salt.isEmpty()) {
        return {};
    }

    QByteArray passwordBytes = password.toUtf8();

    result.hash = CryptoUtils::pbkdf2Sha256(
        passwordBytes,
        result.salt,
        SecurityConfig::PASSWORD_HASH_ITERATIONS,
        SecurityConfig::PASSWORD_HASH_SIZE_BYTES
    );

    if (result.hash.isEmpty()) {
        return {};
    }

    return result;
}

bool PasswordHasher::verifyPassword(
    const QString& password,
    const QByteArray& storedHash,
    const QByteArray& storedSalt
)
{
    QByteArray passwordBytes = password.toUtf8();

    QByteArray computedHash = CryptoUtils::pbkdf2Sha256(
        passwordBytes,
        storedSalt,
        SecurityConfig::PASSWORD_HASH_ITERATIONS,
        SecurityConfig::PASSWORD_HASH_SIZE_BYTES
    );

    return computedHash == storedHash;
}
