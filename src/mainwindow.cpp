#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); 

    // Connect the signal to the slot
    connect(ui->SidebarWidget, &QListWidget::itemClicked, 
            this, &MainWindow::onSidebarItemClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSidebarItemClicked(QListWidgetItem *item)
{
    QString itemText = item->text();
    qDebug() << "Button Clicked:" << itemText;

    if (itemText == "📋 APUR") {
        qDebug() << "Action: Loading APUR";
    }
    else if (itemText ==  "🛒 Belanja"){
        qDebug() << "Action: Loading Belanja";
    }
    else if (itemText ==  "📊 Hasil"){
        qDebug() << "Action: Loading Hasil";
    }
    else if (itemText ==  "🏠 Aset Bukan Semasa"){
        qDebug() << "Action: Loading Aset Bukan Semasa";
    }
    else if (itemText ==  "💵 Aset Semasa"){
        qDebug() << "Action: Loading Aset Semasa";
    }
    else if (itemText ==  "🤝 Liabiliti Bukan Semasa"){
        qDebug() << "Action: Loading Liabiliti Bukan Semasa";
    }
    else if (itemText ==  "🏦 Liabiliti Semasa"){
        qDebug() << "Action: Loading Liabiliti Semasa";
    }
    else if (itemText ==  "🏢 Ekuiti Pemilik"){
        qDebug() << "Action: Loading Ekuiti Pemilik";
    }
    else if (itemText ==  "🛠️ Settings"){
        qDebug() << "Action: Loading Settings";
    }
}