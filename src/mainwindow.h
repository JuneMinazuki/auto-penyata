#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "setting.h"
#include "apur.h"

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

private slots:
    void onSidebarItemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    // Pages
    Apur apurPage;
    Setting settingPage;

    //VARIABLE
    static QHash<QString, int> m_pageIndexMap;

    void populatePageIndexMap(); // Helper function to populate the index map
};
#endif // MAINWINDOW_H
