#ifndef PDFMGENERATOR_H
#define PDFMGENERATOR_H 

#include <QObject>

class PdfGenerator : public QObject
{
    Q_OBJECT
    
public:
    static void createApurPdf(const QVariantMap &data);
};
#endif // PDFMGENERATOR_H