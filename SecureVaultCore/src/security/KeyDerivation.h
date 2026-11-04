#pragma once

#include <QString>
#include <QByteArray>

class KeyDerivation
{
public:
    
    static QByteArray generateEncryptionSalt();

    static QByteArray deriveEncryptionKey(
        const QString& masterPassword,
        const QByteArray& encryptionSalt
    );

};
