#include "fthread.h"

QString sn;

int errStop;

QString getUserPath()
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    return homePath.first().split(QDir::separator()).last();
}

QString getvar(QString get)
{
    QProcess getVar;
    QStringList command; command << "/C" << "fastboot.exe getvar " + get;
    getVar.setWorkingDirectory(getUserPath() + "/AppData/Local/Temp/Sucharek/platform-tools");
    getVar.start("cmd", command); getVar.waitForFinished();
    QString output = getVar.readAllStandardError();
    return output.left(output.count() - 30);
}

QString getPercentage(QString path)
{
    QFile get1(path + "out.txt");
    get1.open(QIODevice::ReadOnly);
    QString contents = get1.readAll();
    get1.close();

    QString mContents = contents.replace("\r", "\n");

    QFile get2(path + "mOut.txt");
    get2.open(QIODevice::ReadWrite); get2.resize(0); get2.write(mContents.toUtf8()); get2.close();
    get2.open(QIODevice::ReadOnly);
    QString lastLine;
    while(!get2.atEnd()) {
        lastLine = get2.readLine();
    }
    get2.close();
    QString percentage = lastLine.left(4).simplified();
    if (lastLine == "\n") {percentage = "100";}

    if (percentage.contains("%")) {
        QString fix = percentage.left(percentage.count() - 1);
        percentage = fix;
    }
    return percentage;
}

void ignoreARB(QString path)
{
    QFile modify(path);
    QString content;
    modify.open(QIODevice::ReadWrite);
    int after = 0;
    while(!modify.atEnd()) {
        QString line = modify.readLine();
        if (line.startsWith("fastboot")) {content += line;}
        if (after == 1) {content += line;}
        if (line.contains("reboot")) {after = 1;}
    }
    modify.resize(0); modify.write(content.toUtf8());  modify.close();
}

void fThread::process(QString app, QStringList command, QString path)
{
    QProcess process;
    process.setWorkingDirectory(path);
    process.start(app, command);
    process.waitForFinished(18000000);
}

QString getROMFolderName(QString path)
{
    QDir search(path);
    QStringList dirs = search.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    QString name;
    foreach(QString folder, dirs) {name = folder; break;}
    return name;
}

void copyPath(QString source, QString destination)
{
    QDir dir(source);
    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = destination + QDir::separator() + d;
        dir.mkpath(dst_path);
        copyPath(source + QDir::separator() + d, dst_path);
    }
    foreach (QString f, dir.entryList(QDir::Files)) {
        QFile::copy(source + QDir::separator() + f, destination + QDir::separator() + f);
    }
}

QString readProg(QString path)
{
    QFile prog(path + "Modified.txt");
    prog.open(QIODevice::ReadOnly);
    QString content = prog.readAll();
    prog.close();
    return content;
}

void terminateFlash()
{
    QProcess kill;
    kill.start("cmd", QStringList() << "/C" << "taskkill" << "/IM" << "fastboot.exe" << "/F");
    kill.waitForFinished();
    kill.start("cmd", QStringList() << "/C" << "taskkill" << "/IM" << "cmd.exe" << "/F");
    kill.waitForFinished();
}

void modifyOut(QString path)
{
    QFile check(path + ".txt");
    check.open(QIODevice::ReadOnly);
    QString line;
    QString content;
    int replace = 0;
    while (!check.atEnd()) {
        replace = 0;
        line = check.readLine();
        if (line.contains("getvar product")) {replace = 0; if (line.contains("exit /B 1")) {replace = 1; break;}}
    }
    QFile get(path + ".txt"); get.open(QIODevice::ReadOnly); content = get.readAll(); get.close();
    if (replace == 1) {content.replace(line, "");}
    QFile write(path + "Modified.txt"); write.open(QIODevice::ReadWrite); write.resize(0); write.write(content.toUtf8()); write.close();
    check.close();

}

int mismatch(QString path)
{
    QFile check(path + ".txt");
    check.open(QIODevice::ReadOnly);
    QString line;
    int toReturn = 0;
    while (!check.atEnd()) {
        line = check.readLine().simplified();
        if (line == "Missmatching image and device") {
            toReturn = 1;
            break;
        }
    }
    return toReturn;
}

