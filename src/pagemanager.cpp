#include "pagemanager.h"
#include "jsonmanager.h"
#include "blockmanager.h"

PageManager::PageManager(Ui::MainWindow *ui, QObject *parent, std::unique_ptr<BlockManager> blockManager)
    : QObject(parent),
      ui(ui),
      m_blockManager(std::move(blockManager))
{
}

// Load from JSON
QVariantMap PageManager::loadJson(QString fileName)
{
    QVariantMap datas = JsonManager::readJson(fileName);

    if (datas.isEmpty()) {
        qWarning() << fileName << "is empty. Cannot load value";
        return QVariantMap();
    }

    return datas;
}

// Check for changes
void PageManager::checkForChanges() 
{
    // Enable the Save button if any change occurred
    saveButton->setEnabled(m_blockManager->hasBlockValuesChanged());
}