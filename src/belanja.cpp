#include "belanja.h"
#include "blockmanager.h"

Belanja::Belanja(Ui::MainWindow *m_ui, QObject *parent) 
    : PageManager(m_ui, parent, std::make_unique<BlockManager>(nullptr))
{
    // Page initailsation
    fileName = "belanja.json";
    saveButton = ui->button_belanja_save;
    addButton = ui->button_belanja_addAccount;
    saveLabel = ui->label_belanja_saved_status;
    scrollArea = ui->scrollable_belanja;
    contentWidget = ui->belanja_scrollAreaWidgetContents;

    // Button
    connect(saveButton, &QPushButton::clicked, this, &Belanja::handleSaveButtonClick); // Save button

    // Check for value changes
    connect(m_blockManager.get(), &BlockManager::blockValueChanged, this, &Belanja::checkForChanges);
}

// When Belanja page is open
void Belanja::handlePageActivation()
{
    qDebug() << "Switched to page: Belanja";

    saveLabel->setVisible(false); // Hide save label
    saveButton->setEnabled(false); // Disable save button

    // Create accounts block
    QVariantMap jsonData = loadJson();
    m_blockManager->createAccountBlocks(jsonData, desiredOrder, ui, scrollArea, contentWidget);
}