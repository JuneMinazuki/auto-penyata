#include "mainwindow.h"
#include "./ui_mainwindow.h"

// Hash of the item in sidebar
static const QHash<QString, QString> s_sidebarActions = {
    {"📋 APUR", "APUR"},
    {"🛒 Belanja", "Belanja"},
    {"📊 Hasil", "Hasil"},
    {"🏠 Aset Bukan Semasa", "AsetBukanSemasa"},
    {"💵 Aset Semasa", "AsetSemasa"},
    {"🤝 Liabiliti Bukan Semasa", "LiabilitiBukanSemasa"},
    {"🏦 Liabiliti Semasa", "LiabilitiSemasa"},
    {"🏢 Ekuiti Pemilik", "EkuitiPemilik"},
    {"🛠️ Settings", "Settings"},
    {"🖨️ Export PDF", "Export"}
};

// VARIABLE
QHash<QString, int> MainWindow::m_pageIndexMap;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); 
    ui->MainScreen->setCurrentIndex(0);

    // Populate the index map once
    populatePageIndexMap();

    // Connect the signal to the slot
    connect(ui->SidebarWidget, &QListWidget::itemClicked, 
            this, &MainWindow::onSidebarItemClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Helper function to populate the index map
void MainWindow::populatePageIndexMap()
{
    QStackedWidget* mainScreen = ui->MainScreen;
    
    for (int i = 0; i < mainScreen->count(); ++i) {
        QWidget* page = mainScreen->widget(i);
        m_pageIndexMap.insert(page->objectName(), i);
    }
}

void MainWindow::onSidebarItemClicked(QListWidgetItem *item)
{
    QString itemText = item->text();
    qDebug() << "Button Clicked:" << itemText;
    QString actionName = s_sidebarActions.value(itemText); 

    if (m_pageIndexMap.contains(actionName)) {
        int index = m_pageIndexMap.value(actionName);
        ui->MainScreen->setCurrentIndex(index);
        qDebug() << "Switched to page:" << actionName << " at index:" << index;
    }
    else {
        qDebug() << "ERROR: age objectName not found in map:" << itemText;
    }
}