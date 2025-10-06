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

    handleApurActivation();
}

// When Apur page is open
void Apur::handleApurActivation()
{
    qDebug() << "Switched to page: APUR";

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