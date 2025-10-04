#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "jsonmanager.h"

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

    // Setup JSON (if applicable)
    JsonManager::initialJsonSetup();

    // Populate the index map once
    populatePageIndexMap();

    // When user click on sidebar button
    connect(ui->SidebarWidget, &QListWidget::itemClicked, this, &MainWindow::onSidebarItemClicked);
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

// Called when user click on sidebar button.
void MainWindow::onSidebarItemClicked(QListWidgetItem *item)
{
    QString itemText = item->text();
    QString actionName = s_sidebarActions.value(itemText); 

    if (m_pageIndexMap.contains(actionName)) {
        int index = m_pageIndexMap.value(actionName);
        ui->MainScreen->setCurrentIndex(index);

        switch (index) {
            case 0: // APUR
                break;
            case 1: // Belanja
                break;
            case 2: // Hasil
                break;
            case 3: // Aset Bukan Semasa
                break;
            case 4: // Aset Semasa
                break;
            case 5: // Liabiliti Bukan Semasa
                break;
            case 6: // Liabiliti Semasa
                break;
            case 7: // Ekuiti Pemilik
                break;
            case 8: // Setting
                settingPage.handleSettingActivation(ui);
                break;
            case 9: // Export PDF
                break;
        }
    }
    else {
        qDebug() << "ERROR: Page not found in map:" << itemText;
    }
}