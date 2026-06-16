#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "WorkingWithSequence.h"
#include "mainwindow.h"

class MainMenu : public QMainWindow {
    Q_OBJECT
public:
    MainMenu(QWidget *parent = nullptr);

private slots:
    void openLazySequence();
    void openSortStream();
    void openSyncStream();
};

inline MainMenu::MainMenu(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Лабораторная работа №4 - Выбор режима");
    resize(400, 300);
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *layout = new QVBoxLayout(central);

    QLabel *title = new QLabel("Выберите задание");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 18px; font-weight: bold; margin: 20px;");
    layout->addWidget(title);

    QPushButton *btnLazy = new QPushButton("Работа с LazySequence (основной интерфейс)");
    QPushButton *btnSort = new QPushButton("Сортировка потока через кучу (тема 10.2)");
    QPushButton *btnSync = new QPushButton("Синхронизация временных потоков (тема 5.1)");

    layout->addWidget(btnLazy);
    layout->addWidget(btnSort);
    layout->addWidget(btnSync);

    connect(btnLazy, &QPushButton::clicked, this, &MainMenu::openLazySequence);
    connect(btnSort, &QPushButton::clicked, this, &MainMenu::openSortStream);
    connect(btnSync, &QPushButton::clicked, this, &MainMenu::openSyncStream);
}

inline void MainMenu::openLazySequence() {
    MainWindowLazySequence *w = new MainWindowLazySequence();
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
    this->close();
}

inline void MainMenu::openSortStream() {
    MainWindow *w = new MainWindow();
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
    this->close();
}

inline void MainMenu::openSyncStream() {
    MainWindow *w = new MainWindow();
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
    this->close();
}

#endif // MAINMENU_H
