#include "blockmanager.h"

#include <QScrollArea>
#include <QVBoxLayout>

BlockManager::BlockManager(QObject *parent)
    : QFrame(static_cast<QWidget*>(parent))
{
}

// Creates visual blocks for each item in the variant map from Json
void BlockManager::createAccountBlocks(const QVariantMap &variantMap, QStringList desiredOrder, Ui::MainWindow *m_ui)
{
    QScrollArea *scrollArea = m_ui->scrollable_apur;

    if (!scrollArea) {
        qCritical() << "Error: QScrollArea 'scrollable_apurs' not found in UI file.";
        return;
    }

    // Area to put all account block
    QWidget *contentWidget = m_ui->apur_scrollAreaWidgetContents;

    // Clear the list of input fields and their initial values before creating new ones
    m_accountValueInputs.clear(); 

    // Check if the content widget exists and has a layout. If not, create a new layout
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(contentWidget->layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(contentWidget);
        mainLayout->setSpacing(10);
        mainLayout->setAlignment(Qt::AlignTop);
    } else {
        // Clear existing widgets in the layout before adding new ones
        QLayoutItem *item;
        while ((item = mainLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    }

    contentWidget->setStyleSheet("QWidget#apur_scrollAreaWidgetContents {background-color: transparent; border: none;}");

    for (const QString &key : desiredOrder)
    {
        if (variantMap.contains(key))
        {
            const QVariant value = variantMap.value(key);

            // Check if it can be converted to a double
            if (value.canConvert<double>()) {
                double doubleValue = value.toDouble();

                // Format the value to 2 decimal places
                QString formattedValue = QString::asprintf("%.02f", doubleValue);

                QWidget *block = createBasicBlock(key, formattedValue);
                mainLayout->addWidget(block);
            } else {
                qWarning() << "Key" << key << "has a non-numeric value or could not be converted, skipping.";
            }
        } else {
            qWarning() << "Key" << key << "from desiredOrder not found in variantMap, skipping.";
        }
    }

    mainLayout->addStretch();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(contentWidget);
}

// Create one block
QWidget* BlockManager::createBasicBlock(const QString &key, QString value)
{
    // Create a container widget for the block
    QFrame *blockFrame = new QFrame;
    blockFrame->setFrameShape(QFrame::StyledPanel);
    blockFrame->setLineWidth(1);
    blockFrame->setMidLineWidth(0);
    blockFrame->setStyleSheet("QFrame {background-color: #262626; border: 1px solid #e6e6e6; border-radius: 3px;}");

    // Horizontal layout for the key and value
    QHBoxLayout *blockLayout = new QHBoxLayout(blockFrame);

    // Display account name on the left
    QLabel *keyLabel = new QLabel(key);
    keyLabel->setFont(QFont("Arial", 16, QFont::Bold));
    keyLabel->setStyleSheet("QLabel {background-color: #262626; color: #E2E8F0; border: none; padding-left: 10px;}");

    // Display account amount on the right in a QLineEdit
    QLineEdit *valueEdit = new QLineEdit;
    valueEdit->setObjectName(key);
    valueEdit->setText(value);
    valueEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Check for 2 decimal places input
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(
        QRegularExpression("^\\d*(\\.\\d{0,2})?$"), valueEdit
    );
    valueEdit->setValidator(validator);

    valueEdit->setStyleSheet(
        "QLineEdit {background-color: #181818; color: #E2E8F0; border: none; padding: 5px;"
                    "min-width: 120px; border-radius: 2px; font-size: 16px;}"
    );
    // Connect the QLineEdit to the BlockManager's internal slot for change detection
    connect(valueEdit, &QLineEdit::textEdited, this, &BlockManager::checkForValueChanges);

    // Reformat the text when the user is finished editing
    connect(valueEdit, &QLineEdit::editingFinished, this, &BlockManager::reformatValueOnFinish);

    // Store the initial value in the map
    m_accountValueInputs.insert(valueEdit, value);

    // Add widgets to the layout
    blockLayout->addWidget(keyLabel, 1);     // keyLabel takes a larger stretch factor
    blockLayout->addWidget(valueEdit, 0);    // valueEdit takes minimal space

    // Set an appropriate minimum height for the block
    blockFrame->setMinimumHeight(60);
    // Ensure the block takes up the full width
    blockFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    return blockFrame;
}

// Returns true if any QLineEdit's current value is different from its stored initial value
bool BlockManager::hasBlockValuesChanged() const
{
    for (auto it = m_accountValueInputs.constBegin(); it != m_accountValueInputs.constEnd(); ++it)
    {
        QLineEdit *lineEdit = it.key();
        const QString &initialValue = it.value();
        
        // Compare the current text with the stored initial value
        if (lineEdit->text() != initialValue)
        {
            return true;
        }
    }
    
    return false;
}

// Slot to catch the QLineEdit::textEdited signal
void BlockManager::checkForValueChanges()
{
    // A change has occurred in one of the blocks. 
    emit blockValueChanged(); 
}

// Returns a map of account and it's current value
QVariantMap BlockManager::getEditedValueMap() const
{
    QVariantMap editedMap;

    for (auto it = m_accountValueInputs.constBegin(); it != m_accountValueInputs.constEnd(); ++it)
    {
        QLineEdit *lineEdit = it.key();
        const QString &initialValue = it.value();

        // Ensure the QLineEdit has a unique objectName set
        QString keyName = lineEdit->objectName();
        if (keyName.isEmpty()) {
            qWarning() << "ERROR: Emtpy keyName found!";
            continue; 
        }

        // Compare the current text with the stored initial value
        if (lineEdit->text() != initialValue)
        {
            editedMap.insert(keyName, lineEdit->text());
        }
    }

    return editedMap;
}

// Update initial value
void BlockManager::updateCurrentValue()
{
    for (auto it = m_accountValueInputs.begin(); it != m_accountValueInputs.end(); ++it)
    {
        QLineEdit *lineEdit = it.key();
        it.value() = lineEdit->text();
    }
}

// Slot to reformat the QLineEdit value when editing is finished
void BlockManager::reformatValueOnFinish()
{
    QLineEdit *valueEdit = qobject_cast<QLineEdit*>(sender());
    if (!valueEdit)
        return;

    // Get the current text and attempt conversion
    QString currentText = valueEdit->text();
    bool ok;
    double value = QLocale().toDouble(currentText, &ok);
    
    // Check the initial stored value for the fallback
    QString initialValue = m_accountValueInputs.value(valueEdit);

    if (ok) {        
        // Format the double with 2 decimal places
        QString formattedValue = QString::asprintf("%.02f", value);

        // Set the formatted text back to the QLineEdit
        valueEdit->setText(formattedValue);
        
    } else {        
        // Reset the QLineEdit text to its last known good value
        if (!initialValue.isNull() && !initialValue.isEmpty()) {
            valueEdit->setText(initialValue);
        } else {
            valueEdit->setText("0.00");
        }
    }
}