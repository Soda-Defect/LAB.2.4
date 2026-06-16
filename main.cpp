#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "MainMenu.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainMenu w;
    w.show();
    return a.exec();
}
