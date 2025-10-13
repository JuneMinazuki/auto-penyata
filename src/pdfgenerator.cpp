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
QFont PdfGenerator::smallFont("Arial", 8);

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
    pen.setWidth(4);
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

    yPos = generateRow(painter, "Jualan", apurData["Jualan"], xCol3, yPos);
    yPos = generateRow(painter, "- Pulangan Jualan", apurData["Pulangan Jualan"], xCol3, yPos, true);

    return yPos;
}

// Create a QRect for the value columns
QRect PdfGenerator::createValueRect(int xLeft, int yBaseLine, const QFontMetrics& fm){
    int yTop = yBaseLine - fm.ascent();
    int height = fm.height();
    return QRect(xLeft, yTop, columnWidth, height);
}

// Create row of account
int PdfGenerator::generateRow(QPainter& painter, const QString& accountName, const QVariant& accountValue, int xCol, int yPos, bool neg){
    // Get font metrics for calculating rectangle height
    const QFontMetrics fm = painter.fontMetrics();
    
    // Draw the name of account
    painter.drawText(xStartLeft, yPos, accountName);

    // Draw the value
    QRect valueRect = createValueRect(xCol, yPos, fm);
    QString value = (neg) ? QString("(%1)").arg(accountValue.toString()) : accountValue.toString();
    painter.drawText(valueRect, Qt::AlignRight, value);

    yPos += fm.height() * 1.4;
    return yPos;
}