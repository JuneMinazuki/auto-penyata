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

// Hast of the item in sidebar
static const QHash<QString, QString> s_sidebarActions = {
    {"📋 APUR", "APUR"},
    {"🛒 Belanja", "Belanja"},
    {"📊 Hasil", "Hasil"},
    {"🏠 Aset Bukan Semasa", "Aset Bukan Semasa"},
    {"💵 Aset Semasa", "Aset Semasa"},
    {"🤝 Liabiliti Bukan Semasa", "Liabiliti Bukan Semasa"},
    {"🏦 Liabiliti Semasa", "Liabiliti Semasa"},
    {"🏢 Ekuiti Pemilik", "Ekuiti Pemilik"},
    {"🛠️ Settings", "Settings"},
    {"🖨️ Export PDF", "Export PDF"}
};

void MainWindow::onSidebarItemClicked(QListWidgetItem *item)
{
    QString itemText = item->text();
    qDebug() << "Button Clicked:" << itemText;

    if (s_sidebarActions.contains(itemText)) {
        QString actionName = s_sidebarActions.value(itemText);
        qDebug() << "Action: Loading" << actionName;
    }
    else{
        qDebug() << "ERROR: Unknown sidebar item clicked:" << itemText;
    }
}