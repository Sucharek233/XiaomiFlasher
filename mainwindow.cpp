#include "mainwindow.h"
#include "ui_mainwindow.h"

int nextScene = 0;
int delScene = -1;

QString stylesheet;
QString msgBoxStylesheet;

QString dir;

QProgressBar* progBar;
int crashStop;
int createBar;

QString type = "link";
QString link;

void MainWindow::setInfoLabelText(QString text)
{
    QSizePolicy labelSize;
    QFont size; size.setPointSize(14);

    lText = new QLabel;
    lText->setText(text);
    lText->setAlignment(Qt::AlignTop);
    lText->setFont(size);
    ui->gridLayout_Content->addWidget(lText, 0, 0);

    //additional setup
    lProg = new QLabel;
    lProg->setSizePolicy(labelSize);
    lProg->setAlignment(Qt::AlignTop);
    lProg->setFont(size);

    spacer = new QLabel;
    labelSize.setHorizontalPolicy(QSizePolicy::Expanding);
    labelSize.setVerticalPolicy(QSizePolicy::Expanding);
    spacer->setSizePolicy(labelSize);
}

void MainWindow::switchScenes(int scene)
{
    if (scene == 1) {
        ui->pushButton_Next->setEnabled(false);
        ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
        ui->label_Prep->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Preparation");
        setInfoLabelText("Now I will begin the process of installing the\n"
                         "necessary (pre-downloaded) programs to do security\n"
                         "checks to make sure you're ready for the flash.\n"
                         "This shouldn't take long.\n\n\n\n");
        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        ui->gridLayout_Content->addWidget(spacer, 2, 0);
        fT.switchFunctions(1);
        fT.start();
    } else if (scene == 2) {
        ui->label_Prep->setStyleSheet(stylesheet + "color: green;");
        ui->label_Check->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Check");
        setInfoLabelText("The check will now begin.\n"
                         "Please connect your phone in fastboot mode.\n"
                         "You can do that by holding\n"
                         "power and volume down buttons.\n"
                         "After you done that, plug in your phone and\n"
                         "click the Next button.\n\n");
    } else if (scene == 3) {
        ui->pushButton_Next->setEnabled(false);
        ui->pushButton_Undetected->setVisible(true);
        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        ui->gridLayout_Content->addWidget(spacer, 2, 0);
        fT.switchFunctions(2);
        fT.start();
    } else if (scene == 4) {
        ui->pushButton_Undetected->setVisible(false);
        ui->pushButton_Next->setEnabled(false);
        ui->label_Check->setStyleSheet(stylesheet + "color: green;");
        ui->label_Download->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("ROM download");
        setInfoLabelText("Now the ROM will download.\n"
                         "You can restart your device by holding the power button.\n"
                         "After it downloads, please put your phone back\n"
                         "in fastboot and click on the Next button.\n\n");
        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        ui->gridLayout_Content->addWidget(rLink, 2, 0);
        ui->gridLayout_Content->addWidget(iROM, 3, 0);
        ui->gridLayout_Content->addWidget(getLink, 4, 0);
        ui->gridLayout_Content->addWidget(rExtract, 5, 0);
        ui->gridLayout_Content->addWidget(extract, 6, 0);
        ui->gridLayout_Content->addWidget(folder, 7, 0);
        ui->gridLayout_Content->addWidget(spacer, 8, 0);
    } else if (scene == 5) {
        fT.setROMType(type);
        qDebug() << type;
        QString codename = fT.getCodename();
        if (!link.contains(codename)) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Link/File/Folder check");
            msgBox.setText("Please make sure this is the correct link.\n"
                           "Codename (" + codename + ") not found in the link/file/folder.\n\n"
                           "Current link/path:\n" +
                           link);
            msgBox.setStyleSheet(msgBoxStylesheet);
            QPushButton* check = msgBox.addButton("Check", QMessageBox::YesRole);
            QPushButton* confirm = msgBox.addButton("Confirm", QMessageBox::NoRole);
            msgBox.exec();
            if (msgBox.clickedButton() == check) {
                nextScene -= 1;
                delScene -= 1;
            } else if (msgBox.clickedButton() == confirm) {
                fT.setROMLink(link);
                ui->pushButton_Next->click();
            }
        } else {ui->pushButton_Next->click(); fT.setROMLink(link);}
    } else if (scene == 6) {
        ui->pushButton_Next->setEnabled(false);
        fT.switchFunctions(3);
        fT.start();
    } else if (scene == 7) {
        ui->label_Download->setStyleSheet(stylesheet + "color: green;");
        ui->label_Flash->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Flash");
        setInfoLabelText("Finally, you are ready to flash.\n"
                         "Before I'll start flashing, there will be\n"
                         "one last final check, just to make sure everything\n"
                         "is ready to go.\n"
                         "Make sure your phone is plugged in, and click on the\n"
                         "next button.\n\n");
    } else if (scene == 8) {
        ui->pushButton_Next->setEnabled(false);

        QMessageBox flashType;
        flashType.setStyleSheet(msgBoxStylesheet);
        flashType.setWindowTitle("Select flash type");
        flashType.setText("Please choose one of the flash types:");
        QAbstractButton* ca = flashType.addButton("Clean all", QMessageBox::YesRole);
        flashType.addButton("Save user data", QMessageBox::NoRole);
        flashType.exec();
        if (flashType.clickedButton() == ca) {fT.setFlashOption(1);
        } else {fT.setFlashOption(2);}

        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        ui->gridLayout_Content->addWidget(spacer, 2, 0);
        fT.switchFunctions(4);
        fT.start();

        log.setModal(false);
        log.exec();
    } else if (scene == 9) {
        ui->pushButton_Next->setVisible(false);
        ui->pushButton_RetryFlash->setVisible(false);
        ui->pushButton_ARB->setVisible(false);
        ui->label_Flash->setStyleSheet(stylesheet + "color: green;");
        ui->label_Cleanup->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Temporary files cleanup");
        setInfoLabelText("Files used for the flash will now delete.\n"
                         "After it's done, please close this window.\n\n");
        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        ui->gridLayout_Content->addWidget(spacer, 2, 0);
        fT.switchFunctions(5);
        fT.start();
    }
}
void MainWindow::deleteScenes(int scene)
{
    if (scene == 0) {
        delete lText;
        ui->pushButton_Resume->setVisible(false);
        delete spacer;
    } else if (scene == 1) {
        delete lText;
        delete lProg;
        delete spacer;
    } else if (scene == 3) {
        delete lText;
        delete lProg;
        delete spacer;
    } else if (scene == 5) {
        delete iROM;
        delete getLink;
        delete rLink;
        delete rExtract;
        delete extract;
        delete folder;
    } else if (scene == 6) {
        delete lText;
        delete lProg;
        delete progBar;
        delete spacer;
    } else if (scene == 8) {
        delete lText;
        delete lProg;
        delete spacer;
    }
}

