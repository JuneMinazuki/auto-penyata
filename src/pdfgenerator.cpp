#include "pdfgenerator.h"

#include <QMap>
#include <QVariant>
#include <QStandardPaths>
#include <QDir>
#include <QPainter>
#include <QFont>
#include <QDesktopServices>
#include <QUrl>
#include <QRegularExpression>

// Font
QFont PdfGenerator::titleFont("Arial", 14, QFont::Bold);
QFont PdfGenerator::headerFont("Arial", 12, QFont::Bold);
QFont PdfGenerator::regularFont("Arial", 12);
QFont PdfGenerator::underlineFont = [] {
    QFont font("Arial", 12, QFont::Bold);
    font.setUnderline(true);
    return font;
}();

// Margin
const int PdfGenerator::margin = 150;
const int PdfGenerator::xStartLeft = margin;

// Height and width
const int PdfGenerator::rectHeight = 100;
const int PdfGenerator::columnWidth = 300;

// Define the three value column positions
const int PdfGenerator::xCol1 = xStartLeft + 1195; // Column 1 (Left)
const int PdfGenerator::xCol2 = xStartLeft + 1495; // Column 2 (Middle)
const int PdfGenerator::xCol3 = xStartLeft + 1795; // Column 3 (Right)

// Main method to create the two PDF
void PdfGenerator::createAllPDF(const QMap<QString, QVariantMap> &data) {
    double untungBersih = createApurPdf(data); // APUR
    createPkkPdf(data, untungBersih); // PKK
}

// Generate Apur as a PDF
double PdfGenerator::createApurPdf(const QMap<QString, QVariantMap> &data) {
    // Get data from setting Json
    QVariantMap settingData = data.value("setting.json");

    QString companyName = settingData.value("CompanyName").toString();
    QDate date = QDate::fromString(settingData["Date"].toString(), "yyyy/MM/dd");

    QLocale malayLocale(QLocale::Malay, QLocale::Malaysia); // Get malay month
    QString reportDate = malayLocale.toString(date, "d MMMM yyyy");
    QString reportName = QString("Akaun Perdagangan bagi tempoh berakhir %1").arg(reportDate);

    // Generate file name
    QString fullFilePath = generateFilePath("APUR", companyName, date);

    // Setup pdf
    std::unique_ptr<QPdfWriter> writer = setupPDF(fullFilePath);
    QPainter painter(writer.get());

    // Define starting coordinates
    int yPos = margin;

    // Get page width and height
    const int pageWidth = writer->width() - (2 * margin); 
    const int pageHeight = writer->height() - margin;

    // Set line width
    QPen pen = painter.pen(); 
    pen.setWidth(3);
    painter.setPen(pen);

    // Draw title
    yPos = drawTitle(painter, yPos, companyName, reportName, pageWidth);

    // Calculate data
    UntungKasarData apurData = calculateUntungKasar(data.value("apur.json"));
    UntungBersihData untungBersihData = calculateUntungBersih(data.value("hasil.json"), data.value("belanja.json"), apurData.untungKasar);

    // Draw untung kasar
    yPos = drawUntungKasar(painter, writer.get(), yPos, apurData, (untungBersihData.hasHasil || untungBersihData.hasBelanja));

    // Draw untung bersih
    yPos = drawUntungBersih(painter, writer.get(), yPos, untungBersihData);

    // Finish drawing
    painter.end();

    // Open the PDF file
    if (!fullFilePath.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullFilePath));
    }

    return untungBersihData.untungBersih;
}

