#ifndef RESUME_H
#define RESUME_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class resume;
}

class resume : public QDialog
{
    Q_OBJECT

public:
    explicit resume(QWidget *parent = nullptr);
    ~resume();

    QString getOption() {return option;}

private slots:
    void on_radioButton_Prep_toggled(bool checked);

    void on_radioButton_Check_toggled(bool checked);

    void on_radioButton_Dl_toggled(bool checked);

    void on_radioButton_Flash_toggled(bool checked);

    void on_radioButton_Cleanup_toggled(bool checked);

    void on_pushButton_Confirm_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::resume *ui;

    QString option;
};

#endif // RESUME_H
