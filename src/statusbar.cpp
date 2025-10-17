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
    updateApurValue(apurData);

    // Aset bukan semasa
    updateAbsValue(absData);

    // Aset semasa
    updateAsValue(asData);

    // Liabiliti bukan semasa
    updateCustomValue(lbsData, creditAmount);

    // Liabiliti semasa
    updateCustomValue(lsData, creditAmount);

    // Belanja
    updateCustomValue(belanjaData, debitAmount);

    // Hasil
    updateCustomValue(hasilData, creditAmount);

    // Ekuiti Pemilik
    updateEpValue(epData);
}

// Update Apur value
void StatusBar::updateApurValue(const QVariantMap apurData, bool reverse)
{
    if (!reverse){
        creditAmount += apurData["Jualan"].toDouble();
        debitAmount += apurData["Pulangan Jualan"].toDouble();
        debitAmount += apurData["Belian"].toDouble();
        creditAmount += apurData["Pulangan Belian"].toDouble();
        debitAmount += apurData["Angkutan Masuk"].toDouble();
        debitAmount += apurData["Duti Import"].toDouble();
        debitAmount += apurData["Insurans Atas Belian"].toDouble();
        debitAmount += apurData["Upah Atas Belian"].toDouble();
        debitAmount += apurData["Inventori Awal"].toDouble();
    }
    else {
        creditAmount -= apurData["Jualan"].toDouble();
        debitAmount -= apurData["Pulangan Jualan"].toDouble();
        debitAmount -= apurData["Belian"].toDouble();
        creditAmount -= apurData["Pulangan Belian"].toDouble();
        debitAmount -= apurData["Angkutan Masuk"].toDouble();
        debitAmount -= apurData["Duti Import"].toDouble();
        debitAmount -= apurData["Insurans Atas Belian"].toDouble();
        debitAmount -= apurData["Upah Atas Belian"].toDouble();
        debitAmount -= apurData["Inventori Awal"].toDouble();
    }
}

// Update Aset Bukan Semasa value
void StatusBar::updateAbsValue(const QVariantMap absData, bool reverse)
{
    if (!reverse){
        for (const QString &key : absData.keys()) {
            if (key == "_placeholder"){
                continue;
            }

            const QVariant &keyMap = absData[key];
            QVariantMap innerMap = keyMap.toMap();

            debitAmount += innerMap.value("value").toDouble();
            creditAmount += innerMap.value("deprecatedValue").toDouble();
        }
    }
    else {
        for (const QString &key : absData.keys()) {
            if (key == "_placeholder"){
                continue;
            }

            const QVariant &keyMap = absData[key];
            QVariantMap innerMap = keyMap.toMap();

            debitAmount -= innerMap.value("value").toDouble();
            creditAmount -= innerMap.value("deprecatedValue").toDouble();
        }
    }
}

// Update Aset Semasa value
void StatusBar::updateAsValue(const QVariantMap asData, bool reverse)
{
    if (!reverse){
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
    }
    else {
        for (const QString &key : asData.keys()) {
            if (key == "_placeholder"){
                continue;
            }

            if (key == "Peruntukan Hutang Ragu"){
                creditAmount -= asData[key].toDouble();
            }
            else {
                debitAmount -= asData[key].toDouble();
            }
        }
    }
}

// Update other page value
void StatusBar::updateCustomValue(const QVariantMap data, double& amount, bool reverse)
{
    if (!reverse){
        for (const QString &key : data.keys()) {
            if (key == "_placeholder"){
                continue;
            }

            amount += data[key].toDouble();
        }
    }
    else {
        for (const QString &key : data.keys()) {
            if (key == "_placeholder"){
                continue;
            }

            amount -= data[key].toDouble();
        }
    }
}

// Update Ekuiti Pemilik value
void StatusBar::updateEpValue(const QVariantMap epData, bool reverse)
{
    if (!reverse){
        creditAmount += epData["Modal Awal"].toDouble();
        debitAmount += epData["Ambilan"].toDouble();
    }
    else {
        creditAmount -= epData["Modal Awal"].toDouble();
        debitAmount -= epData["Ambilan"].toDouble();
    }
}