void MainWindow::setDlProgText(QString text)
{
    if (text == "enable") {
        ui->pushButton_Next->setEnabled(true);
    } else if (text == "green") {
        ui->label_Cleanup->setStyleSheet(stylesheet + "color: green;");
    } else if (text == "close") {
        ui->pushButton_Next->setVisible(false);
        ui->pushButton_Close->setVisible(true);
    } else if (text == "flashFinish") {
        log.updateLog("finished");
        ui->pushButton_Close->setVisible(true);
        ui->pushButton_Next->setEnabled(true);
        ui->pushButton_Next->setText("Clean temporary files");
    } else if (text == "flashError") {
        log.updateLog("error");
        ui->pushButton_Close->setVisible(true);
        ui->pushButton_Next->setEnabled(true);
        ui->pushButton_Next->setText("Clean temporary files");
        lProg->setText("Flash failed!\n"
                       "You can retry, ignore anti-rollback (ARB),\n"
                       "clean up temporary files or close.\n\n"
                       "WARNING! Ignoring ARB may lead to hard bricks!\n"
                       "I'm not responsible for bricked devices\nand hardware damages!");
        ui->pushButton_RetryFlash->setVisible(true);
        ui->pushButton_ARB->setVisible(true);
    } else {
        lProg->setText(text);
    }
}

