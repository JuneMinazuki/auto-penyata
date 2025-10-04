#ifndef SETTING_H
#define SETTING_H 

#include <QObject>
#include "./ui_mainwindow.h"

class Setting : public QObject
{
    Q_OBJECT
    
public:
    // When setting page is open
    void handleSettingActivation(Ui::MainWindow *ui);

private slots:
    // Called when Reset button pressed
    void handleResetButtonClick(QLabel *label_reset);

};
#endif // SETTING_H