// Generate Pkk as a PDF
void PdfGenerator::createPkkPdf(const QMap<QString, QVariantMap> &data, const double untungBersih){
    // Get data from setting Json
    QVariantMap settingData = data.value("setting.json");

    QString companyName = settingData.value("CompanyName").toString();
    QDate date = QDate::fromString(settingData["Date"].toString(), "yyyy/MM/dd");

    QLocale malayLocale(QLocale::Malay, QLocale::Malaysia); // Get malay month
    QString reportDate = malayLocale.toString(date, "d MMMM yyyy");
    QString reportName = QString("Penyata Kedudukan Kewangan pada %1").arg(reportDate);

    // Generate file name
    QString fullFilePath = generateFilePath("PKK", companyName, date);

    // Setup pdf
    std::unique_ptr<QPdfWriter> writer = setupPDF(fullFilePath);
    QPainter painter(writer.get());

    // Define starting coordinates
    int yPos = margin;

    // Get page width and height
    const int pageWidth = writer->width() - (2 * margin); 
    const int pageHeight = writer->height() - margin;

    // Set line width
    QPen pen = painter.pen(); 
    pen.setWidth(3);
    painter.setPen(pen);

    // Draw title
    yPos = drawTitle(painter, yPos, companyName, reportName, pageWidth);

    // Finish drawing
    painter.end();

    // Open the PDF file
    if (!fullFilePath.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullFilePath));
    }
}

// Setup Pdf
std::unique_ptr<QPdfWriter> PdfGenerator::setupPDF(const QString& fullFilePath) {
    // Setup the PDF Writer
    std::unique_ptr<QPdfWriter> writer = std::make_unique<QPdfWriter>(fullFilePath);
    writer->setPageSize(QPageSize(QPageSize::A4));
    writer->setResolution(300);

    return writer;
}

// Draw title and report name
int PdfGenerator::drawTitle(QPainter& painter, int yPos, QString companyName, QString reportName, int pageWidth){
    // Draw Title
    painter.setFont(titleFont);
    QRect namaPerniagaanRect(xStartLeft, yPos, pageWidth, rectHeight);
    painter.drawText(namaPerniagaanRect, Qt::AlignCenter | Qt::TextWordWrap, companyName);
    yPos += 100;

    QRect titleRect(xStartLeft, yPos, pageWidth, rectHeight);
    painter.drawText(titleRect, Qt::AlignCenter | Qt::TextWordWrap, reportName);
    yPos += 125;

    // Draw a line under the title
    painter.drawLine(xStartLeft, yPos, xStartLeft + pageWidth, yPos);
    yPos += 75;

    yPos = drawColumnHeader(painter, yPos);

    return yPos;
}

