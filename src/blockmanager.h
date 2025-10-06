#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#include "ui_mainwindow.h" 

#include <QFrame>
#include <QVariantMap>

class BlockManager : public QFrame
{
    Q_OBJECT

public:
    explicit BlockManager(QObject *parent = nullptr);

    // Creates visual blocks for each item in the variant map from Json
    void createAccountBlocks(const QVariantMap &variantMap, QStringList desiredOrder, Ui::MainWindow *ui);

private:
    // Create each block
    QWidget* createBasicBlock(const QString &key, QString value);
};

#endif // BLOCKMANAGER_H