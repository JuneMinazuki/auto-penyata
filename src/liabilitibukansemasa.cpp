#include "liabilitibukansemasa.h"
#include "blockmanager.h"
#include "jsonmanager.h"

LiabilitiBukanSemasa::LiabilitiBukanSemasa(Ui::MainWindow *m_ui, QObject *parent) 
    : PageManager(m_ui, parent, std::make_unique<BlockManager>(nullptr))
{
    // Page initailsation
    fileName = "liabiliti_bukan_semasa.json";
    addButton = ui->button_lbs_addAccount;
    scrollArea = ui->scrollable_lbs;
    contentWidget = ui->lbs_scrollAreaWidgetContents;

    // Button
    connect(addButton, &QPushButton::clicked, this, &LiabilitiBukanSemasa::handleAddButtonClick); // Add button
}

// When Liabilti Bukan Semasa page is open
void LiabilitiBukanSemasa::handlePageActivation()
{
    qDebug() << "Switched to page: Liabiliti Bukan Semasa";

    // Create accounts block
    jsonData = JsonManager::readJson(fileName);
    m_blockManager->createAccountBlocks(jsonData, desiredOrder, ui, scrollArea, contentWidget);
}