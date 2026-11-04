#pragma once

#include <QString>
#include <QByteArray>

#include "SecurityConfig.h"

class PasswordHasher
{
public:
    struct HashResult {
        QByteArray hash;
        QByteArray salt;
    };

    static HashResult hashPassword(const QString& password);

    static bool verifyPassword(
        const QString& password,
        const QByteArray& storedHash,
        const QByteArray& storedSalt
    );

};
