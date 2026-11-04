#pragma once

#include <optional>
#include "models/user.h"

class UserSession
{
public:
    static UserSession& instance();

    void setCurrentUser(const User& user);
    std::optional<User> currentUser() const;
    void setEncryptionKey(const QByteArray& key);
    QByteArray encryptionKey() const;
    void reset();

private:
    std::optional<User> m_currentUser;
    QByteArray m_encryptionKey;
};
