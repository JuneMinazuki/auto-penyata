#include "pdfgenerator.h"

#include <QMap>
#include <QVariant>
#include <QStandardPaths>
#include <QDir>
#include <QPainter>
#include <QFont>

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

// Generate Apur as a PDF
void PdfGenerator::createApurPdf(const QMap<QString, QVariantMap> &data) {
    //Setup pdf
    std::unique_ptr<QPdfWriter> writer = setupPDF("apur.pdf");
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

    // Get data from setting Json
    QVariantMap settingData = data.value("setting.json");

    QString companyName = settingData.value("CompanyName").toString();
    QDate date = QDate::fromString(settingData["Date"].toString(), "yyyy/MM/dd");

    QLocale malayLocale(QLocale::Malay, QLocale::Malaysia); // Get malay month
    QString reportDate = malayLocale.toString(date, "d MMMM yyyy");
    QString reportName = QString("Akaun Perdagangan bagi tempoh berakhir %1").arg(reportDate);

    // Draw title
    yPos = drawTitle(painter, yPos, companyName, reportName, pageWidth, pageHeight);

    // Get data from apur Json
    QVariantMap apurData = data.value("apur.json");

    // Draw untung kasar
    yPos = drawUntungKasar(painter, yPos, apurData);

    // Finish drawing
    painter.end();
}

// Setup Pdf
std::unique_ptr<QPdfWriter> PdfGenerator::setupPDF(QString fileName) {
    // Get the path to the standard Downloads directory
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    
    // If the downloads path can't be found, go to home directory
    if (downloadsPath.isEmpty()) {
        downloadsPath = QDir::homePath();
    }
    
    // Construct full file path
    QString fullFilePath = QDir::cleanPath(downloadsPath + QDir::separator() + fileName);

    // Setup the PDF Writer
    std::unique_ptr<QPdfWriter> writer = std::make_unique<QPdfWriter>(fullFilePath);
    writer->setPageSize(QPageSize(QPageSize::A4));
    writer->setResolution(300);

    return writer;
}

