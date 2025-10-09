#ifndef APUR_H
#define APUR_H 

#include "blockmanager.h"
#include "./ui_mainwindow.h"

#include <memory>

class Apur : public QObject
{
    Q_OBJECT
    
public:
    explicit Apur(Ui::MainWindow *m_ui, QObject *parent = nullptr); 

    // When Apur page is open
    void handleApurActivation();

private slots:
    void handleSaveButtonClick(); // Called when Save button pressed

private:
    Ui::MainWindow *ui = nullptr; 
    std::unique_ptr<BlockManager> m_blockManager; 

    // Original data
    QVariantMap m_originalData;

    // Desired order of blocksx
    QStringList desiredOrder;

    // Read from Json
    QVariantMap loadJson();

    // Get current data
    QVariantMap getCurrentData();

    // Check for value changes
    void checkForChanges();
};
#endif // APUR_H