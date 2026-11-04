#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

#include "DatabaseManager.h"

// constants
static QString APP_DB_NAME = "password_manager";
static QString DB_HOST = "localhost";
static QString DB_USER = "postgres";
static QString DB_PASS = "root";
static int     DB_PORT = 5432;

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

QSqlDatabase DatabaseManager::database() const
{
    return QSqlDatabase::database("app_db");
}

bool DatabaseManager::connectToSystemDb() 
{
    if (QSqlDatabase::contains("system_db")) {
        m_db = QSqlDatabase::database("system_db");
    }
    else {
        m_db = QSqlDatabase::addDatabase("QPSQL", "system_db");
    }

    m_db.setHostName(DB_HOST);
    m_db.setPort(DB_PORT);
    m_db.setDatabaseName("postgres");
    m_db.setUserName(DB_USER);
    m_db.setPassword(DB_PASS);

    if (!m_db.open()) {
        qCritical() << "Database connection failed : "
            << m_db.lastError().text();
        return false;
    }

    qDebug() << "PostgreSQL connected successfully";
    
    return true;
}

bool DatabaseManager::ensureApplicationDbExists()
{
    QSqlQuery query(m_db);

    query.prepare(R"(
        SELECT 1
        FROM pg_database
        WHERE datname = :dbname
    )");
    query.bindValue(":dbname", APP_DB_NAME);

    if(!query.exec()) {
        qDebug() << "app db check query failed : "
            << query.lastError().text();
        return false;
    }

    if(query.next()) {
        return true;
    }

    // CREATE DATABASE can't be server-side prepared (QPSQL wraps prepare() in
    // PREPARE ... AS), so run it as a plain statement.
    const QString createDbQuery = QString("CREATE DATABASE %1").arg(APP_DB_NAME);

    if(!query.exec(createDbQuery)) {
        qDebug() << "database creation failed : "
            << query.lastError().text();
        return false;
    }

    qDebug() << "Application Database created successfully :" << APP_DB_NAME;
    return true;
}

bool DatabaseManager::connectToApplicationDb()
{
    // close connection made to the system db
    m_db.close();
    QSqlDatabase::removeDatabase("system_db");

    m_db = QSqlDatabase::addDatabase("QPSQL", "app_db");
    m_db.setHostName(DB_HOST);
    m_db.setDatabaseName(APP_DB_NAME);
    m_db.setUserName(DB_USER);
    m_db.setPassword(DB_PASS);
    m_db.setPort(DB_PORT);

    if (!m_db.open()) {
        qDebug() << "failed to connect to app db : "
            << m_db.lastError().text();
        return false;
    }
    qDebug() << "Connected to the application db Successfully";
    return true;
}

bool DatabaseManager::createTablesIfNotExists() 
{
    QSqlQuery query(m_db);

    const QString createUserTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id SERIAL PRIMARY KEY,
            username TEXT NOT NULL UNIQUE,
            password_hash BYTEA NOT NULL,
            password_salt BYTEA NOT NULL,
            encryption_salt BYTEA NOT NULL,
            created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
        )
    )";

    // create users table
    if(!query.exec(createUserTable)) {
        qDebug() << "error while creating users table :"
            << query.lastError().text();
        return false;
    }

    const QString createPasswordsTable = R"(
        CREATE TABLE IF NOT EXISTS passwords (
            id SERIAL PRIMARY KEY,

            user_id INTEGER NOT NULL
                REFERENCES users(id)
                ON DELETE CASCADE,

            domain TEXT NOT NULL,
            username TEXT NOT NULL,

            encrypted_password BYTEA NOT NULL,
            iv BYTEA NOT NULL,

            require_master_reprompt BOOLEAN NOT NULL DEFAULT FALSE,

            created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
        )
    )";

    // create passwords table
    if(!query.exec(createPasswordsTable)) {
        qDebug() << "error while creating users table :"
            << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::initialize() 
{
    if (!connectToSystemDb()) {
        qDebug() << "failed to connect to system db";
        return false;
    }

    if (!ensureApplicationDbExists()) {
        qDebug() << "error while creating/checking application db existence";
        return false;
    }

    if (!connectToApplicationDb()) {
        qDebug() << "error while connecting to the application db";
        return false;
    }

    if (!createTablesIfNotExists()) {
        qDebug() << "error while creating tables";
        return false;
    }

    return true;
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}
