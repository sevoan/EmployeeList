#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QLocale>
#include <QFileInfo>
#include <QDateTime>
#include <QTranslator>

//#define DEBUG

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Temporary apply localization here
    QTranslator translator;
    translator.load(":/locale/uk_UA");
    qApp->installTranslator(&translator);

    qDebug() << QLocale::system().name();

#ifdef DEBUG

    QDateTime lastModified;

    QTimer* cssReloadTimer = new QTimer();
    cssReloadTimer->setInterval(500);
    QObject::connect(cssReloadTimer, &QTimer::timeout, [&lastModified] () {
        QFile file("style.css");
        QFileInfo info(file);
        if (info.lastModified() == lastModified)
            return;

        lastModified = info.lastModified();

        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Cannot open style.css file";
        } else {
            QString stylesheet(file.readAll());
            qApp->setStyleSheet(stylesheet);
            qDebug() << "styles applyed";
            file.close();
        }
    });

    cssReloadTimer->start();

#else

    QFile file(":/style/style.css");
    if (file.open(QIODevice::ReadOnly))
        qApp->setStyleSheet(QString(file.readAll()));
    else
        qDebug() << "Cannot open style.css file";

    MainWindow w;
    w.show();

#endif




    return a.exec();
}
