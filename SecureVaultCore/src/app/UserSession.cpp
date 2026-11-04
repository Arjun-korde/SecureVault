#include<QDebug>

#include "usersession.h"

UserSession& UserSession::instance()
{
    static UserSession instance;
    return instance;
}

void UserSession::setCurrentUser(const User& user)
{
    m_currentUser = user;
    qDebug() << "Currently logged in user : id=" << user.id() << " username=" << user.username();
}

std::optional<User> UserSession::currentUser() const
{
    return m_currentUser;
}

void UserSession::reset()
{
    m_encryptionKey.fill(0);
    m_encryptionKey.clear();
    m_currentUser.reset();
}

void UserSession::setEncryptionKey(const QByteArray& key)
{
    m_encryptionKey = key;
}

QByteArray UserSession::encryptionKey() const
{
    return m_encryptionKey;
}