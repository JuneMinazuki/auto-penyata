#ifndef SETTING_H
#define SETTING_H 

#include "./ui_mainwindow.h"

#include <QObject>

class Setting : public QObject
{
    Q_OBJECT
    
public:
    explicit Setting(Ui::MainWindow *m_ui, QObject *parent = nullptr); 

    // When setting page is open
    void handleSettingActivation();

private slots:
    // Button
    void handleSaveButtonClick(); // Called when Save button pressed
    void handleResetButtonClick(); // Called when Reset button pressed
    void handleSetTodayButtonClick(); // Called when setToday button pressed

    void checkForChanges(); // Check for value changed

private:
    Ui::MainWindow *ui = nullptr;

    // VARIABLE
    QString initialCompanyName;
    QDate initialDate;

    // Read from Json
    void loadJson();

};
#endif // SETTING_H