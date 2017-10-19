#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Temporary apply localization here
    QTranslator translator;
    translator.load(":/locale/uk_UA");
    qApp->installTranslator(&translator);

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

    MainWindow w;
    w.show();

    return a.exec();
}
