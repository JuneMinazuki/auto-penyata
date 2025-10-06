#include "apur.h"
#include "jsonmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

void Apur::handleApurActivation(Ui::MainWindow *m_ui)
{
    ui = m_ui;

    qDebug() << "Switched to page: APUR";

    // Create accounts block
    QVariantMap jsonData = loadJson();
    createJsonBlocks(jsonData);
}

// Load from JSON
QVariantMap Apur::loadJson()
{
    QVariantMap apurs = JsonManager::readJson("apur.json");

    if (apurs.isEmpty()) {
        qWarning() << "apur.json is empty. Cannot load value";
        return QVariantMap();
    }

    return apurs;
}

// Create account blocks from Json
void Apur::createJsonBlocks(const QVariantMap &variantMap)
{
    QScrollArea *scrollArea = ui->scrollable_apur; 

    if (!scrollArea) {
        qCritical() << "Error: QScrollArea 'scrollable_apurs' not found in UI file.";
        return; 
    }

    // Area to put all account block 
    QWidget *contentWidget = ui->apur_scrollAreaWidgetContents;
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(10);
    mainLayout->setAlignment(Qt::AlignTop);

    contentWidget->setStyleSheet("QWidget#apur_scrollAreaWidgetContents {background-color: transparent; border: none;}");

    // Desired display order of keys
    QStringList desiredOrder = {
        "Jualan",
        "Pulangan Jualan",
        "Inventori Awal",
        "Belian",
        "Pulangan Belian",
        "Angkutan Masuk",
        "Upah Atas Belian",
        "Duti Import",
        "Insurans Atas Belian",
        "Inventori Akhir"
    };

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

                QWidget *block = createBlock(key, formattedValue);
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

QWidget* Apur::createBlock(const QString &key, QString value)
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

    keyLabel->setStyleSheet(
        "QLabel {background-color: #262626; color: #E2E8F0; border: none; padding-left: 10px;}"
    );

    // Display account amount on the right in a QLineEdit
    QLineEdit *valueEdit = new QLineEdit;
    valueEdit->setText(value);
    valueEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Check for 2 decimal places input
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(
        QRegularExpression("^\\d+(\\.\\d{0,2})?$"), valueEdit
    );
    valueEdit->setValidator(validator);

    valueEdit->setStyleSheet(
        "QLineEdit {background-color: #181818; color: #E2E8F0; border: none; padding: 5px;"
                    "min-width: 120px; border-radius: 2px; font-size: 16px;}"
    );

    // Add widgets to the layout
    blockLayout->addWidget(keyLabel, 1);     // keyLabel takes a larger stretch factor
    blockLayout->addWidget(valueEdit, 0);    // valueEdit takes minimal space

    // Set an appropriate minimum height for the block
    blockFrame->setMinimumHeight(60);
    // Ensure the block takes up the full width
    blockFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    return blockFrame;
}