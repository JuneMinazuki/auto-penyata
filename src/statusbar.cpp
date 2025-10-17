#include "statusbar.h"
#include "jsonmanager.h"

StatusBar::StatusBar(Ui::MainWindow *ui, QObject *parent)
    : QObject(parent),
      ui(ui)
{
    // Amount 
    debitAmount = 0;
    creditAmount = 0;

    // Status bar initailsation
    statusBar = ui->statusBar;
    amountLabel = ui->amount_label;
}

// Update status bar
void StatusBar::updateStatusBar(QString page, QVariantMap newData, QVariantMap oldData)
{

}

// First time update status bar
void StatusBar::createStatusBar()
{
}