#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    /**
     * @brief Called when user click on sidebar button.
     * 
     * @param item Index of the sidebar button
     */
    void onSidebarItemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    //VARIABLE
    static QHash<QString, int> m_pageIndexMap;

    /**
     * @brief Populate the index map to the button on the sidebar
     */
    void populatePageIndexMap();
};
#endif // MAINWINDOW_H
