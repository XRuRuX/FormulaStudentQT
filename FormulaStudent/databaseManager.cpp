#include "databaseManager.h"

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager()
{
    db.close();
}

// Connects to a database where the path is sent as a parameter
bool DatabaseManager::connect(const QString& path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
        qDebug() << "Error: connection with database fail";
        return false;
    }

    QSqlQuery query(db);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Settings'");
    if (!query.next()) {
        qDebug() << "Table 'Settings' does not exist.";
        return false;
    }

    return true;
}

// Extracts data from the database
QSqlQuery DatabaseManager::queryData()
{
    QSqlQuery query;

    query.exec("SELECT GPSLat, GPSLong FROM Settings");

    return query;
}

bool DatabaseManager::updateSetting(const QString& columnName, int value)
{
    if (!db.isOpen())
    {
        qDebug() << "Database is not open.";
        return false;
    }

    // Safeguard against SQL injection by ensuring the column name is valid
    QStringList validColumnNames = {"GPSLat", "GPSLong"};
    if (!validColumnNames.contains(columnName)) {
        qDebug() << "Invalid column name.";
        return false;
    }

    QSqlQuery query(db);
    query.clear(); // Clear any previous bindings and states

    // Construct the query string with the column name
    QString queryString = QString("UPDATE Settings SET %1 = :value").arg(columnName);

    query.prepare(queryString);
    query.bindValue(":value", value);

    if (!query.exec())
    {
        qDebug() << "Update failed:" << query.lastError().text();
        return false;
    }

    return true;
}



