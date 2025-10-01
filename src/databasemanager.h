#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlTableModel>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // Singleton pattern to ensure only one instance manages the DB
    static DatabaseManager& instance();

    // Connection methods
    bool openDatabase(const QString& dbPath);
    void closeDatabase();

    // CRUD (Create, Read, Update, Delete) operations
    bool createTables();
    void insertRows();
    QSqlTableModel* readAkaunData(const QString& tableName);

private:
    // Private constructor for Singleton
    explicit DatabaseManager(QObject *parent = nullptr);
    DatabaseManager(const DatabaseManager&) = delete; // Delete copy constructor and assignment operator for Singleton
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db; // Database connection object
};
#endif // DATABASEMANAGER_H