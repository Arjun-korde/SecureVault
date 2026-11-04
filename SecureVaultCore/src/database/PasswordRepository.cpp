#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include "PasswordRepository.h"
#include "security/CryptoManager.h"
#include "app/UserSession.h"

bool PasswordRepository::addPassword(int userId, const PasswordEntry& entry)
{

    QByteArray key = UserSession::instance().encryptionKey();
    CryptoManager::EncryptedData encrypted =
        CryptoManager::encrypt(entry.password().toUtf8(), key);

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO passwords 
        (user_id, domain, username, encrypted_password, iv, require_master_reprompt)
        VALUES
        (:user_id, :domain, :username, :encrypted_password, :iv,:isRequiresMasterPassword)
    )");

    query.bindValue(":user_id", userId);
    query.bindValue(":domain", entry.domain());
    query.bindValue(":username", entry.username());
    query.bindValue(":encrypted_password", encrypted.cipherText);
    query.bindValue(":iv", encrypted.iv);
    query.bindValue(":isRequiresMasterPassword", entry.requiresMasterPassword());

    if (!query.exec()) {
        qDebug() << "error occured while adding password " << query.lastError();
        return false;
    }

    return true;
}

std::optional<PasswordEntry> PasswordRepository::fetchById(int passwordId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT 
        id, domain, username, encrypted_password, iv, require_master_reprompt
        FROM passwords WHERE id = :passwordId
    )");
    query.bindValue(":passwordId", passwordId);

    if (!query.exec() || !query.next()) {
        qDebug() << "error occured while fetching the password : " << query.lastError();
        return std::nullopt;
    }

    QByteArray key = UserSession::instance().encryptionKey();

    QByteArray decrypted =
        CryptoManager::decrypt(
            query.value("encrypted_password").toByteArray(),
            query.value("iv").toByteArray(),
            key
        );

    return PasswordEntry(
        query.value("id").toInt(),
        query.value("domain").toString(),
        query.value("username").toString(),
        QString::fromUtf8(decrypted),
        query.value("require_master_reprompt").toBool()
    );
}

bool PasswordRepository::deletePasswordById(int passwordId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM passwords WHERE id = :passwordId");
    query.bindValue(":passwordId", passwordId);

    if (!query.exec()) {
        qDebug() <<"error occured while deleting the password : " << query.lastError();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool PasswordRepository::updatePassword(int userId, const PasswordEntry& entry)
{

    QByteArray key = UserSession::instance().encryptionKey();
    CryptoManager::EncryptedData encrypted =
        CryptoManager::encrypt(entry.password().toUtf8(), key);

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        UPDATE passwords
        SET
            domain = :domain,
            username = :username,
            encrypted_password = :encrypted_password,
            iv = :iv,
            require_master_reprompt= :isRequiresMasterPassword
            updated_at = CURRENT_TIMESTAMP
        WHERE
            id = :id
            AND user_id = :user_id
    )");

    query.bindValue(":domain", entry.domain());
    query.bindValue(":username", entry.username());
    query.bindValue(":encrypted_password", encrypted.cipherText);
    query.bindValue(":iv", encrypted.iv);
    query.bindValue(":id", entry.id());
    query.bindValue(":user_id", userId);
    query.bindValue(":isRequiresMasterPassword", entry.requiresMasterPassword());

    if (!query.exec()) {
        qDebug() << "update password failed : "
            << query.lastError().text();
        return false;
    }

    
    if (query.numRowsAffected() != 1) {
        qDebug() << "while updating password more than 1 rows affedcted : "
            << query.numRowsAffected()
            << "rows (expected 1)";
        return false;
    }

    return true;
}

std::optional<PasswordEntry> PasswordRepository::findByDomain(int userId, const QString& domain)
{
    QSqlQuery query(DatabaseManager::instance().database());

    query.prepare(R"(
        SELECT id, domain, username, encrypted_password, iv
        FROM passwords
        WHERE user_id = :user_id
          AND domain = :domain
        ORDER BY created_at DESC
        LIMIT 1
    )");

    query.bindValue(":user_id", userId);
    query.bindValue(":domain", domain);

    if(!query.exec() || !query.next()) {
        return std::nullopt;
    }

    QByteArray key = UserSession::instance().encryptionKey();

    QByteArray decrypted = CryptoManager::decrypt(
        query.value("encrypted_password").toByteArray(),
        query.value("iv").toByteArray(),
        key
    );

    return PasswordEntry(
        query.value("id").toInt(),
        query.value("domain").toString(),
        query.value("username").toString(),
        QString::fromUtf8(decrypted)
    );
}

QVector<PasswordEntry> PasswordRepository::fetchPasswords(int userId)
{
    return executeQuery(userId, "", {});
}

QVector<PasswordEntry> PasswordRepository::searchPasswords(int userId, const QString& searchText)
{
    const QString pattern = "%" + searchText + "%";

    return executeQuery(
        userId,
        "(domain ILIKE :pattern OR username ILIKE :pattern)",
        { {":pattern", pattern} }
    );
}

QVector<PasswordEntry> PasswordRepository::executeQuery(
    int userId,
    const QString& whereClause,
    const QMap<QString, QVariant>& bindings)
{
    QVector<PasswordEntry> result;

    QString sql = R"(
        SELECT id, domain, username, encrypted_password, iv
        FROM passwords
        WHERE user_id = :user_id
    )";

    if (!whereClause.isEmpty()) {
        sql += " AND " + whereClause;
    }

    sql += " ORDER BY created_at DESC";

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(sql);

    query.bindValue(":user_id", userId);

    for (auto it = bindings.begin(); it != bindings.end(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
        qDebug() << "password query failed:"
            << query.lastError().text();
        return result;
    }

    QByteArray key = UserSession::instance().encryptionKey();

    while (query.next()) {
        QByteArray decrypted =
            CryptoManager::decrypt(
                query.value("encrypted_password").toByteArray(),
                query.value("iv").toByteArray(),
                key
            );

        result.append(
            PasswordEntry(
                query.value("id").toInt(),
                query.value("domain").toString(),
                query.value("username").toString(),
                QString::fromUtf8(decrypted)
            )
        );
    }

    return result;
}