void MainWindow::msgBoxThread(QString title, QString text, int exec)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setStyleSheet(msgBoxStylesheet);
    if (exec == 1) {msgBox.exec();}
    if (title == "Driver install error") {
        text += "Please click Retry to retry the installation.<br>"
                "If you already have the drivers installed, click Ignore.<br>"
                "If you have a 32-bit system, click Install x86 drivers.<br>"
                "If this message keeps showing up, click Open driver folder and try opening both .exe files.<br><br>"
                "If none of these worked for you, please report the error code "
                "on my <a href=\"https://github.com/Sucharek233/Redmi6-6AFlashers/issues\">Github repository</a> (open an issue there).";
        msgBox.setText(text);
        msgBox.setTextFormat(Qt::RichText);
        QPushButton* retry = msgBox.addButton("Retry", QMessageBox::YesRole);
        QPushButton* x86 = msgBox.addButton("Install x86 drivers", QMessageBox::YesRole);
        QPushButton* open = msgBox.addButton("Open driver folder", QMessageBox::YesRole);
        QPushButton* ignore = msgBox.addButton("Ignore", QMessageBox::NoRole);
        msgBox.exec();
        if (msgBox.clickedButton() == retry) {
            fT.setDriverArch("x64");
            fT.start();
        } else if (msgBox.clickedButton() == ignore) {
            setDlProgText("Driver installation failed, ignored.\n"
                          "Please click on the Next button to continue.");
            ui->pushButton_Next->setEnabled(true);
        } else if (msgBox.clickedButton() == x86) {
            fT.setDriverArch("x86");
            fT.start();
        } else if (msgBox.clickedButton() == open) {
            QString dir = fT.getDir() + "drivers"; dir.replace("/", "\\");
            system("explorer " + dir.toUtf8());
            QMessageBox msgBox;
            msgBox.setWindowTitle("Installing drivers on your own");
            msgBox.setText("An explorer window should be now opened on your screen.<br>"
                           "Please open \"drivers_x64.exe\" or \"drivers_x86.exe\" to install these drivers.<br>"
                           "If you can't install any of these, please open an issue on my "
                           "<a href=\"https://github.com/Sucharek233/Redmi6-6AFlashers/issues\">Github repository</a> "
                           "and tell me, what is the installer doing, if you want, record a video or take screenshots of it.");
            msgBox.setStyleSheet(msgBoxStylesheet);
            msgBox.setTextFormat(Qt::RichText);
            QPushButton* success = msgBox.addButton("Installation succeeded", QMessageBox::YesRole);
            QPushButton* fail = msgBox.addButton("Installation failed", QMessageBox::NoRole);
            msgBox.exec();
            if (msgBox.clickedButton() == success) {
                setDlProgText("Finished\n\n"
                              "Please proceed to the next step, where your\n"
                              "phone will be checked for any mismatches.");
                ui->pushButton_Next->setEnabled(true);
            } else if (msgBox.clickedButton() == fail) {
                setDlProgText("Drivers not installed, cannot continue.");
                ui->pushButton_Next->setVisible(false);
                ui->pushButton_Close->setVisible(true);
            }
        }
    } else if (title == "Could not detect device") {
        QPushButton* retry = msgBox.addButton("Retry", QMessageBox::YesRole);
        QPushButton* cancel = msgBox.addButton("Cancel", QMessageBox::NoRole);
        msgBox.setText("Device cannot be detected\n"
                       "Before you continue reading this, please click on the Retry button and then click on the \"Device not being detected?\" button (if you haven't already).\n"
                       "Please make sure your phone is in fastboot mode and connected into your PC.\n\n"
                       "If this issue persists, here's a list of things you can try:\n"
                       "Restarting your computer.\n"
                       "Using a different cable.\n"
                       "Trying a different port on your PC or laptop.\n"
                       "Using a different PC or a laptop.");
        msgBox.exec();
        if (msgBox.clickedButton() == retry) {
            fT.start();
        } else if (msgBox.clickedButton() == cancel) {
            ui->pushButton_Next->setVisible(false);
            ui->pushButton_Close->setVisible(true);
        }
    } else if (title == "No internet") {
        QPushButton* retry = msgBox.addButton("Retry", QMessageBox::YesRole);
        QPushButton* cancel = msgBox.addButton("Cancel", QMessageBox::NoRole);
        msgBox.exec();
        if (msgBox.clickedButton() == retry) {
            fT.start();
        } else if (msgBox.clickedButton() == cancel) {
            ui->pushButton_Next->setVisible(false);
            ui->pushButton_Close->setVisible(true);
        }
    } else if (title == "Different device detected") {
        QPushButton* retry = msgBox.addButton("Continue", QMessageBox::YesRole);
        QPushButton* cancel = msgBox.addButton("Cancel", QMessageBox::NoRole);
        msgBox.exec();
        if (msgBox.clickedButton() == retry) {
            fT.terminate();
            fT.start();
        } else if (msgBox.clickedButton() == cancel) {
            fT.terminate();
            ui->pushButton_Next->setVisible(false);
            ui->pushButton_Close->setVisible(true);
            setDlProgText("Serial number mismatch.\n"
                          "Cannot continue.");
        }
    }
}

