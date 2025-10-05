#ifndef APUR_H
#define APUR_H 

#include <QObject>
#include "./ui_mainwindow.h"

class Apur : public QObject
{
    Q_OBJECT
    
public:
    // When Apur page is open
    void handleApurActivation(Ui::MainWindow *ui);

private slots:

private:
    Ui::MainWindow *ui = nullptr; 

    // Read from Json
    QVariantMap loadJson();

    // Create account blocks
    void createJsonBlocks(const QVariantMap &variantMap);
    QWidget* createBlock(const QString &key, QString value);

};
#endif // APUR_H