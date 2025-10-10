#include "apur.h"
#include "blockmanager.h"
#include "jsonmanager.h"

Apur::Apur(Ui::MainWindow *m_ui, QObject *parent) 
    : PageManager(m_ui, parent, std::make_unique<BlockManager>(nullptr))
{
    // Page initailsation
    fileName = "apur.json";
    saveButton = ui->button_apur_save;
    saveLabel = ui->label_apur_saved_status;
    scrollArea = ui->scrollable_apur;
    contentWidget = ui->apur_scrollAreaWidgetContents;

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
    QVariantMap jsonData = JsonManager::readJson(fileName);
    m_blockManager->createAccountBlocks(jsonData, desiredOrder, ui, scrollArea, contentWidget);
}