#pragma once
#include <QString>
#include <optional>

#include "models/User.h"
#include "DatabaseManager.h"
#include "security/PasswordHasher.h"

class UserRepository
{
public:
    std::optional<User> login(const QString& username, const QString& password);

    bool createUser(const QString& username, const QString& password);
    bool deleteAccount(int userId, const QString& password);
    bool verifyPassword(const QString& masterPasswordInput);

private: 
    QString normalizeUsername(const QString& username);
};
