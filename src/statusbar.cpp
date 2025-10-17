#include "statusbar.h"
#include "jsonmanager.h"

StatusBar::StatusBar(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent),
      ui(ui)
{
    // Amount 
    debitAmount = 0;
    creditAmount = 0;

    // Status bar initailsation
    statusBar = ui->statusBar;
    amountLabel = ui->amount_label;
}

// Update status bar
void StatusBar::updateStatusBar(QString page, QVariantMap newData, QVariantMap oldData)
{

}

// First time update status bar
void StatusBar::createStatusBar()
{
        // Read from all Json
    QMap<QString, QVariantMap> jsonData = JsonManager::readAllJson();

    QVariantMap apurData = jsonData.value("apur.json");
    QVariantMap absData = jsonData.value("aset_bukan_semasa.json");
    QVariantMap asData = jsonData.value("aset_semasa.json");
    QVariantMap lbsData = jsonData.value("liabiliti_bukan_semasa.json");
    QVariantMap lsData = jsonData.value("liabiliti_semasa.json");
    QVariantMap belanjaData = jsonData.value("belanja.json");
    QVariantMap hasilData = jsonData.value("hasil.json");
    QVariantMap epData = jsonData.value("ekuiti_pemilik.json");

    // Apur
    creditAmount += apurData["Jualan"].toDouble();
    debitAmount += apurData["Pulangan Jualan"].toDouble();
    debitAmount += apurData["Belian"].toDouble();
    creditAmount += apurData["Pulangan Belian"].toDouble();
    debitAmount += apurData["Angkutan Masuk"].toDouble();
    debitAmount += apurData["Duti Import"].toDouble();
    debitAmount += apurData["Insurans Atas Belian"].toDouble();
    debitAmount += apurData["Upah Atas Belian"].toDouble();
    debitAmount += apurData["Inventori Awal"].toDouble();

    // Aset bukan semasa
    for (const QString &key : absData.keys()) {
        if (key == "_placeholder"){
            continue;
        }

        const QVariant &keyMap = absData[key];
        QVariantMap innerMap = keyMap.toMap();

        debitAmount += innerMap.value("value").toDouble();
        creditAmount += innerMap.value("deprecatedValue").toDouble();
    }

    // Aset semasa
    for (const QString &key : asData.keys()) {
        if (key == "_placeholder"){
            continue;
        }

        if (key == "Peruntukan Hutang Ragu"){
            creditAmount += asData[key].toDouble();
        }
        else {
            debitAmount += asData[key].toDouble();
        }
    }

    // Liabiliti bukan semasa
    for (const QString &key : lbsData.keys()) {
        if (key == "_placeholder"){
            continue;
        }

        creditAmount += lbsData[key].toDouble();
    }

    // Liabiliti semasa
    for (const QString &key : lsData.keys()) {
        if (key == "_placeholder"){
            continue;
        }

        creditAmount += lsData[key].toDouble();
    }

    // Belanja
    for (const QString &key : belanjaData.keys()) {
        if (key == "_placeholder"){
            continue;
        }

        debitAmount += belanjaData[key].toDouble();
    }

    // Hasil
    for (const QString &key : hasilData.keys()) {
        if (key == "_placeholder"){
            continue;
        }

        creditAmount += hasilData[key].toDouble();
    }

    // Ekuiti Pemilik
    creditAmount += apurData["Modal Awal"].toDouble();
    debitAmount += apurData["Ambilan"].toDouble();
}