#include "flashlog.h"
#include "ui_flashlog.h"

flashLog::flashLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::flashLog)
{
    ui->setupUi(this);
}

void flashLog::updateLog(QString content)
{
    if (content == "finished") {
        QString finish = ui->log->toPlainText() + "\n\nFlash finished.";
        ui->log->setPlainText(finish);
        ui->log->verticalScrollBar()->setValue(ui->log->verticalScrollBar()->maximum());
        ui->pushButton_Cancel->setEnabled(false);
    } else if (content == "error") {
        QString error = ui->log->toPlainText() + "\n\nFlash failed.";
        ui->log->setPlainText(error);
        ui->log->verticalScrollBar()->setValue(ui->log->verticalScrollBar()->maximum());
        ui->pushButton_Cancel->setEnabled(false);
    } else if (content == "clear") {
        ui->log->setPlainText("");
    } else {
        ui->log->setPlainText(content);
        ui->log->verticalScrollBar()->setValue(ui->log->verticalScrollBar()->maximum());
    }
}

void flashLog::updateButton()
{
    ui->pushButton_Cancel->setEnabled(true);
}

flashLog::~flashLog()
{
    delete ui;
}

void flashLog::on_pushButton_Save_clicked()
{
    QString content = ui->log->toPlainText();
    QString path = QFileDialog::getSaveFileName(this, tr("Save log"), "", tr("Log files (*.txt *.log)"));
    QFile write(path); write.open(QIODevice::ReadWrite); write.resize(0);
    write.write(content.toUtf8());
    write.close();
}

void flashLog::on_pushButton_Cancel_clicked()
{
    QProcess kill;
    kill.startDetached("cmd", QStringList() << "/C" << "taskkill" << "/IM" << "fastboot.exe" << "/F");
    kill.startDetached("cmd", QStringList() << "/C" << "taskkill" << "/IM" << "cmd.exe" << "/F");
    QString end = ui->log->toPlainText() + "\n\n\nFlash canceled.";
    ui->log->setPlainText(end);
    ui->log->verticalScrollBar()->setValue(ui->log->verticalScrollBar()->maximum());
    ui->pushButton_Cancel->setEnabled(false);
    cancel(ui->log->toPlainText());
}

void flashLog::on_pushButton_Close_clicked()
{
    close();
}
