#include "liabilitisemasa.h"
#include "blockmanager.h"
#include "jsonmanager.h"

LiabilitiSemasa::LiabilitiSemasa(Ui::MainWindow *m_ui, QObject *parent) 
    : PageManager(m_ui, parent, std::make_unique<BlockManager>(nullptr))
{
    // Page initailsation
    fileName = "liabiliti_semasa.json";
    addButton = ui->button_ls_addAccount;
    scrollArea = ui->scrollable_ls;
    contentWidget = ui->ls_scrollAreaWidgetContents;

    // Desired display order of keys
    desiredOrder = {
        "Akaun Belum Bayar"
    };

    // Button
    connect(addButton, &QPushButton::clicked, this, &LiabilitiSemasa::handleAddButtonClick); // Add button
}

// When Liabiliti Semasa page is open
void LiabilitiSemasa::handlePageActivation()
{
    qDebug() << "Switched to page: Liabiliti Semasa";

    // Create accounts block
    jsonData = JsonManager::readJson(fileName);
    m_blockManager->createAccountBlocks(jsonData, desiredOrder, ui, scrollArea, contentWidget);
}