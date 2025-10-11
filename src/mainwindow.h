#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "setting.h"
#include "apur.h"
#include "belanja.h"
#include "pagemanager.h"
#include "pdfgenerator.h"

#include <QApplication>
#include <QMainWindow>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onSidebarItemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    // Pages
    Apur *m_apurPage; 
    Belanja *m_belanjaPage;
    Setting *m_settingPage;

    //VARIABLE
    QHash<QString, int> m_pageIndexMap; // Index of each page

    // A pointer to the currently active page manager. 
    PageManager* m_currentPageManager = nullptr;

    void generatePDF();
};
#endif // MAINWINDOW_H