void MainWindow::progressBar(int percentage)
{
    if (percentage >= 0) {
        ui->gridLayout_Content->addWidget(progBar);
        ui->gridLayout_Content->addWidget(spacer);
        progBar->setValue(percentage);
        progBar->update();
        crashStop += 1;
    }
}

void MainWindow::iROM_text_changed()
{
    link = iROM->text().simplified();
    if (link == "") {
        ui->pushButton_Next->setEnabled(false);
    } else {
        ui->pushButton_Next->setEnabled(true);
    }
}

void MainWindow::getLink_clicked()
{
    QMessageBox info;
    info.setStyleSheet(msgBoxStylesheet);
    info.setTextFormat(Qt::RichText);
    info.setWindowTitle("How do I get the download link?");
    info.setText("Here's how you get the download link:<br>⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀<br>"
                 "Go to <a href=\"https://xiaomifirmwareupdater.com/archive/miui/" + fT.getCodename() + "\">xiaomifirmwareupdater.com</a>.<br>"
                 "You should be automatically on your device's page if you clicked the link above.<br>"
                 "Choose the version you like and click on Download.<br>"
                 "2 variants of ROM types will appear. Recovery and fastboot. You need the fastboot one.<br>"
                 "Press \"Ctrl + Shift + i\" or \"F12\" on your keyboard to open inspect element.<br>"
                 "When opened, press \"Ctrl + Shift + C\" and click on the blue Full ROM download button.<br>"
                 "In inspect element, there has been selected an element.<br>"
                 "Somewhere in it should be a link. It normally starts with \"http://bigota.d.miui.com\" and ends with \"tgz.\".<br>"
                 "If it ends with \".zip\", you have selected the Recovery, not Fastboot ROM. Please check again and pick the fastboot ROM this time.<br>"
                 "Now copy that link and put it in my program.<br><br>"
                 "The ROM should start downloading. If it doesn't, you might've entered it wrong. Please rerun my program if this happens.<br><br>"
                 "If you didn't understand this guide, check out <a href=\"https://www.reddit.com/r/SuchareksGuides/comments/wv0192/detailed_rom_link_getting_guide_a_part_of_my\">this</a> one. It's more detailed with pictures.");
    info.exec();
}