// Draw untung kasar
int PdfGenerator::drawUntungKasar(QPainter& painter, QPdfWriter* writer, int yPos, const UntungKasarData& apurData, bool containHasilBelanja){
    painter.setFont(regularFont);

    // Jualan / Pulangan Jualan / Jualan Bersih
    yPos = generateRow(painter, writer, "Jualan", apurData.jualan, xCol3, yPos);

    if (apurData.pulanganJualan != 0){
        yPos = generateRow(painter, writer, "- Pulangan Jualan", apurData.pulanganJualan, xCol3, yPos, true);
        drawLine(painter, xCol3, yPos);
        yPos = generateRow(painter, writer, "Jualan Bersih", apurData.jualanBersih, xCol3, yPos);
    }

    // Inventori Awal
    yPos = drawHeader(painter, writer, "Kos Jualan", yPos);
    yPos = generateRow(painter, writer, "Inventori Awal", apurData.inventoriAwal, xCol2, yPos);

    // Belian / Pulangan belian / Belian bersih
    if (!apurData.hasPulanganBelian && !apurData.hasKosBelianItems) {
        yPos = generateRow(painter, writer, "Belian", apurData.belian, xCol2, yPos);
    }
    else {
        yPos = generateRow(painter, writer, "Belian", apurData.belian, xCol1, yPos);

        if (apurData.hasPulanganBelian) {
            yPos = generateRow(painter, writer, "- Pulangan Belian", apurData.pulanganBelian, xCol1, yPos, true);
            drawLine(painter, xCol1, yPos);
            if (apurData.hasKosBelianItems){
                yPos = generateRow(painter, writer, "Belian Bersih", apurData.belianBersih, xCol1, yPos);
            }
            else{
                yPos = generateRow(painter, writer, "Belian Bersih", apurData.belianBersih, xCol2, yPos);
            }
        }

        // Kos belian
        if (apurData.hasKosBelianItems) {
            bool firstItemFound = false;

            for (const QPair<QString, double> &item : apurData.kosBelianAccount){
                // Add "+" to the first non-zero key
                QString label = firstItemFound ? "   " + item.first : "+ " + item.first;
                
                // Generate the row and calculate the total kos belian
                yPos = generateRow(painter, writer, label, item.second, xCol1, yPos);
                firstItemFound = true;
            }
            drawLine(painter, xCol1, yPos);
            yPos = generateRow(painter, writer, "Kos Belian", apurData.kosBelian, xCol2, yPos);
        }
    }

    // Kos barang untuk dijual
    drawLine(painter, xCol2, yPos);
    yPos = generateRow(painter, writer, "Kos Barang Untuk Dijual", apurData.kosBarangUntukDijual, xCol2, yPos);

    // Inventori Akhir
    yPos = generateRow(painter, writer, "- Inventori Akhir", apurData.inventoriAkhir, xCol2, yPos, true);

    // Kos jualan
    drawLine(painter, xCol2, yPos);
    if (apurData.kosJualan >= 0){
        yPos = generateRow(painter, writer, "Kos Jualan", apurData.kosJualan, xCol3, yPos, true);
    }
    else {
        yPos = generateRow(painter, writer, "Kos Jualan", -apurData.kosJualan, xCol3, yPos);
    }

    // Untung kasar / rugi kasar
    drawLine(painter, xCol3, yPos);
    if (containHasilBelanja){
        if (apurData.untungKasar >= 0){
            yPos = generateRow(painter, writer, "Untung Kasar", apurData.untungKasar, xCol3, yPos);
        }
        else {
            yPos = generateRow(painter, writer, "Rugi Kasar", -apurData.untungKasar, xCol3, yPos, true);
        }
    }
    else {
        if (apurData.untungKasar >= 0){
            yPos = generateRow(painter, writer, "Untung Bersih", apurData.untungKasar, xCol3, yPos);
        }
        else {
            yPos = generateRow(painter, writer, "Rugi Bersih", -apurData.untungKasar, xCol3, yPos, true);
        }
        drawLine(painter, xCol3, yPos);
    }

    return yPos;
}

// Draw untung bersih
int PdfGenerator::drawUntungBersih(QPainter& painter, QPdfWriter* writer, int yPos, const UntungBersihData& data){
    // Hasil
    if (data.hasHasil){
        yPos = drawHeader(painter, writer, "+ Hasil", yPos);

        if (data.hasilAccount.count() == 1){
            const QPair<QString, double>& item = data.hasilAccount.first();
            yPos = generateRow(painter, writer, item.first, item.second, xCol3, yPos);
        }
        else{
            for (const QPair<QString, double> &item : data.hasilAccount){
                yPos = generateRow(painter, writer, item.first, item.second, xCol2, yPos);
            }

            drawLine(painter, xCol2, yPos);
            yPos = generateRow(painter, writer, "", data.totalHasil, xCol3, yPos);
        }

        drawLine(painter, xCol3, yPos);
        if (data.hasBelanja) {
            yPos = generateRow(painter, writer, "", data.tambahHasil, xCol3, yPos);
        }
        else {
            if (data.tambahHasil >= 0){
                yPos = generateRow(painter, writer, "Untung Bersih", data.tambahHasil, xCol3, yPos);
            }
            else {
                yPos = generateRow(painter, writer, "Rugi Bersih", -data.tambahHasil, xCol3, yPos, true);
            }
            drawLine(painter, xCol3, yPos);
        }
    }

    // Belanja
    if (data.hasBelanja){
        yPos = drawHeader(painter, writer, "- Belanja", yPos);

        if (data.belanjaAccount.count() == 1){
            const QPair<QString, double>& item = data.belanjaAccount.first();
            yPos = generateRow(painter, writer, item.first, item.second, xCol3, yPos, true);
        }
        else{
            for (const QPair<QString, double> &item : data.belanjaAccount){
                yPos = generateRow(painter, writer, item.first, item.second, xCol2, yPos);
            }

            drawLine(painter, xCol2, yPos);
            yPos = generateRow(painter, writer, "", data.totalBelanja, xCol3, yPos, true);
        }

        drawLine(painter, xCol3, yPos);
        if (data.untungBersih >= 0){
            yPos = generateRow(painter, writer, "Untung Bersih", data.untungBersih, xCol3, yPos);
        }
        else {
            yPos = generateRow(painter, writer, "Rugi Bersih", -data.untungBersih, xCol3, yPos, true);
        }
        drawLine(painter, xCol3, yPos);
    }
    
    return yPos;
}

