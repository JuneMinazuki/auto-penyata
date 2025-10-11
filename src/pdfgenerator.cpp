#include "pdfgenerator.h"

#include <QMap>
#include <QVariant>
#include <QStandardPaths>
#include <QDir>
#include <QPdfWriter>
#include <QPainter>
#include <QFont>

// Generate Apur as a PDF
void PdfGenerator::createApurPdf(const QVariantMap &data) {
    // Get the path to the standard Downloads directory
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    
    // If the downloads path can't be found, go to home directory
    if (downloadsPath.isEmpty()) {
        downloadsPath = QDir::homePath();
    }
    
    // Construct full file path
    QString fullFilePath = QDir::cleanPath(downloadsPath + QDir::separator() + "Apur.pdf");

    // Setup the PDF Writer
    QPdfWriter writer(fullFilePath);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(300);

    QPainter painter(&writer);

    // Define margins and starting coordinates
    const int margin = 150;
    int yPos = margin;
    const int xStartLeft = margin;

    // Get page width and height
    int pageWidth = writer.width() - (2 * margin); 
    int pageHeight = writer.height() - (1 * margin);

    // Define the column width for the values
    const int numberColWidth = 300;

    // Define the three value column positions
    const int xCol1 = xStartLeft + 1195; // Column 1 (Left)
    const int xCol2 = xStartLeft + 1495; // Column 2 (Middle)
    const int xCol3 = xStartLeft + 1795; // Column 3 (Right)

    const int lineWidth = xCol3 + 50 - xStartLeft; // Total width for the content
    int rectHeight = 100; // Height of each row

    // Set Fonts
    QFont titleFont("Arial", 14, QFont::Bold);
    QFont headerFont("Arial", 12, QFont::Bold);
    QFont regularFont("Arial", 12);
    QFont smallFont("Arial", 8);

    // Set line width
    QPen pen = painter.pen(); 
    pen.setWidth(4);
    painter.setPen(pen);

    // Create a QRect for the value columns
    auto createNumberRect = [&](int xLeft, int yBaseLine, const QFontMetrics& fm) -> QRect {
        int yTop = yBaseLine - fm.ascent();
        int height = fm.height();
        return QRect(xLeft, yTop, numberColWidth, height);
    };

    // Draw Title
    painter.setFont(titleFont);
    QRect namaPerniagaanRect(xStartLeft, yPos, pageWidth, rectHeight);
    painter.drawText(namaPerniagaanRect, Qt::AlignCenter | Qt::TextWordWrap, "Nama Perniagaan");
    yPos += 100;

    QRect titleRect(xStartLeft, yPos, pageWidth, rectHeight);
    painter.drawText(titleRect, Qt::AlignCenter | Qt::TextWordWrap, "Akaun Perdagangan tahun berakhir 31 Disember 2025");
    yPos += 125;

    // Draw a line under the title
    painter.drawLine(xStartLeft, yPos, xStartLeft + pageWidth, yPos); // Under title
    painter.drawLine(xCol1, yPos, xCol1, pageHeight); 
    painter.drawLine(xCol2, yPos, xCol2, pageHeight); 
    painter.drawLine(xCol3, yPos, xCol3, pageHeight); 
    yPos += 75;

    // Draw value column headers (RM)
    painter.setFont(headerFont);
    QFontMetrics headerFm = painter.fontMetrics();

    QRect col1Rect = createNumberRect(xCol1, yPos, headerFm);
    painter.drawText(col1Rect, Qt::AlignCenter, "RM");

    QRect col2Rect = createNumberRect(xCol2, yPos, headerFm);
    painter.drawText(col2Rect, Qt::AlignCenter, "RM");

    QRect col3Rect = createNumberRect(xCol3, yPos, headerFm);
    painter.drawText(col3Rect, Qt::AlignCenter, "RM");
    yPos += headerFm.height();

    // Get font metrics for calculating rectangle height
    QFontMetrics fm = painter.fontMetrics();

    // Finish drawing
    painter.end();
}