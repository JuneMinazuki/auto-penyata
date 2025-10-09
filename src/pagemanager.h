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
    QLabel* saveLabel; // Save status label

    // Desired order of blocks
    QStringList desiredOrder;

    // Read from Json
    QVariantMap loadJson(QString fileName);

    // Check for value changes
    void checkForChanges();

protected slots: // or public/private
    virtual void handleSaveButtonClick() = 0;
};
#endif // PAGEMANAGER_H