// Draw title and report name
int PdfGenerator::drawTitle(QPainter& painter, int yPos, QString companyName, QString reportName, int pageWidth, int pageHeight){
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
    painter.drawLine(xCol1, yPos, xCol1, pageHeight); 
    painter.drawLine(xCol2, yPos, xCol2, pageHeight); 
    painter.drawLine(xCol3, yPos, xCol3, pageHeight); 
    yPos += 75;

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

// Draw untung kasar
int PdfGenerator::drawUntungKasar(QPainter& painter, int yPos, const QVariantMap apurData){
    painter.setFont(regularFont);

    // Jualan / Pulangan Jualan / Jualan Bersih
    QVariant jualanBersih;
    yPos = generateRow(painter, "Jualan", apurData["Jualan"], xCol3, yPos);

    if (apurData["Pulangan Jualan"] != "0.00"){
        // Find jualan bersih
        jualanBersih = apurData["Jualan"].toDouble() - apurData["Pulangan Jualan"].toDouble();

        yPos = generateRow(painter, "- Pulangan Jualan", apurData["Pulangan Jualan"], xCol3, yPos, true);
        drawLine(painter, xCol3, yPos);
        yPos = generateRow(painter, "Jualan Bersih", jualanBersih, xCol3, yPos);
    }
    else {
        jualanBersih = apurData["Jualan"];
    }

    // Cari kos Jualan
    bool hasPulanganBelian = (apurData["Pulangan Belian"] != "0.00");
    bool hasKosBelianItems = false;
    QList<QString> kosBelianAccount = {"Angkutan Masuk", "Upah Atas Belian", "Duti Import", "Insurans Atas Belian"};

    // Find at least 1 non-zero item from kosBelianAccount
    for (const QString &key : kosBelianAccount) {
        if (apurData[key].toString() != "0.00") {
            hasKosBelianItems = true;
            break;
        }
    }

    // Inventori Awal
    yPos = drawHeader(painter, "Kos Jualan", yPos);
    yPos = generateRow(painter, "Inventori Awal", apurData["Inventori Awal"], xCol2, yPos);

    // Belian / Pulangan belian / Belian bersih
    QVariant kosBelian;
    if (!hasPulanganBelian && !hasKosBelianItems) {
        yPos = generateRow(painter, "Belian", apurData["Belian"], xCol2, yPos);
        kosBelian = apurData["Belian"];
    }
    else {
        yPos = generateRow(painter, "Belian", apurData["Belian"], xCol1, yPos);

        if (hasPulanganBelian) {
            // Find belian bersih
            QVariant belianBersih = apurData["Belian"].toDouble() - apurData["Pulangan Belian"].toDouble();

            yPos = generateRow(painter, "- Pulangan Belian", apurData["Pulangan Belian"], xCol1, yPos, true);
            drawLine(painter, xCol1, yPos);
            if (hasKosBelianItems){
                yPos = generateRow(painter, "Belian Bersih", belianBersih, xCol1, yPos);
            }
            else{
                yPos = generateRow(painter, "Belian Bersih", belianBersih, xCol2, yPos);
            }
            
            kosBelian = belianBersih;
        }
        else {
            kosBelian = apurData["Belian"];
        }

        if (hasKosBelianItems) {
            // Kos belian
            bool firstItemFound = false;

            for (const QString &key : kosBelianAccount){
                QString displayName;
                if (apurData[key].toString() != "0.00") {
                    // Add "+" to the first non-zero key
                    if (!firstItemFound) {
                        displayName = key.trimmed().prepend("+ ");
                        firstItemFound = true;
                    }
                    else{
                        displayName = key.trimmed().prepend("   ");
                    }

                    // Generate the row and calculate the total kos belian
                    yPos = generateRow(painter, displayName, apurData[key], xCol1, yPos);
                    kosBelian = kosBelian.toDouble() + apurData[key].toDouble();
                }
            }
            drawLine(painter, xCol1, yPos);
            yPos = generateRow(painter, "Kos Belian", kosBelian, xCol2, yPos);
        }
    }

    // Kos barang untuk dijual
    QVariant kosBarangUntukDijual = apurData["Inventori Awal"].toDouble() + kosBelian.toDouble();
    drawLine(painter, xCol2, yPos);
    yPos = generateRow(painter, "Kos Barang Untuk Dijual", kosBarangUntukDijual, xCol2, yPos);

    // Inventori Akhir
    yPos = generateRow(painter, "- Inventori Akhir", apurData["Inventori Akhir"], xCol2, yPos, true);

    // Kos jualan
    QVariant kosJualan = kosBarangUntukDijual.toDouble() - apurData["Inventori Akhir"].toDouble();
    drawLine(painter, xCol2, yPos);
    if (kosJualan.toDouble() >= 0){
        yPos = generateRow(painter, "Kos Jualan", kosJualan, xCol3, yPos, true);
    }
    else {
        QVariant absKosJualan = -kosJualan.toDouble();
        yPos = generateRow(painter, "Kos Jualan", absKosJualan, xCol3, yPos);
    }

    // Untung kasar / rugi kasar
    QVariant untungKasar = jualanBersih.toDouble() - kosJualan.toDouble();
    drawLine(painter, xCol3, yPos);
    if (untungKasar.toDouble() >= 0){
        yPos = generateRow(painter, "Untung Kasar", untungKasar, xCol3, yPos);
    }
    else {
        QVariant absUntungKasar = -untungKasar.toDouble();
        yPos = generateRow(painter, "Rugi Kasar", absUntungKasar, xCol3, yPos, true);
    }

    return yPos;
}

// Create a QRect for the value columns
QRect PdfGenerator::createValueRect(int xLeft, int yBaseLine, const QFontMetrics& fm){
    int yTop = yBaseLine - fm.ascent();
    int height = fm.height();
    return QRect(xLeft, yTop, columnWidth - 20, height);
}

// Create row of account
int PdfGenerator::generateRow(QPainter& painter, const QString& accountName, const QVariant& accountValue, int xCol, int yPos, bool neg){
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
int PdfGenerator::drawHeader(QPainter& painter, const QString& header, int yPos){
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