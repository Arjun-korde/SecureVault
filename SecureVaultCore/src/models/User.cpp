#include "User.h"

User::User(int id, const QString& username)
    : m_id(id), m_username(username)
{

}

int User::id() const {
    return m_id;
}

QString User::username() const {
    return m_username;
}

void User::setId(int id) {
    m_id = id;
}

void User::setUsername(const QString& username) {
    m_username = username;
}
