#include <QApplication>
#include <QStandardPaths>
#include <QDir>

#include "mainwindow.h"
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // Initializes the Qt system

    // Get path to DB
    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataLocation);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString dbPath = dataLocation + QDir::separator() + "auto-penyata.db";
    if (!DatabaseManager::instance().openDatabase(dbPath)) {
        // Handle error: database could not be opened
        qCritical() << "FATAL: Could not open database. Exiting.";
        return 1; 
    }

    MainWindow w;               // Creates the MainWindow object
    w.show();                   // Displays the window

    int result = a.exec();      // Starts the event loop

    DatabaseManager::instance().closeDatabase(); // Close DB connection when done

    return result; 
}