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
QVariantMap PageManager::loadJson()
{
    QVariantMap datas = JsonManager::readJson(fileName);

    if (datas.isEmpty()) {
        qWarning() << fileName << "is empty. Cannot load value";
        return QVariantMap();
    }

    return datas;
}

// When user press Save button
void PageManager::handleSaveButtonClick()
{
    // Get current settings from the UI
    QVariantMap changedData = m_blockManager->getEditedValueMap();

    // Save the changed data to JSON
    bool success = JsonManager::updateJson(fileName, changedData);

    if (success) {
        // Update the original data
        m_blockManager->updateCurrentValue();

        saveButton->setEnabled(false);

        saveLabel->setText("Saved!");
        saveLabel->setStyleSheet("QLabel { color : #37ba1e }");
    } else {
        saveLabel->setText("Failed to save!");
        saveLabel->setStyleSheet("QLabel { color : #e21717 }");
    }

    saveLabel->setVisible(true);
}

// Check for changes
void PageManager::checkForChanges() 
{
    // Enable the Save button if any change occurred
    saveButton->setEnabled(m_blockManager->hasBlockValuesChanged());
}