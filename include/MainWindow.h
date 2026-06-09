#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "WindowTest.h"
#include "WindowChar.h"
#include "WindowDouble.h"
#include "WindowInt.h"
#include "WindowString.h"
#include "test.h"
#include <QApplication>
#include <QDebug>

int  run(int argc, char *argv[]){
    QApplication a(argc, argv);
    QStringList types = {"Целые числа", "Вещественные числа", "Символы", "Строки", "Тесты"};
    bool ok;
    QString choice = QInputDialog::getItem(nullptr, "Выбор типа данных",
                                           "Выберите тип данных для последовательностей:",
                                           types, 0, false, &ok);
    if (!ok || choice.isEmpty())
        return 0;

    if (choice == "Целые числа") {
        MainWindowInt w;
        w.show();
        return a.exec();
    } else if (choice == "Вещественные числа") {
        MainWindowDouble w;
        w.show();
        return a.exec();
    } else if (choice == "Символы") {
        MainWindowChar w;
        w.show();
        return a.exec();
    } else if (choice == "Строки") {
        MainWindowString w;
        w.show();
        return a.exec();
    } else if (choice == "Тесты") {
        TestRunnerGUI w;
        QString results = w.runAllTests();
        TestResultsDialog dialog(results, nullptr);
        dialog.exec();
    }
    return 1;
}
#endif // MAINWINDOW_H