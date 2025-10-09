#include "apur.h"
#include "jsonmanager.h"
#include "blockmanager.h"

Apur::Apur(Ui::MainWindow *m_ui, QObject *parent) 
    : PageManager(m_ui, parent, std::make_unique<BlockManager>(nullptr))
{
    // Page initailsation
    fileName = "apur.json";
    saveButton = ui->button_apur_save;
    saveLabel = ui->label_apur_saved_status;

    // Desired display order of keys
    desiredOrder = {
        "Jualan",
        "Pulangan Jualan",
        "Inventori Awal",
        "Belian",
        "Pulangan Belian",
        "Angkutan Masuk",
        "Upah Atas Belian",
        "Duti Import",
        "Insurans Atas Belian",
        "Inventori Akhir"
    };

    // Button
    connect(saveButton, &QPushButton::clicked, this, &Apur::handleSaveButtonClick); // Save button

    // Check for value changes
    connect(m_blockManager.get(), &BlockManager::blockValueChanged, this, &Apur::checkForChanges);

    handlePageActivation();
}

// When Apur page is open
void Apur::handlePageActivation()
{
    qDebug() << "Switched to page: APUR";

    saveLabel->setVisible(false); // Hide save label
    saveButton->setEnabled(false); // Disable save button

    // Create accounts block
    QVariantMap jsonData = loadJson(fileName);
    m_blockManager->createAccountBlocks(jsonData, desiredOrder, ui);
}

// When user press Save button
void Apur::handleSaveButtonClick()
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