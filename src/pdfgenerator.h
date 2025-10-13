#ifndef PDFMGENERATOR_H
#define PDFMGENERATOR_H 

#include <memory>
#include <QObject>
#include <QPdfWriter>

class PdfGenerator : public QObject
{
    Q_OBJECT
    
public:
    static void createApurPdf(const QMap<QString, QVariantMap> &data);

private:
    // Setup Pdf
    static std::unique_ptr<QPdfWriter> setupPDF(QString fileName);

    // Draw title and report name
    static int drawTitle(QPainter& painter, int yPos, QString companyName, QString reportName, int pageWidth, int pageHeight);

    // Draw untung kasar
    static int drawUntungKasar(QPainter& painter, int yPos, const QVariantMap apurData);

    // Create a QRect for the value columns
    static QRect createValueRect(int xLeft, int yBaseLine, const QFontMetrics& fm);

    // Create row of account
    static int generateRow(QPainter& painter, const QString& accountName, const QVariant& accountValue, int xCol, int yPos, bool neg = false);

    // Font
    static QFont titleFont;
    static QFont headerFont;
    static QFont regularFont;
    static QFont smallFont;

    // Margin
    static const int margin;
    static const int xStartLeft;

    // Height and width
    static const int rectHeight;
    static const int columnWidth;

    // Define the three value column positions
    static const int xCol1;
    static const int xCol2;
    static const int xCol3;
};
#endif // PDFMGENERATOR_H