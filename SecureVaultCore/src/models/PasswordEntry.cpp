#include "PasswordEntry.h"

PasswordEntry::PasswordEntry(int id,
    const QString& domain,
    const QString& username,
    const QString& password,
    const bool isMasterPasswordRequired)
    : m_id(id),
    m_domain(domain),
    m_username(username),
    m_password(password),
    m_requiresMasterPassword(isMasterPasswordRequired)
{
}

int PasswordEntry::id() const { return m_id; }
QString PasswordEntry::domain() const { return m_domain; }
QString PasswordEntry::username() const { return m_username; }
QString PasswordEntry::password() const { return m_password; }
bool PasswordEntry::requiresMasterPassword() const { return m_requiresMasterPassword; }

void PasswordEntry::setId(int id) { m_id = id; }
void PasswordEntry::setDomain(const QString& domain) { m_domain = domain; }
void PasswordEntry::setUsername(const QString& username) { m_username = username; }
void PasswordEntry::setPassword(const QString& password) { m_password = password; }
void PasswordEntry::setRequiresMasterPassword(const bool isMasterPasswordRequired) { m_requiresMasterPassword = isMasterPasswordRequired; }
