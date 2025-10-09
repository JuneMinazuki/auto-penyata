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

    // Check for value changes
    bool hasBlockValuesChanged() const;

    // Returns a map of account and it's current value
    QVariantMap getEditedValueMap() const;

    // Update intial value
    void updateCurrentValue();

signals:
    // Notify Main Window that a block value has changed
    void blockValueChanged();

private slots:
    void checkForValueChanges();
    void reformatValueOnFinish();

private:
    // Stores the QLineEdit pointer and its original value
    QHash<QLineEdit*, QString> m_accountValueInputs; 

    // Create each block
    QWidget* createBasicBlock(const QString &key, QString value);
};

#endif // BLOCKMANAGER_H