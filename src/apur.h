#ifndef APUR_H
#define APUR_H 

#include "pagemanager.h"

class Apur : public PageManager
{
    Q_OBJECT
    
public:
    explicit Apur(Ui::MainWindow *m_ui, QObject *parent = nullptr); 

    // When Apur page is open
    void handlePageActivation() override;

private slots:
    void handleSaveButtonClick() override; // Called when Save button pressed
};
#endif // APUR_H