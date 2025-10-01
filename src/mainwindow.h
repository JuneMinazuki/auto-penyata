#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSidebarItemClicked(QListWidgetItem *item);
    void displayAkaunData(QString sidebarName);

private:
    Ui::MainWindow *ui;

    //VARIABLE
    static QHash<QString, int> m_pageIndexMap;

    void populatePageIndexMap(); // Helper function to populate the index map
};
#endif // MAINWINDOW_H
