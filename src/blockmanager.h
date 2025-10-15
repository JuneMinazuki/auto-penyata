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
    void createAccountBlocks(const QVariantMap &variantMap, QStringList desiredOrder, Ui::MainWindow *m_ui, QScrollArea *scrollArea, QWidget *contentWidget);

    // Returns a map of account and it's current value
    QVariantMap getAllValueMap() const;

private slots:
    void reformatValueOnFinish();

private:
    // Stores the QLineEdit pointer and its original value
    QHash<QLineEdit*, QString> m_accountValueInputs; 
    QSet<QString> m_activeKeys;

    // Create block
    QWidget* createBasicBlock(const QString &key, QString value); // Basic block
    QWidget* createDeletableBlock(const QString &key, QString value); // Deletable block
};

#endif // BLOCKMANAGER_H