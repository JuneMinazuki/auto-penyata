#include "ekuitipemilik.h"
#include "blockmanager.h"
#include "jsonmanager.h"

EkuitiPemilik::EkuitiPemilik(Ui::MainWindow *m_ui, QObject *parent) 
    : PageManager(m_ui, parent, std::make_unique<BlockManager>(nullptr))
{
    // Page initailsation
    fileName = "ekuiti_pemilik.json";
    scrollArea = ui->scrollable_ep;
    contentWidget = ui->ep_scrollAreaWidgetContents;

    // Desired display order of keys
    desiredOrder = {
        "Modal Awal",
        "Ambilan"
    };
}

// When Ekuiti Pemilik page is open
void EkuitiPemilik::handlePageActivation()
{
    qDebug() << "Switched to page: Ekuiti Pemilik";

    // Create accounts block
    jsonData = JsonManager::readJson(fileName);
    m_blockManager->createAccountBlocks(jsonData, desiredOrder, ui, scrollArea, contentWidget);
}