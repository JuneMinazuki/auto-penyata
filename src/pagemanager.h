#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H 

#include "./ui_mainwindow.h"

#include <memory>

class BlockManager;

class PageManager : public QObject
{
    Q_OBJECT
    
public:
    explicit PageManager(Ui::MainWindow *ui, QObject *parent, std::unique_ptr<BlockManager> blockManager);
    virtual ~PageManager() = default;

    // When page is open
    virtual void handlePageActivation() = 0;

protected:
    Ui::MainWindow *ui; 
    std::unique_ptr<BlockManager> m_blockManager; 

    // Page initailsation
    QString fileName; // Json file name
    QPushButton* saveButton; // Save button
    QPushButton* addButton; // Add account button
    QLabel* saveLabel; // Save status label
    QScrollArea* scrollArea; // Scrollable area
    QWidget *contentWidget; // Area to put all account block

    // Desired order of blocks
    QStringList desiredOrder;

    // Check for value changes
    void checkForChanges();

protected slots:
    virtual void handleSaveButtonClick();
    virtual void handleAddButtonClick();
};
#endif // PAGEMANAGER_H