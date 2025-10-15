#include "blockmanager.h"

#include <QScrollArea>
#include <QVBoxLayout>

BlockManager::BlockManager(QObject *parent)
    : QFrame(static_cast<QWidget*>(parent))
{
}

// Creates visual blocks for each item in the variant map from Json
void BlockManager::createAccountBlocks(const QVariantMap &variantMap, QStringList desiredOrder,
                                       Ui::MainWindow *m_ui, QScrollArea *scrollArea, QWidget *contentWidget)
{
    if (!scrollArea) {
        qCritical() << "Error: QScrollArea not found in UI file.";
        return;
    }

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

    QSet<QString> processedKeys;
    for (const QString &key : desiredOrder)
    {
        if (variantMap.contains(key))
        {
            processedKeys.insert(key);
            const QVariant value = variantMap.value(key);

            // Check if it can be converted to a double
            if (value.canConvert<double>()) {
                double doubleValue = value.toDouble();

                // Format the value to 2 decimal places
                QString formattedValue = QString::asprintf("%.02f", doubleValue);

                // Use the standard block creation
                QWidget *block = createBasicBlock(key, formattedValue);
                mainLayout->addWidget(block);
            } else {
                qWarning() << "Key" << key << "has a non-numeric value or could not be converted, skipping.";
            }
        }
    }

    for (auto it = variantMap.constBegin(); it != variantMap.constEnd(); ++it) {
        const QString& key = it.key();
        const QVariant& value = it.value();

        if (key == "_placeholder") {
            continue;
        }

        // If the key hasn't been processed in the desiredOrder loop
        if (!processedKeys.contains(key))
        {
            if (value.canConvert<double>()) {
                double doubleValue = value.toDouble();
                QString formattedValue = QString::asprintf("%.02f", doubleValue);

                // Use the new block creation for deletable items
                QWidget *block = createDeletableBlock(key, formattedValue); 
                mainLayout->addWidget(block);
            } else {
                qWarning() << "Key" << key << "has a non-numeric value or could not be converted, skipping.";
            }
        }
    }

    mainLayout->addStretch();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(contentWidget);
}

// Create basic block
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

// Create block with a delete button
QWidget* BlockManager::createDeletableBlock(const QString &key, QString value)
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
    QLineEdit *keyEdit = new QLineEdit;
    keyEdit->setText(key);
    keyEdit->setFont(QFont("Arial", 16, QFont::Bold));
    keyEdit->setStyleSheet(
        "QLineEdit {background-color: #181818; color: #E2E8F0; border: none; padding: 5px;"
                    "min-width: 120px; border-radius: 2px; font-size: 16px;}"
    );

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

    // Reformat the text when the user is finished editing
    connect(valueEdit, &QLineEdit::editingFinished, this, &BlockManager::reformatValueOnFinish);

    // Check for duplicate keys
    connect(keyEdit, &QLineEdit::editingFinished, this, [this, keyEdit, valueEdit]() {
        QString oldKey = valueEdit->objectName();
        QString newKey = keyEdit->text();

        // Skip if key didn't change
        if (oldKey == newKey) {
            return;
        }

        // Check if the new key is a duplicate of another existing key
        if (m_activeKeys.contains(newKey)) {
            qWarning() << "Key " << newKey << " already exists";
            keyEdit->setText(oldKey);
        } 
        else {
            // Update the key in the tracking set
            m_activeKeys.remove(oldKey);
            m_activeKeys.insert(newKey);
            
            // Update m_accountValueInputs
            valueEdit->setObjectName(newKey);
        }
    });

    // Store the initial value in the map
    m_accountValueInputs.insert(valueEdit, value);

    // Delete button
    QPushButton *deleteButton = new QPushButton("X", blockFrame);
    deleteButton->setFixedSize(30, 30);
    deleteButton->setFont(QFont("Arial", 14, QFont::Bold));
    deleteButton->setStyleSheet("QPushButton {background-color: #550404; color: #e6e6e6; border: 1px solid #cccccc; border-radius: 5px;}"
                                "QPushButton:hover {background-color: #990505;}"
                                "QPushButton:pressed {background-color: #4c0808;}"
    );
    
    // Connect the button to a slot to delete the block
    connect(deleteButton, &QPushButton::clicked, this, [blockFrame, valueEdit, this]() {
        // Remove the QLineEdit from the map
        m_accountValueInputs.remove(valueEdit);
        
        // Delete the block widget
        blockFrame->deleteLater();

        qDebug() << "Removed block for key:" << valueEdit->objectName();;
    });

    // Add widgets to the layout
    blockLayout->addWidget(keyEdit, 1);
    blockLayout->addWidget(deleteButton, 0);
    blockLayout->addWidget(valueEdit, 0);  

    // Set an appropriate minimum height for the block
    blockFrame->setMinimumHeight(60);
    // Ensure the block takes up the full width
    blockFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    return blockFrame;
}

// Returns a map of account and it's current value
QVariantMap BlockManager::getAllValueMap() const
{
    QVariantMap allValuesMap;

    for (auto it = m_accountValueInputs.constBegin(); it != m_accountValueInputs.constEnd(); ++it)
    {
        QLineEdit *lineEdit = it.key();

        // Ensure the QLineEdit has a unique objectName set
        QString keyName = lineEdit->objectName();
        if (keyName.isEmpty()) {
            qWarning() << "ERROR: Empty keyName found! Skipping entry.";
            continue; 
        }

        // Insert the current text of the QLineEdit into the result map.
        allValuesMap.insert(keyName, lineEdit->text());
    }

    return allValuesMap;
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