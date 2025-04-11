#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

#include <iostream>
#include <exception>

#include "include/welcomescreenDialog.h"
#include "include/init.h"

QFile logFile;

void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(&logFile);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logMsg = QString("[%1] %2").arg(time, msg);

    out << logMsg << Qt::endl;
    out.flush();
}

int main(int argc, char *argv[])
{
    try {
        logFile.setFileName("log.txt");
        logFile.open(QIODevice::Append | QIODevice::Text);
        qInstallMessageHandler(logHandler);

        QApplication a(argc, argv);


        WelcomeScreen* w = new WelcomeScreen();

        bool init = initApp::checkInit();

        if (!init){
            w->showOpenDbDialog();
        }
        w->show();

        QObject::connect(&a, &QApplication::aboutToQuit, w, &QWidget::deleteLater);

        return a.exec();
    }
    catch(const std::exception& e){
        std::cerr << "Fatal error: " << e.what() << std::endl;
        QMessageBox::critical(nullptr, "Error", "Ein Fehler ist aufgetreten. Die Applikation wird beendet");
        return 1;
    }

}