// Calculate untung kasar
PdfGenerator::UntungKasarData PdfGenerator::calculateUntungKasar(const QVariantMap& data){
    PdfGenerator::UntungKasarData apurData;

    // Jualan
    apurData.jualan = data["Jualan"].toDouble();
    apurData.pulanganJualan = data["Pulangan Jualan"].toDouble();
    apurData.jualanBersih = apurData.jualan - apurData.pulanganJualan;

    // Belian / Inventori Awal
    apurData.inventoriAwal = data["Inventori Awal"].toDouble();
    apurData.belian = data["Belian"].toDouble();
    apurData.pulanganBelian = data["Pulangan Belian"].toDouble();
    apurData.belianBersih = apurData.belian - apurData.pulanganBelian;
    apurData.hasPulanganBelian = (apurData.pulanganBelian != 0.0);

    // Kos belian
    apurData.kosBelian = apurData.belianBersih;
    const QList<QString> kosBelianKeys = {"Angkutan Masuk", "Upah Atas Belian", "Duti Import", "Insurans Atas Belian"};
    for (const QString& key : kosBelianKeys) {
        double value = data[key].toDouble();
        if (value != 0.0) {
            apurData.kosBelianAccount.append({key, value});
            apurData.kosBelian += value;
        }
    }
    apurData.hasKosBelianItems = !apurData.kosBelianAccount.isEmpty();

    // Kos barang untuk dijual
    apurData.kosBarangUntukDijual = apurData.inventoriAwal + apurData.kosBelian;

    // Kos Jualan
    apurData.inventoriAkhir = data["Inventori Akhir"].toDouble();
    apurData.kosJualan = apurData.kosBarangUntukDijual - apurData.inventoriAkhir;

    // Untung kasar / rugi kasar
    apurData.untungKasar = apurData.jualanBersih - apurData.kosJualan;

    return apurData;
}

// Calculate untung bersih
PdfGenerator::UntungBersihData PdfGenerator::calculateUntungBersih(const QVariantMap& hasilData, const QVariantMap& belanjaData, const double untungKasar){
    PdfGenerator::UntungBersihData untungBersihData;

    // Hasil
    for (const QString &key : hasilData.keys()) {
        if (key == "_placeholder"){
            continue;
        }

        double value = hasilData[key].toDouble();
        if (value != 0.0) {
            untungBersihData.hasilAccount.append({key, value});
            untungBersihData.totalHasil += value;
        }
    }
    untungBersihData.tambahHasil = untungKasar + untungBersihData.totalHasil;
    untungBersihData.hasHasil = !untungBersihData.hasilAccount.isEmpty();

    // Belanja
    for (const QString &key : belanjaData.keys()) {
        if (key == "_placeholder"){
            continue;
        }

        double value = belanjaData[key].toDouble();
        if (value != 0.0) {
            untungBersihData.belanjaAccount.append({key, value});
            untungBersihData.totalBelanja += value;
        }
    }
    untungBersihData.untungBersih = untungBersihData.tambahHasil - untungBersihData.totalBelanja;
    untungBersihData.hasBelanja = !untungBersihData.belanjaAccount.isEmpty();

    return untungBersihData;
}

// Create a QRect for the value columns
QRect PdfGenerator::createValueRect(int xLeft, int yBaseLine, const QFontMetrics& fm){
    int yTop = yBaseLine - fm.ascent();
    int height = fm.height();
    return QRect(xLeft, yTop, columnWidth - 20, height);
}

