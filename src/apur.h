#ifndef APUR_H
#define APUR_H 

#include "blockmanager.h"

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Apur : public QObject
{
    Q_OBJECT
    
public:
    explicit Apur(Ui::MainWindow *m_ui, QObject *parent = nullptr); 

    // When Apur page is open
    void refreshApurData();

private slots:

private:
    Ui::MainWindow *ui = nullptr; 
    std::unique_ptr<BlockManager> m_blockManager; 

    // Desired order of blocksx
    QStringList desiredOrder;

    // Read from Json
    QVariantMap loadJson();
};
#endif // APUR_H