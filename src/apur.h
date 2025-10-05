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
    void loadJson();

};
#endif // APUR_H