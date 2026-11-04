#include "UserRepository.h"
#include "security/KeyDerivation.h"
#include "app/UserSession.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

std::optional<User> UserRepository::login(const QString& username, const QString& password)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT id, username, password_hash, password_salt, encryption_salt FROM users
        WHERE username = :username
    )");
    query.bindValue(":username", normalizeUsername(username));

    if (!query.exec()) {
        qDebug() << "error occured while login : " << query.lastError();
        return std::nullopt;
    }

    if (!query.next()) {
        qDebug() << "username not found";
        return std::nullopt;
    }

    const QByteArray storedHash = query.value("password_hash").toByteArray();
    const QByteArray storedSalt = query.value("password_salt").toByteArray();

    const bool isValidPassword = PasswordHasher::verifyPassword(
        password, storedHash, storedSalt
    );

    if (!isValidPassword) {
        return std::nullopt;
    }

    const QByteArray encryptionSalt =
        query.value("encryption_salt").toByteArray();

    QByteArray encryptionKey =
        KeyDerivation::deriveEncryptionKey(password, encryptionSalt);
    qDebug() << "Key : " << encryptionKey;
    UserSession::instance().setEncryptionKey(encryptionKey);

    return User(
        query.value("id").toInt(),
        query.value("username").toString()
    );
}

bool UserRepository::createUser(const QString& username, const QString& password)
{
    auto hashResult = PasswordHasher::hashPassword(password);
    QByteArray encryptionSalt = KeyDerivation::generateEncryptionSalt();

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        INSERT INTO users (
            username,
            password_hash,
            password_salt,
            encryption_salt
        )
        VALUES (
            :username,
            :password_hash,
            :password_salt,
            :encryption_salt
        )
    )");

    query.bindValue(":username", normalizeUsername(username));
    query.bindValue(":password_hash", hashResult.hash);
    query.bindValue(":password_salt", hashResult.salt);
    query.bindValue(":encryption_salt", encryptionSalt);

    if (!query.exec()) {
        qDebug() << "error occured while creating User" << query.lastError();
        return false;
    }

    return true;
}

bool UserRepository::deleteAccount(int userId, const QString& password)
{
    QSqlQuery fetchQuery(DatabaseManager::instance().database());
    fetchQuery.prepare(R"(
        SELECT password_hash, password_salt
        FROM users
        WHERE id = :userId
    )");
    fetchQuery.bindValue(":userId", userId);

    if (!fetchQuery.exec() || !fetchQuery.next()) {
        qDebug() << "failed to fetch user for deletion";
        return false;
    }

    const QByteArray storedHash = fetchQuery.value("password_hash").toByteArray();
    const QByteArray storedSalt = fetchQuery.value("password_salt").toByteArray();

    if (!PasswordHasher::verifyPassword(password, storedHash, storedSalt)) {
        qDebug() << "password verification failed for deleteAccount";
        return false;
    }

    QSqlQuery deleteQuery(DatabaseManager::instance().database());
    deleteQuery.prepare("DELETE FROM users WHERE id = :userId");
    deleteQuery.bindValue(":userId", userId);

    if (!deleteQuery.exec()) {
        qDebug() << "error deleting user:" << deleteQuery.lastError();
        return false;
    }

    return deleteQuery.numRowsAffected() == 1;
}


QString UserRepository::normalizeUsername(const QString& username) {
    return username.trimmed().toLower();
}

bool UserRepository::verifyPassword(const QString& masterPasswordInput)
{
    auto user = UserSession::instance().currentUser();

    if (!user.has_value()) {
        return false;
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(R"(
        SELECT password_hash, password_salt
        FROM users
        WHERE id = :userId
    )");
    query.bindValue(":userId", user->id());

    if (!query.exec() || !query.next()) {
        qDebug() << "verifyPassword: failed to fetch user";
        return false;
    }

    const QByteArray storedHash =
        query.value("password_hash").toByteArray();
    const QByteArray storedSalt =
        query.value("password_salt").toByteArray();

    return PasswordHasher::verifyPassword(
        masterPasswordInput,
        storedHash,
        storedSalt
    );
}
