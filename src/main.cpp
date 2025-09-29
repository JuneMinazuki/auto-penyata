#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QFont>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // Initializes the Qt system
    MainWindow w;               // Creates the MainWindow object
    w.show();                   // Displays the window
    return a.exec();            // Starts the event loop
}