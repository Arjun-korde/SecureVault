#pragma once

#include <QSqlDatabase>

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    QSqlDatabase database() const;
    bool initialize();

private:
    QSqlDatabase m_db;

    ~DatabaseManager();

    bool connectToSystemDb();
    bool ensureApplicationDbExists();
    bool connectToApplicationDb();
    bool createTablesIfNotExists();
};
