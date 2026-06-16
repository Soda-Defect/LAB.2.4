#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "tasksortworker.h"
#include "TaskSyncWorker.h"
#include "ArraySequence.h"
#include <optional>
#include <QGraphicsView>
#include <QGraphicsScene>

class QTableWidget;
class QProgressBar;
class QCustomPlot;
class QLineEdit;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSortFinished(const ArraySequence<int>& sorted);
    void onSyncFinished(const ArraySequence<ArraySequence<std::optional<double>>>& events);
    void onWorkerError(const QString& msg);


private:
    ArraySequence<int> currentData_;

    void func(const ArraySequence<int>& data);
    void setupUI();
    void displaySorted(const ArraySequence<int>& data);
    void displaySyncEvents(const ArraySequence<ArraySequence<std::optional<double>>>& events);
    void drawHeapTree(const ArraySequence<int>& heapData);
    QGraphicsView *treeView_;
    QGraphicsScene *treeScene_;
    QThread sortThread_;
    TaskSortWorker *sortWorker_;
    QThread syncThread_;
    TaskSyncWorker *syncWorker_;

    QTableWidget *sortTable_;
    QProgressBar *progressBar_;
    QTableWidget *syncTable_;
    QCustomPlot *plot_;
    QLineEdit *manualEdit_;
    QPushButton *sortBtn_;
    QPushButton *loadFileBtn_;
    QLineEdit *numStreamsEdit_;
    QLineEdit *eventsEdit_;
    QLineEdit *tauEdit_;
    QPushButton *syncBtn_;
};

#endif // MAINWINDOW_H
