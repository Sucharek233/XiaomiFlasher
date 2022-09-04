#ifndef FTHREAD_H
#define FTHREAD_H

#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTcpSocket>

class fThread : public QThread
{
   Q_OBJECT

public:
    QString getDir() {return dir;}

    void process(QString app, QStringList command, QString path);

    QString getCodename() {return codename;}

    void flashProgress(QStringList command, QString file);

    void ARB();

public slots:
    void stopRunning();

    void setPEStat(QString stat) {status = stat;};

    void switchFunctions(int funct) {function = funct;};

    void setFlashOption(int opt) {option = opt;};

    void setDriverArch(QString architecture) {arch = architecture;}

    void setROMType(QString ROM) {romType = ROM;}
    void setROMLink(QString ROM) {romLink = ROM;}

protected:
   virtual void run();

signals:
   void update(QString);

   void msgBox(QString, QString, int);

   void progBar(int);

   void flashProg(QString);

private:
    bool isRunning;

    QString dir;

    QString status;

    int function;

    int option;

    QString arch = "x64";

    QString romLink;
    QString romType = "link";

    QString codename;
};

#endif // FTHREAD_H
