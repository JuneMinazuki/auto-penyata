#include "pagemanager.h"
#include "jsonmanager.h"
#include "blockmanager.h"

PageManager::PageManager(Ui::MainWindow *ui, QObject *parent, std::unique_ptr<BlockManager> blockManager)
    : QObject(parent),
      ui(ui),
      m_blockManager(std::move(blockManager))
{
}

PageManager::PageManager(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent),
      ui(ui)
{
}

PageManager::~PageManager() = default; 

// When page is closed
void PageManager::handlePageDeactivation()
{   
    // Get all data
    QVariantMap newData = m_blockManager->getAllValueMap();
    newData.insert("_placeholder", true);

    // Save to Json
    bool success = JsonManager::writeJson(fileName, newData);

    if (success) {
        qDebug() << "Data saved successfully to" << fileName;
    } else {
        qDebug() << "Failed to save data to" << fileName;
    }
}

// When user press Add button
void PageManager::handleAddButtonClick()
{
}