void fThread::flashProgress(QStringList command, QString file)
{
    QProcess flash;
    flash.startDetached("cmd", command);
    msleep(50);
    QString content = readProg(file);  flashProg("Please plug in your phone\n" + content);
    while (true) {
        modifyOut(file);
        if (mismatch(file) == 1) {terminateFlash(); errStop = 1; break;}
        QString newContent = "Please plug in your phone\n" + readProg(file);
        if (content != newContent) {
            flashProg(newContent);
            if (newContent.contains("pause")) {terminateFlash(); break;}
            if (newContent.contains("exit /B 1")) {
                terminateFlash(); errStop = 1; break;
            }
            content = "Please plug in your phone\n" + readProg(file);
        }
        msleep(50);
    }
    content = "Please plug in your phone\n" + readProg(file);
    flashProg(content);
}

void fThread::ARB()
{
    ignoreARB(dir + "ROM/ROM/flash_all.bat");
    ignoreARB(dir + "ROM/ROM/flash_all_except_data.bat");
    ignoreARB(dir + "ROM/ROM/flash_all_except_data_storage.bat");
}

void fThread::run()
{
    isRunning = 1;
    while(isRunning == 1)
    {
        dir = getUserPath() + "/AppData/Local/Temp/Sucharek/";
        QStringList command;
        if (function == 1) {
            QDir createFolder; createFolder.mkdir(dir);
            createFolder.mkdir(dir + "/ROM");

            QString updateText;
            updateText = "Copying files... "; emit update(updateText);
            QFile::copy(":/files/drivers.zip", dir + "drivers.zip");
            QFile::copy(":/files/platform-tools.zip", dir + "platform-tools.zip");
            QFile::copy(":/files/7z.exe", dir + "7z.exe");
            QFile::copy(":/files/curl.exe", dir + "curl.exe");

            updateText += "Done\nExtracting Platform Tools (r33.0.2)... "; emit update(updateText);
            process("cmd", QStringList() << "/C" << "7z.exe" << "x" <<  "platform-tools.zip" << "-oplatform-tools" << "-y", dir);

            updateText += "Done\nExtracting drivers... "; emit update(updateText);
            process("cmd", QStringList() << "/C" << "7z.exe" << "x" <<  "drivers.zip" << "-odrivers" << "-y", dir);
            updateText += "Done\nRetrieving codename info... "; //NOT DONE YET!!!!!!!!!!!!

            int errorCheck = 0;
            if (arch == "x64") {
                emit update("Installing drivers...");
                errorCheck = system(dir.toUtf8() + "drivers/drivers_x64.exe");
            } else if (arch == "x86") {
                emit update("Installing 32-bit (x86) drivers...");
                errorCheck = system(dir.toUtf8() + "drivers/drivers_x86.exe");
            }
            if (errorCheck == 256) {
                emit update("Finished\n\n"
                       "Please proceed to the next step, where your\n"
                       "phone will be checked for any mismatches.\n\n");
                emit update("enable");
            } else {
                emit update("Driver installation failed.");
                QString reason = "Reason: ";
                if (errorCheck == 1) {reason += "Access denied or file not found.<br>"
                                                "Solution: Retry and allow administrative rights.";}
                else if (errorCheck == -2147483648) {reason += "Installation canceled or wrong architecture.<br>"
                                                               "Solution: Don't cancel or close the driver installation window or try x86 drivers.";}
                else {reason += "Unknown reason.<br>"
                                "Solution: Unknown solution.";}
                emit msgBox("Driver install error", "Drivers failed to install!<br>" +
                                                    reason +
                                                    "<br>Error code: " + QString::number(errorCheck) + "<br><br>", 0);
            }
            stopRunning();
        } else if (function == 2) {
            QProcess fd;
            fd.setWorkingDirectory(dir + "platform-tools");
            command.clear(); command << "/C" << "fastboot.exe devices";
            for (int i = 1; i < 101; i++) {
                emit update("Detecting device... (tries: " + QString::number(i) + "/100)");
                sleep(1);
                fd.start("cmd", command); fd.waitForFinished();
                QString output = fd.readAll();
                if (output != "") {
                    sn = output.left(output.count() - 10);
                    QString textToDisplay = "Device detected\n\n"
                                            "Serial number: " + sn + "\n\n";
                    emit update(textToDisplay);

                    int enableNext = 0;

                    output = getvar("unlocked");
                    textToDisplay += output;
                    if (output == "unlocked: yes\r\n") {
                        textToDisplay += "Bootloader unlocked, PASS.\n\n"; enableNext += 1;
                    } else if (output == "unlocked: no\r\n") {
                        textToDisplay += "Bootloader locked, cannot continue, FAIL.\n"
                                         "Cannot continue.\n\n";
                        update("close");
                    } else {
                        textToDisplay += "Could not get bootloader info, continuing anyway.\n\n"; enableNext += 1;
                    }
                    emit update(textToDisplay);

                    codename = getvar("product");
                    codename = codename.right(codename.count() - 9).simplified();
                    textToDisplay += "Codename: " + codename;
                    enableNext += 1;
                    if (enableNext > 1) {emit update("enable");} else {emit update("close");}
                    emit update(textToDisplay);

                    break;
                } else {
                    if (i > 99) {
                    emit update("Could not detect device.\n"
                                "Cannot continue.");
                    emit msgBox("Could not detect device", "do the rest :)", 0);
                    }
                }
            }
            stopRunning();
        } else if (function == 3) {
            QString checkInternet;
            QString dlProg;
            QString extProg;

            int internetBreak = 0;
            if (romType == "link") {
                checkInternet = "Checking internet connection... (can take up to 15 seconds)"; emit update(checkInternet);
                QTcpSocket check;
                check.connectToHost("www.google.com", 80);
                bool connected = check.waitForConnected();
                if (connected == true) {
                    checkInternet = "Checking internet connection... Online\n"; emit update(checkInternet);
                } else {
                    checkInternet = "Checking internet connection... Offline\n"
                                    "Cannot continue.";
                    emit update(checkInternet);
                    emit msgBox("No internet", "You have no internet access.\n"
                                               "Please check your internet connection and click the Retry button.\n\n"
                                               "Here's a list of things you can try:\n"
                                               "Repluging your ethernet cable.\n"
                                               "Turning WiFi off and on.\n"
                                               "Restarting your router.\n"
                                               "Restarting your PC.", 0);
                    internetBreak = 1;
                }
            }

            if (internetBreak == 0) {
            QProcess getROMReady;
            int avoidComplete = 0;
            int extractSwitch = 1;
            int misc = 1;
            int modify = 1;

            QString checkExisting = "Checking for any older ROM files... "; emit update (checkExisting);
            QDir oldROM(dir + "ROM/ROM");
            if (oldROM.exists()) {oldROM.removeRecursively();}
            checkExisting += "Done"; emit update (checkExisting);
            sleep(1);
            if (romType == "link") {
                command.clear(); command << "/C" << dir + "curl.exe " + romLink + " --output " + dir + "ROM/ROM.tgz --insecure 2>" + dir + "ROM/out.txt";
                getROMReady.startDetached("cmd", command);
                emit progBar(-1);
                while(true) {
                    QString percentage = getPercentage(dir + "ROM/");
                    dlProg = "Downloading ROM... (" + percentage + "%)"; emit update(checkInternet + dlProg);
                    emit progBar(percentage.toInt());

                    avoidComplete += 1;
                    if (avoidComplete > 40) {if (percentage == "100") {sleep(2); break;}}

                    msleep(100);
                }
                dlProg = "ROM downloaded\n\n"; emit update(checkInternet + dlProg); emit progBar(0);
            } else if (romType == "folder") {
                extractSwitch = 0; misc = 0;
                extProg = "Copying folder contents... "; emit update(extProg);
                QDir create; create.mkpath(dir + "ROM/ROM");
                copyPath(romLink, dir + "ROM/ROM");
                extProg += "Done\n\n";
                emit update(extProg);
            } else if (romType == "extract") {
                if (romLink.endsWith(".tgz")) {
                    dlProg = "Copying ROM archive... "; emit update(dlProg);
                    QFile::copy(romLink, dir + "ROM/ROM.tgz");
                    dlProg += "Done\n"; emit update(dlProg);
                } else if (romLink.endsWith(".tar")) {
                    extractSwitch = 0;
                    dlProg = "Copying ROM archive... "; emit update(dlProg);
                    QFile::copy(romLink, dir + "ROM/ROM.tar");
                    dlProg += "Done\n"; emit update(dlProg);

                    extProg = "Extracting ROM files... "; emit update(dlProg + extProg);
                    command.clear(); command << "/C" << dir + "7z.exe" << "x" << dir + "ROM/ROM.tar" << "-o" + dir + "ROM/" << "-y" << "-bsp1" << ">" + dir + "ROM/out.txt";
                    QProcess extract;
                    extract.startDetached("cmd", command);
                    avoidComplete = 0;
                    while(true) {
                        QString percentage = getPercentage(dir + "ROM/");
                        extProg = "Extracting ROM files... (" + percentage + "%)"; emit update(dlProg + extProg);
                        progBar(percentage.toInt());

                        avoidComplete += 1;
                        if (avoidComplete > 40) {if (percentage == "100") {extProg = "Extracting ROM files... Done\n"; emit update(dlProg + extProg); sleep(2); break;}}

                        msleep(100);
                    }
                }
            }

            if (extractSwitch != 0) {
                extProg = "Extracting ROM files... (0%, 1/2)"; emit update(checkInternet + dlProg + extProg);
                QProcess extract; command.clear();
                command <<  "/C" << dir + "7z.exe" << "e" << dir + "ROM/ROM.tgz" << "-o" + dir + "ROM/" << "-y" << "-bsp1" << ">" + dir + "ROM/out.txt";
                extract.startDetached("cmd", command);
                avoidComplete = 0;
                while(true) {
                    QString percentage = getPercentage(dir + "ROM/");
                    int overallPercentage = percentage.toInt() / 2;
                    extProg = "Extracting ROM files... (" + QString::number(overallPercentage) + "%, 1/2)"; emit update(checkInternet + dlProg + extProg);
                    emit progBar(overallPercentage);

                    avoidComplete += 1;
                    if (avoidComplete > 40) {if (percentage == "100") {break;}}

                    msleep(100);
                }
                extProg = "Extracting ROM files... (2/2)"; emit update(checkInternet + dlProg + extProg);
                emit progBar(50);
                command.clear(); command << "/C" << dir + "7z.exe" << "x" << dir + "ROM/ROM.tar" << "-o" + dir + "ROM/" << "-y" << "-bsp1" << ">" + dir + "ROM/out.txt";
                extract.startDetached("cmd", command);
                avoidComplete = 0;
                while(true) {
                    QString percentage = getPercentage(dir + "ROM/");
                    int overallPercentage = 50 + (percentage.toInt() / 2);
                    extProg = "Extracting ROM files... (" + QString::number(overallPercentage) + "%, 2/2)"; emit update(checkInternet + dlProg + extProg);
                    emit progBar(overallPercentage);

                    avoidComplete += 1;
                    if (avoidComplete > 40) {if (percentage == "100") {sleep(2); break;}}

                    msleep(100);
                }
                extProg = "Extracting ROM files... Done\n"; emit update(checkInternet + dlProg + extProg);
            }
            if (misc != 0) {
                extProg += "Deleting leftover files... ";
                emit progBar(0);
                QFile::remove(dir + "ROM/ROM.tgz"); QFile::remove(dir + "ROM/ROM.tar"); QFile::remove(dir + "ROM/out.txt"); QFile::remove(dir + "ROM/mOut.txt");
                emit progBar(50);
                extProg += "Done\nRenaming folder to a reasonable name... "; emit update(checkInternet + dlProg + extProg);
                emit progBar(75);
                QDir rename; rename.rename(dir + "ROM/" + getROMFolderName(dir + "ROM"), dir + "ROM/ROM");
                emit progBar(100);
                extProg += "Done\n";
                emit update(checkInternet + dlProg + extProg);
                sleep(2);
                emit progBar(500);
            }
            if (modify == 1) {
                emit update(checkInternet + dlProg + extProg + "Finished");
                emit update("enable");
            }
            }
            stopRunning();
        } else if (function == 4) {
            QProcess checkConnection;
            checkConnection.setWorkingDirectory(dir + "platform-tools");
            command.clear(); command << "/C" << "fastboot.exe devices";
            int connectionBreak = 0;
            for (int i = 1; i < 101; i++) {
                emit update("Detecting device... (tries: " + QString::number(i) + "/100)"); sleep(1);
                checkConnection.start("cmd", command); checkConnection.waitForFinished();
                QString output = checkConnection.readAll();
                QString newSN = output.left(output.count() - 10);
                if (output != "") {
                    if (newSN == sn) {
                        emit update("Device connection verified.\n"
                                    "A cmd will open, where you will be\n"
                                    "able to see the progress\n"
                                    "Starting flash in 10 seconds...");
                        sleep(10);
                    } else {
                        emit msgBox("Different device detected", "Error! A different device has been detected!\n\n"
                                                                 "Reason: mismatching serial number.\n"
                                                                 "Previous: " + sn + "\n"
                                                                 "Current: " + newSN + "\n\n"
                                                                 "Please plug in the phone you originally used.", 0);
                        connectionBreak = 1;
                    }
                    break;
                } else {
                    if (i > 99) {
                        emit msgBox("Could not detect device", "do the rest :)", 0);
                        connectionBreak = 1;
                    }
                }
            }
            if (connectionBreak == 0) {
            QFile::copy(dir + "platform-tools/fastboot.exe", dir + "ROM/ROM/fastboot.exe");
            QFile::copy(dir + "platform-tools/AdbWinApi.dll", dir + "ROM/ROM/AdbWinApi.dll");
            QFile::copy(dir + "platform-tools/AdbWinUsbApi.dll", dir + "ROM/ROM/AdbWinUsbApi.dll");

            QFile rmOldOut(dir + "ROM/ROM/out.txt"); if (rmOldOut.exists()) {rmOldOut.remove();}

            emit update("Flashing...");

            if (option == 1) {
                flashProgress(QStringList() << "cmd" << "/C" << dir.toUtf8() + "ROM/ROM/flash_all.bat 1>" +
                              dir.toUtf8() + "ROM/ROM/out.txt" <<  "2>&1", dir + "ROM/ROM/out");
            } else if (option == 2) {
                QFile check(dir + "flash_all_except_data.bat");
                if (check.exists()) {
                    flashProgress(QStringList() << "cmd" << "/C" << dir.toUtf8() + "ROM/ROM/flash_all_except_data.bat 1>" <<
                                  dir.toUtf8() + "ROM/ROM/out.txt" << "2>&1", dir + "ROM/ROM/out");
                } else {
                    flashProgress(QStringList() << "cmd" << "/C" << dir.toUtf8() + "ROM/ROM/flash_all_except_data_storage.bat 1>" <<
                                  dir.toUtf8() + "ROM/ROM/out.txt" << "2>&1", dir + "ROM/ROM/out");
                }
            }

            if (errStop == 0) {
                emit update("flashFinish");
                emit update("Flashing finished!\n"
                            "If you don't want to clean temporary files,\n"
                            "close this window now.\n"
                            "If you want to clean them,\n"
                            "click on the next button.\n\n"
                            "You should have your phone flashed now.\n"
                            "Enjoy!");
            } else {
                emit update("flashError");
                errStop = 0;
            }
            }
            stopRunning();
        } else if (function == 5) {
            emit update("Deleting temporary files...");
            QDir remove(dir); remove.removeRecursively();
            emit update("green");
            emit update("Temporary files deleted.\n"
                        "You can close this window now.\n\n"
                        "Thank you for using my program.");
            emit update("close");
            stopRunning();
        }
    }
}

void fThread::stopRunning()
{
    isRunning = 0;
}
