#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <random>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QHeaderView>
#include <QGraphicsTextItem>
#include <QScrollArea>
#include <functional>
#include <QTimer>
#include "HeapSorter.h"
#include "Streams.h"
#include "LazySequence.h"
#include "TimeValue.h"

static std::vector<TimeLazySequence<double>> generateRandomTimeStreams(int numStreams, int numEvents, double /*tau*/) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disTime(0.0, 100.0);
    std::uniform_real_distribution<> disVal(0.0, 100.0);
    std::vector<TimeLazySequence<double>> streams;
    for (int s = 0; s < numStreams; ++s) {
        ArraySequence<TimedValue<double>> data;
        for (int e = 0; e < numEvents; ++e) {
            double t = disTime(gen);
            double v = disVal(gen);
            data.Append({t, v});
        }
        streams.push_back(TimeLazySequence<double>(data));
    }
    return streams;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();

    sortWorker_ = new TaskSortWorker;
    sortWorker_->moveToThread(&sortThread_);
    connect(&sortThread_, &QThread::finished, sortWorker_, &QObject::deleteLater);
    connect(sortWorker_, &TaskSortWorker::finished, this, &MainWindow::onSortFinished);
    connect(sortWorker_, &TaskSortWorker::error, this, &MainWindow::onWorkerError);

    sortThread_.start();

    syncWorker_ = new TaskSyncWorker;
    syncWorker_->moveToThread(&syncThread_);
    connect(&syncThread_, &QThread::finished, syncWorker_, &QObject::deleteLater);
    connect(syncWorker_, &TaskSyncWorker::finished, this, &MainWindow::onSyncFinished);
    connect(syncWorker_, &TaskSyncWorker::error, this, &MainWindow::onWorkerError);
    syncThread_.start();
}

MainWindow::~MainWindow() {
    sortThread_.quit();
    sortThread_.wait();
    syncThread_.quit();
    syncThread_.wait();
}

