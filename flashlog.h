#ifndef FLASHLOG_H
#define FLASHLOG_H

#include <QDialog>
#include <QScrollBar>
#include <QProcess>
#include <QFile>
#include <QFileDialog>

namespace Ui {
class flashLog;
}

class flashLog : public QDialog
{
    Q_OBJECT

public:
    explicit flashLog(QWidget *parent = nullptr);
    ~flashLog();

    void updateLog(QString content);

    void updateButton();

signals:
    void cancel(QString);

private slots:
    void on_pushButton_Save_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::flashLog *ui;
};

#endif // FLASHLOG_H
