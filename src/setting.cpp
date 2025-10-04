#include "setting.h"
#include "jsonmanager.h"

void Setting::handleSettingActivation(Ui::MainWindow *m_ui)
{
    ui = m_ui;

    qDebug() << "Switched to page: Setting";
    loadJson();
    
    ui->label_saved_status->setVisible(false); // Hide save label
    ui->label_reset_status->setVisible(false); // Hide reset label
    ui->button_save->setEnabled(false); // Disable save button
    
    // Button
    connect(ui->button_save, &QPushButton::clicked, this, &Setting::handleSaveButtonClick); // Save button
    connect(ui->button_reset, &QPushButton::clicked, this, &Setting::handleResetButtonClick); // Reset button
    connect(ui->button_setToday, &QPushButton::clicked, this, &Setting::handleSetTodayButtonClick); // setToday button

    // Check for value changed
    connect(ui->input_company_name, &QLineEdit::textEdited, this, &Setting::checkForChanges);
    connect(ui->input_date, &QDateEdit::editingFinished, this, &Setting::checkForChanges);
    connect(ui->input_date, &QDateEdit::dateChanged, this, &Setting::checkForChanges);
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
        initialCompanyName = settings["CompanyName"].toString();
    } else {
        qWarning() << "Key 'CompanyName' not found in setting.json.";
        initialCompanyName = ""; 
    }

    // Find "Date"
    if (settings.contains("Date")) {
        QString dateString = settings["Date"].toString();
        QDate date = QDate::fromString(dateString, "dd/MM/yyyy");

        // Check if the conversion was successful
        if (date.isValid()) {
            initialDate = date;
        } else {
            qWarning() << "Date string '" << dateString << "' is not in the expected format (DD/MM/YYYY).";
            initialDate = QDate::currentDate();
        }
    } else {
        qWarning() << "Key 'Date' not found in settings.";
        initialDate = QDate::currentDate();
    }

    // Update the form
    ui->input_company_name->setText(initialCompanyName);
    ui->input_date->setDate(initialDate);
}

// When user press Reset button
void Setting::handleSaveButtonClick()
{
    QVariantMap settingsMap{
        {"CompanyName", ui->input_company_name->text()},
        {"Date", ui->input_date->date()}
    };

    if (JsonManager::writeJson("setting.json", settingsMap)) {
        ui->label_saved_status->setText("Saved!");
        ui->label_saved_status->setStyleSheet("QLabel { color : #37ba1e }");

        // Disable save button
        initialCompanyName = ui->input_company_name->text();
        initialDate = ui->input_date->date();
        ui->button_save->setEnabled(false);
    } else {
        ui->label_saved_status->setText("Failed to save!");
        ui->label_saved_status->setStyleSheet("QLabel { color : #e21717 }");
    }

    ui->label_saved_status->setVisible(true);
}

// When user press Reset button
void Setting::handleResetButtonClick()
{
    // Reset JSON
    if (JsonManager::resetJSON())
    {
        ui->label_reset_status->setText("Reset Successful!");
        ui->label_reset_status->setStyleSheet("QLabel { color : #37ba1e }");
    }
    else
    {
        ui->label_reset_status->setText("Reset Failed!");
        ui->label_reset_status->setStyleSheet("QLabel { color : #e21717 }");
    }

    ui->label_reset_status->setVisible(true);
    loadJson();
}

// When user press setToday button
void Setting::handleSetTodayButtonClick()
{
    qDebug() << "Change date to today";
    ui->input_date->setDate(QDate::currentDate());
}

// Check for changes
void Setting::checkForChanges() 
{
    // Compare current values against initial values
    bool changed = (ui->input_company_name->text() != initialCompanyName) || (ui->input_date->date() != initialDate);

    // Enable or disable the Save button
    ui->button_save->setEnabled(changed);
}