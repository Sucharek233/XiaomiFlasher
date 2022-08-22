#include "resume.h"
#include "ui_resume.h"

resume::resume(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resume)
{
    ui->setupUi(this);
}

resume::~resume()
{
    delete ui;
}

void resume::on_radioButton_Prep_toggled(bool checked)
{
    if (checked == true) {
        option = "prep";
    }
}

void resume::on_radioButton_Check_toggled(bool checked)
{
    if (checked == true) {
        option = "check";
    }
}

void resume::on_radioButton_Dl_toggled(bool checked)
{
    if (checked == true) {
        option = "dl";
    }
}

void resume::on_radioButton_Flash_toggled(bool checked)
{
    if (checked == true) {
        option = "flash";
    }
}

void resume::on_radioButton_Cleanup_toggled(bool checked)
{
    if (checked == true) {
        option = "cleanup";
    }
}

void resume::on_pushButton_Confirm_clicked()
{
    if (option != "") {
        accept();
        close();
    } else {
        QMessageBox noSel;
        noSel.setStyleSheet("QMessageBox QLabel {font-size: 20px;} "
                            "QMessageBox QPushButton {font-size: 16px;}");
        noSel.setWindowTitle("Not selected");
        noSel.setText("You haven't selected an option.\n"
                      "Please select an option and click Confirm.\n\n"
                      "If you opened this by accident, please click on Cancel or close this window.");
        noSel.exec();
    }
}

void resume::on_pushButton_Cancel_clicked()
{
    reject();
    close();
}
