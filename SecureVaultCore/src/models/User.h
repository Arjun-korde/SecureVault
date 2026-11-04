#pragma once
#include <QString>

class User
{
public:
    User() = default;
    User(int id, const QString& username);

    int id() const;
    QString username() const;

    void setId(int id);
    void setUsername(const QString& username);

private:
    int m_id;
    QString m_username;
};
