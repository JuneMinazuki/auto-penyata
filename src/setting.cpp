#include "setting.h"
#include "jsonmanager.h"

void Setting::handleSettingActivation(Ui::MainWindow *m_ui)
{
    ui = m_ui;

    qDebug() << "Switched to page: Setting";
    loadJson();
    
    ui->label_saved_status->setVisible(false); // Hide save label
    ui->label_reset_status->setVisible(false); // Hide reset label

    // Disconnect any previous connection
    disconnect(ui->button_reset, &QPushButton::clicked,
                nullptr, nullptr);
    
    // Reset button
    connect(ui->button_reset, &QPushButton::clicked, 
            this, [this]() {this->handleResetButtonClick(ui->label_reset_status);
    });
}

// Load from JSON
void Setting::loadJson()
{
    QVariantMap settings = JsonManager::readJson("setting.json");

    if (settings.isEmpty()) {
        qWarning() << "setting.json is empty. Cannot load value";
        return;
    }

    // Find "CompanyName"
    if (settings.contains("CompanyName")) {
        QString companyName = settings["CompanyName"].toString();

        ui->input_company_name->setText(companyName);
    } else {
        qWarning() << "Key 'CompanyName' not found in setting.json.";
        ui->input_company_name->setText(""); 
    }

    // Find "Date"
    if (settings.contains("Date")) {
        QString dateString = settings["Date"].toString();
        QDate date = QDate::fromString(dateString, "dd/MM/yyyy");

        // Check if the conversion was successful
        if (date.isValid()) {
            ui->input_date->setDate(date);
        } else {
            qWarning() << "Date string '" << dateString << "' is not in the expected format (DD/MM/YYYY).";
            ui->input_date->setDate(QDate::currentDate());
        }
    } else {
        qWarning() << "Key 'Date' not found in settings.";
        ui->input_date->setDate(QDate::currentDate()); 
    }
}

// When user press Reset button
void Setting::handleResetButtonClick(QLabel *label_reset)
{
    // Reset JSON
    bool success = JsonManager::resetJSON();

    // Update the label based on the return value
    if (success)
    {
        label_reset->setText("Reset Successful!");
        label_reset->setStyleSheet("QLabel { color : #37ba1e }");
    }
    else
    {
        label_reset->setText("Reset Failed!");
        label_reset->setStyleSheet("QLabel { color : #e21717 }");
    }

    label_reset->setVisible(true);
    loadJson();
}