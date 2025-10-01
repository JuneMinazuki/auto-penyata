#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

bool DatabaseManager::openDatabase(const QString& dbPath)
{
    // Set the path to the SQLite file
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "Error: Connection with database failed:" << m_db.lastError().text();
        return false;
    } else {
        qDebug() << "Database connection successful.";
        return createTables();
    }
}

void DatabaseManager::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "Database connection closed.";
    }
}

bool DatabaseManager::createTables()
{
    // Ensure the connection is open before attempting to query
    if (!m_db.isOpen()) {
        qCritical() << "Database is not open.";
        return false;
    }

    QSqlQuery query(m_db);
    
    bool success = query.exec("CREATE TABLE IF NOT EXISTS APUR ("
                              "akaun TEXT PRIMARY KEY, "
                              "value DOUBLE)");

    if (!success) {
        qCritical() << "Error creating tables:" << query.lastError().text();
    }

    insertRows();  // Insert initial data

    return success;
}

void DatabaseManager::insertRows() {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qCritical() << "Database is not open.";
        return;
    }

    QSqlQuery query(db);

    // Define the list of names and initial values
    QList<QString> APURAccounts = {
        "Jualan",
        "Belian",
        "Pulangan Jualan",
        "Pulangan Belian",
        "Inventori Awal",
        "Inventori Akhir",
        "Angkutan Masuk",
        "Upah Atas Belian",
        "Duti Import",
        "Insurans Atas Belian"
    };

    // Use a transaction for performance and atomicity
    if (!db.transaction()) {
        qCritical() << "Error starting transaction:" << db.lastError().text();
        return; // Exit if transaction can't start
    } 

    // Insert into table if not exists
    query.prepare("INSERT OR IGNORE INTO APUR (akaun, value) "
                  "VALUES (:akaun, 0.0)");

    for (const QString& account : APURAccounts) {
        query.bindValue(":akaun", account);

        if (!query.exec()) {
            qCritical() << "Error inserting row into APUR:" << query.lastError().text();
            db.rollback();
            return;
        }
    }

    if (!db.commit()) {
        qCritical() << "Error committing transaction:" << db.lastError().text();
    } else {
        qDebug() << "APUR populated successfully.";
    }
}

QSqlTableModel* DatabaseManager::readAkaunData(const QString& tableName) {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qCritical() << "Database is not open.";
        return nullptr;
    }

    QSqlTableModel* model = new QSqlTableModel(nullptr, db);

    // Select from the table
    model->setTable(tableName);

    // Populate the model with data
    if (!model->select()) {
        qCritical() << "QSqlTableModel select failed:" << model->lastError().text();
        delete model;
        return nullptr;
    }

    return model;
}