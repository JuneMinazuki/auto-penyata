#include "apur.h"
#include "jsonmanager.h"
#include "blockmanager.h"

Apur::Apur(Ui::MainWindow *m_ui, QObject *parent) 
    : QObject(parent),
      ui(m_ui),
      m_blockManager(std::make_unique<BlockManager>(nullptr))
{
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
    connect(ui->button_apur_save, &QPushButton::clicked, this, &Apur::handleSaveButtonClick); // Save button

    // Check for value changes
    connect(m_blockManager.get(), &BlockManager::blockValueChanged, this, &Apur::checkForChanges);

    handleApurActivation();
}

// When Apur page is open
void Apur::handleApurActivation()
{
    qDebug() << "Switched to page: APUR";

    ui->label_apur_saved_status->setVisible(false); // Hide save label
    ui->button_apur_save->setEnabled(false); // Disable save button

    // Create accounts block
    QVariantMap jsonData = loadJson();
    m_blockManager->createAccountBlocks(jsonData, desiredOrder, ui);
}

// Load from JSON
QVariantMap Apur::loadJson()
{
    QVariantMap apurs = JsonManager::readJson("apur.json");

    if (apurs.isEmpty()) {
        qWarning() << "apur.json is empty. Cannot load value";
        return QVariantMap();
    }

    return apurs;
}

// When user press Save button
void Apur::handleSaveButtonClick()
{
    ui->label_apur_saved_status->setVisible(true);
}

// Check for changes
void Apur::checkForChanges() 
{
    // Enable the Save button if any change occurred
    ui->button_apur_save->setEnabled(m_blockManager->hasBlockValuesChanged());
}