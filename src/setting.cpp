#include "setting.h"
#include "jsonmanager.h"

void Setting::handleSettingActivation(Ui::MainWindow *ui)
{
    qDebug() << "Handling setting page.";
    
    ui->label_saved_status->setVisible(false); // Hide save label
    ui->label_reset_status->setVisible(false); // Hide reset label

    // Disconnect any previous connection
    disconnect(ui->button_reset, &QPushButton::clicked,
                nullptr, nullptr);
    
    // Reset button
    connect(ui->button_reset, &QPushButton::clicked, 
            this, [this, ui]() {this->handleResetButtonClick(ui->label_reset_status);
    });
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
}