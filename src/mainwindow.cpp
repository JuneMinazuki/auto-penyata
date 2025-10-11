#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "jsonmanager.h"

#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); 

    // Hash of the item in sidebar
    m_pageIndexMap = {
        {"📋 APUR", 0},
        {"🛒 Belanja", 1},
        {"📊 Hasil", 2},
        {"🏠 Aset Bukan Semasa", 3},
        {"💵 Aset Semasa", 4},
        {"🤝 Liabiliti Bukan Semasa", 5},
        {"🏦 Liabiliti Semasa", 6},
        {"🏢 Ekuiti Pemilik", 7},
        {"🛠️ Settings", 8},
        {"🖨️ Export PDF", 9}
    };

    // Setup for each pages
    m_apurPage = new Apur(ui, this);
    m_belanjaPage = new Belanja(ui, this);
    m_settingPage = new Setting(ui, this);

    // Default page
    ui->MainScreen->setCurrentIndex(0); 
    m_currentPageManager = dynamic_cast<PageManager*>(m_apurPage);

    // Setup JSON (if applicable)
    JsonManager::initialJsonSetup();

    // When user click on sidebar button
    connect(ui->SidebarWidget, &QListWidget::itemClicked, this, &MainWindow::onSidebarItemClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Performs JSON update for the old page
    if (m_currentPageManager) {
        m_currentPageManager->handlePageDeactivation(); 
    }

    // Accept the event to allow the application to close
    event->accept();
}

// Called when user click on sidebar button.
void MainWindow::onSidebarItemClicked(QListWidgetItem *item)
{
    QString actionName = item->text();

    if (m_pageIndexMap.contains(actionName)) {
        int index = m_pageIndexMap.value(actionName);

        // Performs JSON update for the old page
        if (m_currentPageManager) {
            m_currentPageManager->handlePageDeactivation(); 
        }

        if (index != 9){
            ui->MainScreen->setCurrentIndex(index);
        }

        // Pointer to the new page manager
        QObject* newPageManager = nullptr; 

        // Determine should the app update m_currentPageManager
        bool shouldUpdateCurrentPage = true; 

        switch (index) {
            case 0: // APUR
                newPageManager = m_apurPage;
                if (m_apurPage) {
                    m_apurPage->handlePageActivation();
                }
                break;
            case 1: // Belanja
                newPageManager = m_belanjaPage;
                if (m_belanjaPage) {
                    m_belanjaPage->handlePageActivation();
                }
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
                newPageManager = m_settingPage;
                if (m_settingPage) {
                    m_settingPage->handlePageActivation();
                }
                break;
            case 9: // Export PDF
                shouldUpdateCurrentPage = false;
                generatePDF();
                break;
        }

        // Update the tracker for the next deactivation
        if (shouldUpdateCurrentPage){
            m_currentPageManager = dynamic_cast<PageManager*>(newPageManager);
        }
    }
    else {
        qDebug() << "ERROR: Page not found in map:" << actionName;
    }
}

void MainWindow::generatePDF(){
    // Read from Json
    QVariantMap jsonData = JsonManager::readJson("setting.json");
    PdfGenerator::createApurPdf(jsonData);
}