void MainWindow::setupUI() {
    setWindowTitle("Lab4: LazySequence & Streams");
    resize(1000, 700);

    QTabWidget *tabs = new QTabWidget(this);
    setCentralWidget(tabs);


    QWidget *sortTab = new QWidget;
    QVBoxLayout *sortLayout = new QVBoxLayout(sortTab);

    QHBoxLayout *manualLayout = new QHBoxLayout;
    manualLayout->addWidget(new QLabel("Элементы (через запятую):"));
    manualEdit_ = new QLineEdit("5,2,8,1,9,3");
    manualLayout->addWidget(manualEdit_);
    sortBtn_ = new QPushButton("Сортировать");
    manualLayout->addWidget(sortBtn_);
    sortLayout->addLayout(manualLayout);

    QHBoxLayout *fileLayout = new QHBoxLayout;
    loadFileBtn_ = new QPushButton("Загрузить из файла");
    fileLayout->addWidget(loadFileBtn_);
    sortLayout->addLayout(fileLayout);

    progressBar_ = new QProgressBar;
    progressBar_->setVisible(false);
    sortLayout->addWidget(progressBar_);
    treeScene_ = new QGraphicsScene(this);
    treeView_ = new QGraphicsView(treeScene_);
    treeView_->setMinimumHeight(300);
    treeView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sortLayout->addWidget(treeView_, 1);

    sortTable_ = new QTableWidget;
    sortTable_->setColumnCount(1);
    sortTable_->setHorizontalHeaderLabels({"Отсортированные значения"});
    sortLayout->addWidget(sortTable_,1);
    sortTable_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sortTable_->horizontalHeader()->setStretchLastSection(true);
    sortTable_->verticalHeader()->setVisible(false);

    tabs->addTab(sortTab, "Сортировка потока (10.2)");


    QWidget *syncTab = new QWidget;
    QVBoxLayout *syncLayout = new QVBoxLayout(syncTab);

    QHBoxLayout *paramsLayout = new QHBoxLayout;
    paramsLayout->addWidget(new QLabel("Число потоков:"));
    numStreamsEdit_ = new QLineEdit("3");
    paramsLayout->addWidget(numStreamsEdit_);
    paramsLayout->addWidget(new QLabel("Событий на поток:"));
    eventsEdit_ = new QLineEdit("100");
    paramsLayout->addWidget(eventsEdit_);
    paramsLayout->addWidget(new QLabel("Tau:"));
    tauEdit_ = new QLineEdit("1.0");
    paramsLayout->addWidget(tauEdit_);
    syncBtn_ = new QPushButton("Сгенерировать и синхронизировать");
    paramsLayout->addWidget(syncBtn_);
    QPushButton *helpBtn = new QPushButton("Инструкция");
    syncLayout->addWidget(helpBtn);
    connect(helpBtn, &QPushButton::clicked, this, []() {
        QMessageBox::information(nullptr, "Справка по синхронизации потоков",
                                 "<b>Строки таблицы</b> – найденные события.<br>"
                                 "<b>Столбцы:</b><br>"
                                 " 1 – оценённое время события (t_est)<br>"
                                 " 2...N – значения датчиков (— если показание отсутствует)<br><br>"
                                 "<b>Формула оценки времени:</b><br>"
                                 "t_est = (среднее арифметическое времён показаний в группе) - Tau/2<br><br>"
                                 "<b>Рекомендации по параметрам:</b><br>"
                                 "• Используйте Tau = 5.0, число потоков = 3, событий = 50<br>"
                                 "• Если результат пуст – увеличьте Tau или число событий<br>"
                                 "• Для ручного ввода создайте CSV-файлы с колонками timestamp,value"
                                 );
    });
    syncLayout->addLayout(paramsLayout);

    syncTable_ = new QTableWidget;
    syncLayout->addWidget(syncTable_);



    tabs->addTab(syncTab, "Синхронизация потоков (5.1)");


    connect(sortBtn_, &QPushButton::clicked, this, [this]() {
        QString text = manualEdit_->text();
        QStringList parts = text.split(',');
        ArraySequence<int> data;
        for (const QString& p : parts) {
            bool ok;
            int val = p.trimmed().toInt(&ok);
            if (ok) data.Append(val);
        }
        currentData_ = data;


        BinaryHeap<int, std::less<int>> heap;
        for (int i = 0; i < data.GetLength(); ++i) {
            heap.Insert(data.Get(i));
        }

        drawHeapTree(heap.ToArray());


        QTimer::singleShot(200, this, [this, data]() {
            func(data);
        });
    });

    connect(loadFileBtn_, &QPushButton::clicked, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл с числами", "", "*.txt *.csv");
        if (!fileName.isEmpty()) {
            sortWorker_->setFileMode(true, fileName);
            progressBar_->setVisible(true);
            QMetaObject::invokeMethod(sortWorker_, "run", Qt::QueuedConnection);
        }
    });

    connect(syncBtn_, &QPushButton::clicked, this, [this]() {
        int n = numStreamsEdit_->text().toInt();
        int e = eventsEdit_->text().toInt();
        double tau = tauEdit_->text().toDouble();
        auto streams = generateRandomTimeStreams(n, e, tau);
        syncWorker_->setStreams(streams, tau);
        QMetaObject::invokeMethod(syncWorker_, "run", Qt::QueuedConnection);
    });
}

void MainWindow::onSortFinished(const ArraySequence<int>& sorted) {
    progressBar_->setVisible(false);
    displaySorted(sorted);
    sortTable_->show();
    sortTable_->resizeColumnsToContents();
    sortTable_->resizeRowsToContents();
}

void MainWindow::displaySorted(const ArraySequence<int>& data) {
    sortTable_->clearContents();
    sortTable_->setRowCount(data.GetLength());
    sortTable_->setColumnCount(1);
    sortTable_->setHorizontalHeaderLabels({"Отсортированные значения"});

    for (int i = 0; i < data.GetLength(); ++i) {
        sortTable_->setItem(i, 0, new QTableWidgetItem(QString::number(data.Get(i))));
    }

    sortTable_->resizeColumnsToContents();
    sortTable_->resizeRowsToContents();
    sortTable_->show();
}

void MainWindow::onSyncFinished(const ArraySequence<ArraySequence<std::optional<double>>>& events) {
    displaySyncEvents(events);

    QVector<double> times;
    for (int i = 0; i < events.GetLength(); ++i) {
        auto ev = events.Get(i);
        if (ev.GetLength() > 0 && ev.Get(0).has_value())
            times.append(ev.Get(0).value());
    }

}

