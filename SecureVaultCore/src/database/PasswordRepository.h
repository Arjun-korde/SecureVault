#pragma once
#include <QVector>
#include "models/PasswordEntry.h"
#include "DatabaseManager.h"

class PasswordRepository
{
public:
    bool addPassword(int userId, const PasswordEntry& entry);
    bool deletePasswordById(int passwordId);
    bool updatePassword(int userId, const PasswordEntry& entry);

    QVector<PasswordEntry> fetchPasswords(int userId);
    QVector<PasswordEntry> searchPasswords(int userId, const QString& searchText);

    std::optional<PasswordEntry> fetchById(int passwordId);
    std::optional<PasswordEntry> findByDomain(int userId, const QString& domain);

private: 
    QVector<PasswordEntry> executeQuery(int userId, const QString& whereClause, const QMap<QString, QVariant>& bindings
    );
};
