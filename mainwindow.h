#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include "fthread.h"
#include "resume.h"
#include "flashlog.h"
#include <iostream>
#include <QMainWindow>
#include <QStandardPaths>
#include <QDir>
#include <QRadioButton>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QScrollBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Next_clicked();

    void setInfoLabelText(QString text);

    void setDlProgText(QString text);

    void msgBoxThread(QString title, QString text, int exec);

    void on_pushButton_Close_clicked();

    void on_pushButton_Resume_clicked();

    void switchScenes(int scene);
    void deleteScenes(int scene);

    void progressBar(int percentage);

    void on_pushButton_Undetected_clicked();

    void iROM_text_changed();

    void rLink_toggled() {iROM->setEnabled(true); extract->setEnabled(false); folder->setEnabled(false); getLink->setEnabled(true); extraType("link");}
    void rExtract_toggled() {iROM->setEnabled(false); extract->setEnabled(true); folder->setEnabled(true); getLink->setEnabled(false); extraType("extract");}
    void extraType(QString extra);

    void getLink_clicked();

    void extract_clicked();
    void folder_clicked();

    void updateFlash(QString content);
    void cancelFlash(QString content);

    void on_pushButton_RetryFlash_clicked();

    void on_pushButton_ARB_clicked();

private:
    Ui::MainWindow *ui;

    fThread fT;
    resume startFrom;
    flashLog log;

    QLabel *lText;
    QLabel *lProg;

    QLabel *spacer;

    QRadioButton *rLink;
    QRadioButton *rExtract;
    QPushButton *getLink;
    QPushButton *extract;
    QPushButton *folder;
    QLineEdit *iROM;
    QPlainTextEdit *flashOut;
};
#endif // MAINWINDOW_H