void MainWindow::extract_clicked()
{
    type = "extract";
    link = QFileDialog::getOpenFileName(this, tr("Open ROM archive"), "", tr("Archives (*.tgz *.tar)")).simplified();
    if (link != "") {fT.setROMLink(link); ui->pushButton_Next->setEnabled(true); ui->pushButton_Next->click();}
}
void MainWindow::folder_clicked()
{
    type = "folder";
    link = QFileDialog::getExistingDirectory(this, tr("Open ROM folder"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (link != "") {fT.setROMLink(link); ui->pushButton_Next->setEnabled(true); ui->pushButton_Next->click();}
}
void MainWindow::extraType(QString extra)
{
    if (extra == "link") {
        type = "link";
        if (iROM->text().simplified() != "") {ui->pushButton_Next->setEnabled(true);}
    }
    if (extra == "extract") {ui->pushButton_Next->setEnabled(false);}
}

void MainWindow::updateFlash(QString content)
{
    log.updateLog(content);
}

void MainWindow::cancelFlash(QString content)
{
    fT.terminate();
    QProcess kill;
    if (!fT.isFinished()) {
        cancelFlash(content);
    } else {
        updateFlash(content);

        ui->pushButton_Close->setVisible(true);
        ui->pushButton_RetryFlash->setVisible(true);
        ui->pushButton_ARB->setVisible(true);
        ui->pushButton_Next->setEnabled(true);
        ui->pushButton_Next->setText("Clean temporary files");
        setDlProgText("Flash has been canceled.\n"
                      "You can retry, ignore anti-rollback (ARB),\n"
                      "clean up temporary files or close.\n\n"
                      "WARNING! Ignoring ARB may lead to hard bricks!\n"
                      "I'm not responsible for bricked devices\nand hardware damages!");
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_Close->setVisible(false);
    ui->pushButton_Undetected->setVisible(false);
    ui->pushButton_RetryFlash->setVisible(false);
    ui->pushButton_ARB->setVisible(false);

    QFont size; size.setPointSize(14);
    iROM = new QLineEdit; iROM->setFont(size);
    rLink = new QRadioButton; rLink->setText("Enter in a download link"); rLink->setFont(size); rLink->setChecked(true);
    rExtract = new QRadioButton; rExtract->setText("Already downloaded"); rExtract->setFont(size);
    extract = new QPushButton; extract->setText("Select ROM archive"); extract->setFont(size); extract->setEnabled(false);
    folder = new QPushButton; folder->setText("Select ROM folder"); folder->setFont(size); folder->setEnabled(false);
    getLink = new QPushButton; getLink->setText("How do I get a download link?"); getLink->setFont(size);

    setInfoLabelText("Hello, and welcome to this automatic flash setup.\n"
                     "You will be walked trough steps that involve downloading, \n"
                     "extracting, preparing and finally flashing.\n"
                     "Things such as a locked bootloader, device mismatch, etc.\n"
                     "will be told to you trough different alerts.\n\n"
                     "This app is free and open source, released on GitHub.\n\n"
                     "Anyways, let's get started.");
    ui->gridLayout_Content->addWidget(spacer);

    progBar = new QProgressBar;
    progBar->setRange(0, 100);
    progBar->setTextVisible(false);

    QObject::connect(&fT,SIGNAL(update(const QString&)),SLOT(setDlProgText(const QString&)), Qt::AutoConnection);
    QObject::connect(&fT,SIGNAL(msgBox(const QString&, const QString&, const int&)),SLOT(msgBoxThread(const QString&, const QString&, const int&)), Qt::AutoConnection);
    QObject::connect(&fT,SIGNAL(progBar(const int&)),SLOT(progressBar(const int&)), Qt::AutoConnection);
    QObject::connect(&fT,SIGNAL(flashProg(const QString&)),SLOT(updateFlash(const QString&)), Qt::AutoConnection);
    QObject::connect(&log,SIGNAL(cancel(const QString&)),SLOT(cancelFlash(const QString&)), Qt::AutoConnection);

    QObject::connect(iROM, &QLineEdit::textChanged, this, &MainWindow::iROM_text_changed);
    QObject::connect(rLink, &QRadioButton::toggled, this, &MainWindow::rLink_toggled);
    QObject::connect(rExtract, &QRadioButton::toggled, this, &MainWindow::rExtract_toggled);
    QObject::connect(extract, &QPushButton::clicked, this, &MainWindow::extract_clicked);
    QObject::connect(folder, &QPushButton::clicked, this, &MainWindow::folder_clicked);
    QObject::connect(getLink, &QPushButton::clicked, this, &MainWindow::getLink_clicked);

    stylesheet = "border: 2px solid black; ";

    msgBoxStylesheet = "QMessageBox QLabel {font-size: 20px;} "
                       "QMessageBox QPushButton {font-size: 16px;}";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Next_clicked()
{
    nextScene += 1;
    delScene += 1;
    deleteScenes(delScene);
    switchScenes(nextScene);
}

void MainWindow::on_pushButton_Close_clicked()
{
    close();
}

void MainWindow::on_pushButton_Resume_clicked()
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString mainPath = homePath.first().split(QDir::separator()).last() + "/AppData/Local/Temp/Sucharek/";
    int isGood = 0;
    QDir check(mainPath);
    if (check.exists()) {
        isGood += 1;
        check.setPath(mainPath + "platform-tools");
        if (check.exists()) {
            isGood += 1;
        }
    }
    if (isGood > 1) {
    if (startFrom.exec() == QDialog::Accepted) {
        ui->pushButton_Resume->setVisible(false);

        QString option = startFrom.getOption();
        if (option == "prep") {
            ui->pushButton_Next->click();
        } else if (option == "check") {
            ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
            nextScene += 1; delScene += 1;
            ui->pushButton_Next->click();
        } else if (option == "dl") {
            ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
            ui->label_Prep->setStyleSheet(stylesheet + "color: green;");
            ui->label_Check->setStyleSheet(stylesheet + "color: green;");
            nextScene += 3; delScene += 3;
            ui->pushButton_Next->click();
        } else if (option == "flash") {
            ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
            ui->label_Prep->setStyleSheet(stylesheet + "color: green;");
            ui->label_Check->setStyleSheet(stylesheet + "color: green;");
            ui->label_Download->setStyleSheet(stylesheet + "color: green;");
            nextScene += 4; delScene += 4;
            ui->pushButton_Next->click();
        } else if (option == "cleanup") {
            ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
            ui->label_Prep->setStyleSheet(stylesheet + "color: green;");
            ui->label_Check->setStyleSheet(stylesheet + "color: green;");
            ui->label_Download->setStyleSheet(stylesheet + "color: green;");
            nextScene += 8; delScene += 6;
            ui->pushButton_Next->click();
        }
    }
    } else {
        QMessageBox notFound;
        notFound.setWindowTitle("Cannot resume");
        notFound.setText("Directory not found.\n"
                         "Cannot load important files.\n\n"
                         "Cannot resume from action.");
        notFound.setStyleSheet(msgBoxStylesheet);
        notFound.exec();
    }
}

void MainWindow::on_pushButton_Undetected_clicked()
{
    QProcess devmgmt; devmgmt.startDetached("cmd", QStringList() << "/C" << "devmgmt.msc");

    QMessageBox fix;
    fix.setTextFormat(Qt::RichText);
    fix.setWindowTitle("Device not being detected?");
    fix.setText("This is probably a common issue.<br>"
                "There's a simple fix.<br>⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀<br>"
                "Device manager should open.<br>"
                "Now you should see \"Android\" somewhere in the list.<br>"
                "Right click on it and click on \"Update driver\"<br>"
                "Now a window should pop up with 2 options: \"Search automatically for drivers\" and \"Browse my computer for drivers.\"<br>"
                "Pick the second one (Browse my computer for drivers).<br>"
                "Now there should be an option at the bottom of the window (Let me pick from a list of availible drivers on my computer). Click on it.<br>"
                "Now you should see 3 entries. Select \"Android bootloader interface\" and click on next.<br>"
                "Now the driver should install and be detected by this tool.<br><br>"
                "If device manager didn't open, click on the Windows (Super) key on your keyboard, search for Device manager and open it.<br><br>"
                "If you didn't understand this guide, check out <a href=\"https://www.reddit.com/r/SuchareksGuides/comments/wuzize/detailed_driver_fix_guide_a_part_of_my_flashing\">this</a> one. It's more detailed with pictures.");
    fix.setStyleSheet(msgBoxStylesheet);
    fix.exec();
}

void MainWindow::on_pushButton_RetryFlash_clicked()
{
    ui->pushButton_RetryFlash->setVisible(false);
    ui->pushButton_ARB->setVisible(false);
    ui->pushButton_Close->setVisible(false);
    ui->pushButton_Next->setEnabled(false);
    ui->pushButton_Next->setText("Next");
    log.updateButton();
    fT.start();
    log.updateLog("clear");
    log.exec();
}

void MainWindow::on_pushButton_ARB_clicked()
{
    QMessageBox sure;
    sure.setStyleSheet(msgBoxStylesheet);
    QPushButton* yes = sure.addButton("Yes", QMessageBox::YesRole);
    QPushButton* no = sure.addButton("No", QMessageBox::NoRole);
    sure.setWindowTitle("Ignore ARB?");
    sure.setText("WARNING! Ignoring anti-rollback may lead to hard bricks!\n"
                 "I am not reponsible for bricked devices\nand hardware damages!\n\n"
                 "You have been warned.");
    sure.exec();
    if (sure.clickedButton() == yes) {
        fT.ARB();
        QMessageBox done;
        done.setStyleSheet(msgBoxStylesheet);
        done.setWindowTitle("ARB Ignored");
        done.setText("Anti-rollback has been ignored.");
        done.exec();
    } else if (sure.clickedButton() == no) {}
}
