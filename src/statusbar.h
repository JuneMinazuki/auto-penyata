#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "./ui_mainwindow.h"

class StatusBar : public QObject
{
    Q_OBJECT

public:
    StatusBar(Ui::MainWindow *ui, QObject *parent);

    // Update status bar
    void updateStatusBar(QString page, QVariantMap newData, QVariantMap oldData);
    void createStatusBar();

private:
    Ui::MainWindow *ui;

    // Amount
    double debitAmount;
    double creditAmount;

    // Status bar initailsation
    QWidget* statusBar; // Status bar
    QLabel* amountLabel; // Label showing amount
};
#endif // STATUSBAR_H