// Create row of account
int PdfGenerator::generateRow(QPainter& painter, QPdfWriter* writer, const QString& accountName, const QVariant& accountValue, int xCol, int yPos, bool neg){
    // Check if out of bound
    yPos = checkYPos(painter, writer, yPos);
    
    // Get font metrics for calculating rectangle height
    const QFontMetrics fm = painter.fontMetrics();
    
    // Draw the name of account
    painter.drawText(xStartLeft, yPos, accountName);

    // Draw the value
    QString stringValue = QString::number(accountValue.toDouble(), 'f', 2);
    QString value = (neg) ? QString("(%1)").arg(stringValue) : stringValue;

    QRect valueRect = createValueRect(xCol, yPos, fm);
    painter.drawText(valueRect, Qt::AlignRight, value);

    yPos += fm.height() * 1.4;
    return yPos;
}

// Draw line
void PdfGenerator::drawLine(QPainter& painter, int xCol, int yPos){
    painter.drawLine(xCol, yPos - 52.4, xCol + columnWidth, yPos - 52.4);
}

// Draw line
int PdfGenerator::drawHeader(QPainter& painter, QPdfWriter* writer, const QString& header, int yPos){
    // Check if out of bound
    yPos = checkYPos(painter, writer, yPos);
    
    // Get font metrics
    painter.setFont(underlineFont);
    const QFontMetrics fm = painter.fontMetrics();
    yPos += fm.height() * 1.4;

    // Draw the name of account
    painter.drawText(xStartLeft, yPos, header);
    yPos += fm.height() * 1.4;

    painter.setFont(regularFont);
    return yPos;
}

// Draw column header
int PdfGenerator::drawColumnHeader(QPainter& painter, int yPos){
    // Draw value column headers (RM)
    painter.setFont(headerFont);
    QFontMetrics headerFm = painter.fontMetrics();

    QRect col1Rect = createValueRect(xCol1, yPos, headerFm);
    painter.drawText(col1Rect, Qt::AlignCenter, "RM");

    QRect col2Rect = createValueRect(xCol2, yPos, headerFm);
    painter.drawText(col2Rect, Qt::AlignCenter, "RM");

    QRect col3Rect = createValueRect(xCol3, yPos, headerFm);
    painter.drawText(col3Rect, Qt::AlignCenter, "RM");
    yPos += headerFm.height() * 1.4;

    return yPos;
}

// Setup new page
int PdfGenerator::checkYPos(QPainter& painter, QPdfWriter* writer, int yPos){
    if (yPos >= 3336){
        if (!writer->newPage()) {
            qWarning() << "Failed to create a new page!";
            return -1;
        }

        yPos = drawColumnHeader(painter, margin);
        painter.setFont(regularFont);
    }

    return yPos;
}

// Generate file name
QString PdfGenerator::generateFilePath(QString fileName, QString companyName, QDate date){
    // Sanitize the company name to remove characters that are invalid in file paths.
    QString safeCompanyName = companyName;
    safeCompanyName.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");

    // Create a base name for the file. Using a consistent, sortable date format is best.
    QString baseName = QString("%1 %2 %3").arg(safeCompanyName, fileName, date.toString("yyyy-MM-dd"));
    const QString extension = ".pdf";

    // Get the path to the standard Downloads directory
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    
    // If the downloads path can't be found, go to home directory
    if (downloadsPath.isEmpty()) {
        downloadsPath = QDir::homePath();
    }

    // Construct initial full file path
    QString fullFilePath = QDir(downloadsPath).filePath(baseName + extension);

    // Check for duplicate
    QFileInfo fileInfo(fullFilePath);
    int counter = 1;
    while (fileInfo.exists()) {
        QString newName = QString("%1 (%2)").arg(baseName).arg(counter);
        fullFilePath = QDir(downloadsPath).filePath(newName + extension);
        
        // Update the QFileInfo object to check the new path in the next loop iteration.
        fileInfo.setFile(fullFilePath); 
        counter++;
    }

    return fullFilePath;
}