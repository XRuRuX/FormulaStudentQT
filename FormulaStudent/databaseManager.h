#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QString>
#include <QSqlError>

class DatabaseManager
{
private:
    QSqlDatabase db;

public:
    DatabaseManager();
    ~DatabaseManager();

public:
    bool connect(const QString& path);
    QSqlQuery queryData();
    bool updateSetting(const QString& columnName, int value);
};

#endif // DATABASEMANAGER_H