void MainWindow::displaySyncEvents(const ArraySequence<ArraySequence<std::optional<double>>>& events) {
    if (events.GetLength() == 0) return;
    int numCols = events.Get(0).GetLength();
    syncTable_->setColumnCount(numCols);
    QStringList headers;
    headers << "Оценка времени";
    for (int i = 1; i < numCols; ++i) headers << QString("Поток %1").arg(i-1);
    syncTable_->setHorizontalHeaderLabels(headers);
    syncTable_->setRowCount(events.GetLength());
    for (int i = 0; i < events.GetLength(); ++i) {
        auto row = events.Get(i);
        for (int j = 0; j < row.GetLength(); ++j) {
            QString val = row.Get(j).has_value() ? QString::number(row.Get(j).value()) : "—";
            QTableWidgetItem *item = new QTableWidgetItem(val);
            syncTable_->setItem(i, j, item);
        }
    }
}

void MainWindow::onWorkerError(const QString& msg) {
    QMessageBox::critical(this, "Ошибка", msg);
    if (progressBar_) progressBar_->setVisible(false);
}


/*void MainWindow::drawHeapTree(const ArraySequence<int>& heapData) {
    treeScene_->clear();
    if (heapData.GetLength() == 0) return;

    const int radius = 20;
    const int levelHeight = 70;

    std::function<void(int, int, int, int)> drawNode =
        [&](int index, int x, int y, int offset) {
            if (index >= heapData.GetLength()) return;

            treeScene_->addEllipse(x - radius, y - radius, 2 * radius, 2 * radius,
                                   QPen(Qt::black), QBrush(Qt::white));

            auto *text = treeScene_->addText(QString::number(heapData.Get(index)));
            text->setPos(x - 8, y - 12);

            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < heapData.GetLength()) {
                int childX = x - offset;
                int childY = y + levelHeight;
                treeScene_->addLine(x, y + radius, childX, childY - radius, QPen(Qt::black));
                drawNode(left, childX, childY, offset / 2);
            }

            if (right < heapData.GetLength()) {
                int childX = x + offset;
                int childY = y + levelHeight;
                treeScene_->addLine(x, y + radius, childX, childY - radius, QPen(Qt::black));
                drawNode(right, childX, childY, offset / 2);
            }
        };

    drawNode(0, 500, 40, 220);
    treeView_->fitInView(treeScene_->itemsBoundingRect(), Qt::KeepAspectRatio);
}*/
void MainWindow::drawHeapTree(const ArraySequence<int>& heapData) {
    treeScene_->clear();
    if (heapData.GetLength() == 0) return;

    const int radius = 20;
    const int levelHeight = 70;

    std::function<void(int, int, int, int)> drawNode =
        [&](int index, int x, int y, int offset) {
            if (index >= heapData.GetLength()) return;

            treeScene_->addEllipse(x - radius, y - radius, 2 * radius, 2 * radius,
                                   QPen(Qt::black), QBrush(Qt::white));

            auto *text = treeScene_->addText(QString::number(heapData.Get(index)));
            text->setPos(x - 8, y - 12);

            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < heapData.GetLength()) {
                int childX = x - offset;
                int childY = y + levelHeight;
                treeScene_->addLine(x, y + radius, childX, childY - radius, QPen(Qt::black));
                drawNode(left, childX, childY, offset / 2);
            }

            if (right < heapData.GetLength()) {
                int childX = x + offset;
                int childY = y + levelHeight;
                treeScene_->addLine(x, y + radius, childX, childY - radius, QPen(Qt::black));
                drawNode(right, childX, childY, offset / 2);
            }
        };

    drawNode(0, 500, 40, 220);

    auto rect = treeScene_->itemsBoundingRect().adjusted(-30, -30, 30, 30);
    treeScene_->setSceneRect(rect);
    treeView_->fitInView(rect, Qt::KeepAspectRatio);
    treeView_->show();
    treeView_->viewport()->update();
}
void MainWindow::func(const ArraySequence<int>& data) {
    auto seq = std::make_shared<LazySequence<int>>(data);
    SequenceReadStream<int> stream(seq);
    ArraySequence<int> sorted = HeapSorter<int>::Sort(stream);

    displaySorted(sorted);
}
