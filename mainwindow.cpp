#include "mainwindow.h"

#include <QFileDialog>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMetaObject>
#include <QProgressBar>
#include <QBrush>
#include <QPen>
#include <QRectF>
#include <QStringList>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "BinaryHeap.h"
#include "HeapSorter.h"
#include "LazySequence.h"
#include "Streams.h"
#include "TimeValue.h"

namespace {
std::vector<TimeLazySequence<double>> generateRandomTimeStreams(
    int numStreams,
    int numEvents,
    double tau) {

    if (numStreams <= 0)
        throw std::invalid_argument("Число потоков должно быть больше нуля");

    if (numEvents <= 0)
        throw std::invalid_argument("Число событий должно быть больше нуля");

    if (!std::isfinite(tau) || tau < 0.0)
        throw std::invalid_argument("Tau должен быть конечным неотрицательным числом");

    const double eventSpacing = 2.0 * tau + 1.0;
    if (!std::isfinite(eventSpacing))
        throw std::overflow_error("Слишком большое значение Tau");

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> valueDist(0.0, 100.0);
    std::bernoulli_distribution hasReading(0.85);  
    std::uniform_int_distribution<int> fallbackSensor(0, numStreams - 1);

    std::vector<ArraySequence<TimedValue<double>>> rawData(
        static_cast<size_t>(numStreams));

    double trueTime = 10.0;

    for (int eventIndex = 0; eventIndex < numEvents; ++eventIndex) {
        std::vector<bool> present(static_cast<size_t>(numStreams), false);
        bool atLeastOneReading = false;

        for (int streamIndex = 0; streamIndex < numStreams; ++streamIndex) {
            present[static_cast<size_t>(streamIndex)] = hasReading(gen);
            atLeastOneReading =
                atLeastOneReading || present[static_cast<size_t>(streamIndex)];
        }

        if (!atLeastOneReading) {
            present[static_cast<size_t>(fallbackSensor(gen))] = true;
        }

        std::uniform_real_distribution<double> delayDist(0.0, tau);

        for (int streamIndex = 0; streamIndex < numStreams; ++streamIndex) {
            if (!present[static_cast<size_t>(streamIndex)])
                continue;

            const double delay = (tau == 0.0) ? 0.0 : delayDist(gen);
            const double timestamp = trueTime + delay;

            if (!std::isfinite(timestamp))
                throw std::overflow_error("Переполнение времени при генерации данных");

            rawData[static_cast<size_t>(streamIndex)].Append(
                TimedValue<double>{timestamp, valueDist(gen)});
        }

        trueTime += eventSpacing;
        if (!std::isfinite(trueTime) && eventIndex + 1 < numEvents)
            throw std::overflow_error("Переполнение времени при генерации данных");
    }

    std::vector<TimeLazySequence<double>> streams;
    streams.reserve(static_cast<size_t>(numStreams));

    for (const auto& data : rawData)
        streams.emplace_back(data);

    return streams;
}

} 

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      treeView_(nullptr),
      treeScene_(nullptr),
      sortWorker_(nullptr),
      syncWorker_(nullptr),
      sortTable_(nullptr),
      progressBar_(nullptr),
      syncTable_(nullptr),
      plot_(nullptr),
      manualEdit_(nullptr),
      sortBtn_(nullptr),
      loadFileBtn_(nullptr),
      numStreamsEdit_(nullptr),
      eventsEdit_(nullptr),
      tauEdit_(nullptr),
      syncBtn_(nullptr) {

    setupUI();

    sortWorker_ = new TaskSortWorker;
    sortWorker_->moveToThread(&sortThread_);

    connect(&sortThread_, &QThread::finished,
            sortWorker_, &QObject::deleteLater);
    connect(sortWorker_, &TaskSortWorker::finished,
            this, &MainWindow::onSortFinished);
    connect(sortWorker_, &TaskSortWorker::error,
            this, &MainWindow::onWorkerError);

    sortThread_.start();

    syncWorker_ = new TaskSyncWorker;
    syncWorker_->moveToThread(&syncThread_);

    connect(&syncThread_, &QThread::finished,
            syncWorker_, &QObject::deleteLater);
    connect(syncWorker_, &TaskSyncWorker::finished,
            this, &MainWindow::onSyncFinished);
    connect(syncWorker_, &TaskSyncWorker::error,
            this, &MainWindow::onWorkerError);

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

    auto* tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    auto* sortTab = new QWidget;
    auto* sortLayout = new QVBoxLayout(sortTab);

    auto* manualLayout = new QHBoxLayout;
    manualLayout->addWidget(new QLabel("Элементы (через запятую):"));

    manualEdit_ = new QLineEdit("5,2,8,1,9,3");
    manualLayout->addWidget(manualEdit_);

    sortBtn_ = new QPushButton("Сортировать");
    manualLayout->addWidget(sortBtn_);
    sortLayout->addLayout(manualLayout);

    auto* fileLayout = new QHBoxLayout;
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
    sortTable_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sortTable_->horizontalHeader()->setStretchLastSection(true);
    sortTable_->verticalHeader()->setVisible(false);
    sortLayout->addWidget(sortTable_, 1);

    tabs->addTab(sortTab, "Сортировка потока (10.2)");

    auto* syncTab = new QWidget;
    auto* syncLayout = new QVBoxLayout(syncTab);
    auto* paramsLayout = new QHBoxLayout;

    paramsLayout->addWidget(new QLabel("Число потоков:"));
    numStreamsEdit_ = new QLineEdit("3");
    paramsLayout->addWidget(numStreamsEdit_);

    paramsLayout->addWidget(new QLabel("Число событий:"));
    eventsEdit_ = new QLineEdit("100");
    paramsLayout->addWidget(eventsEdit_);

    paramsLayout->addWidget(new QLabel("Tau:"));
    tauEdit_ = new QLineEdit("1.0");
    paramsLayout->addWidget(tauEdit_);

    syncBtn_ = new QPushButton("Сгенерировать и синхронизировать");
    paramsLayout->addWidget(syncBtn_);

    syncLayout->addLayout(paramsLayout);

    auto* helpBtn = new QPushButton("Инструкция");
    syncLayout->addWidget(helpBtn);

    syncTable_ = new QTableWidget;
    syncTable_->horizontalHeader()->setStretchLastSection(true);
    syncTable_->verticalHeader()->setVisible(false);
    syncLayout->addWidget(syncTable_);

    tabs->addTab(syncTab, "Синхронизация потоков (5.1)");

    connect(helpBtn, &QPushButton::clicked, this, []() {
        QMessageBox::information(
            nullptr,
            "Справка по синхронизации потоков",
            "<b>Строки таблицы</b> — найденные события.<br>"
            "<b>Столбцы:</b><br>"
            "1 — оценённое время события (t_est)<br>"
            "2...N — значения датчиков (— если показание отсутствует)<br><br>"
            "<b>Формула оценки времени:</b><br>"
            "t_est = среднее время показаний группы - Tau/2<br><br>"
            "Тестовый генератор создаёт упорядоченные временные потоки. "
            "Для каждого события задержка каждого сработавшего датчика "
            "равномерно распределена на [0, Tau], а часть показаний "
            "случайно пропускается."
        );
    });

    connect(sortBtn_, &QPushButton::clicked, this, [this]() {
        const QString text = manualEdit_->text().trimmed();

        if (text.isEmpty()) {
            QMessageBox::warning(this, "Ошибка ввода",
                                 "Введите хотя бы одно целое число");
            return;
        }

        const QStringList parts = text.split(',', Qt::KeepEmptyParts);
        ArraySequence<int> data;

        for (const QString& part : parts) {
            const QString token = part.trimmed();
            bool ok = false;
            const int value = token.toInt(&ok);

            if (!ok || token.isEmpty()) {
                QMessageBox::warning(
                    this,
                    "Ошибка ввода",
                    QString("Некорректное целое число: \"%1\"").arg(token));
                return;
            }

            data.Append(value);
        }

        currentData_ = data;

        BinaryHeap<int, std::less<int>> heap;
        for (int i = 0; i < data.GetLength(); ++i)
            heap.Insert(data.Get(i));

        drawHeapTree(heap.ToArray());
        func(data);
    });

    connect(loadFileBtn_, &QPushButton::clicked, this, [this]() {
        const QString fileName = QFileDialog::getOpenFileName(
            this,
            "Выберите файл с числами",
            QString(),
            "Текстовые файлы (*.txt *.csv);;Все файлы (*)");

        if (fileName.isEmpty())
            return;

        loadFileBtn_->setEnabled(false);
        sortBtn_->setEnabled(false);
        progressBar_->setRange(0, 0);  
        progressBar_->setVisible(true);

        QMetaObject::invokeMethod(
            sortWorker_,
            [worker = sortWorker_, fileName]() {
                worker->setFileMode(true, fileName);
                worker->run();
            },
            Qt::QueuedConnection);
    });

    connect(syncBtn_, &QPushButton::clicked, this, [this]() {
        bool streamsOk = false;
        bool eventsOk = false;
        bool tauOk = false;

        const int numStreams = numStreamsEdit_->text().trimmed().toInt(&streamsOk);
        const int numEvents = eventsEdit_->text().trimmed().toInt(&eventsOk);
        const double tau = tauEdit_->text().trimmed().toDouble(&tauOk);

        if (!streamsOk || numStreams <= 0) {
            QMessageBox::warning(this, "Ошибка ввода",
                                 "Число потоков должно быть положительным целым числом");
            return;
        }

        if (!eventsOk || numEvents <= 0) {
            QMessageBox::warning(this, "Ошибка ввода",
                                 "Число событий должно быть положительным целым числом");
            return;
        }

        if (!tauOk || !std::isfinite(tau) || tau < 0.0) {
            QMessageBox::warning(this, "Ошибка ввода",
                                 "Tau должен быть конечным неотрицательным числом");
            return;
        }

        try {
            auto streams = generateRandomTimeStreams(numStreams, numEvents, tau);

            syncBtn_->setEnabled(false);
            syncTable_->clearContents();
            syncTable_->setRowCount(0);

            QMetaObject::invokeMethod(
                syncWorker_,
                [worker = syncWorker_,
                 streams = std::move(streams),
                 tau]() mutable {
                    worker->setStreams(streams, tau);
                    worker->run();
                },
                Qt::QueuedConnection);
        } catch (const std::exception& e) {
            syncBtn_->setEnabled(true);
            QMessageBox::critical(this, "Ошибка", QString::fromUtf8(e.what()));
        }
    });
}

void MainWindow::onSortFinished(const ArraySequence<int>& sorted) {
    progressBar_->setVisible(false);
    progressBar_->setRange(0, 100);

    loadFileBtn_->setEnabled(true);
    sortBtn_->setEnabled(true);

    displaySorted(sorted);
}

void MainWindow::displaySorted(const ArraySequence<int>& data) {
    sortTable_->setUpdatesEnabled(false);
    sortTable_->clearContents();
    sortTable_->setRowCount(data.GetLength());
    sortTable_->setColumnCount(1);
    sortTable_->setHorizontalHeaderLabels({"Отсортированные значения"});

    for (int i = 0; i < data.GetLength(); ++i) {
        sortTable_->setItem(
            i,
            0,
            new QTableWidgetItem(QString::number(data.Get(i))));
    }

    sortTable_->resizeColumnsToContents();
    sortTable_->resizeRowsToContents();
    sortTable_->setUpdatesEnabled(true);
    sortTable_->show();
}

void MainWindow::onSyncFinished(
    const ArraySequence<ArraySequence<std::optional<double>>>& events) {

    syncBtn_->setEnabled(true);
    displaySyncEvents(events);
}

void MainWindow::displaySyncEvents(
    const ArraySequence<ArraySequence<std::optional<double>>>& events) {

    syncTable_->setUpdatesEnabled(false);
    syncTable_->clearContents();

    if (events.GetLength() == 0) {
        syncTable_->setRowCount(0);
        syncTable_->setColumnCount(0);
        syncTable_->setUpdatesEnabled(true);
        return;
    }

    const int numCols = events.Get(0).GetLength();
    syncTable_->setColumnCount(numCols);
    syncTable_->setRowCount(events.GetLength());

    QStringList headers;
    headers << "Оценка времени";
    for (int column = 1; column < numCols; ++column)
        headers << QString("Поток %1").arg(column);

    syncTable_->setHorizontalHeaderLabels(headers);

    for (int rowIndex = 0; rowIndex < events.GetLength(); ++rowIndex) {
        const auto row = events.Get(rowIndex);

        if (row.GetLength() != numCols) {
            syncTable_->clearContents();
            syncTable_->setRowCount(0);
            syncTable_->setColumnCount(0);
            syncTable_->setUpdatesEnabled(true);
            QMessageBox::critical(
                this,
                "Ошибка",
                "StreamSynchronizer вернул строки разной длины");
            return;
        }

        for (int column = 0; column < numCols; ++column) {
            const auto value = row.Get(column);
            const QString text = value.has_value()
                ? QString::number(value.value(), 'g', 12)
                : QString::fromUtf8("—");

            syncTable_->setItem(
                rowIndex,
                column,
                new QTableWidgetItem(text));
        }
    }

    syncTable_->resizeColumnsToContents();
    syncTable_->resizeRowsToContents();
    syncTable_->setUpdatesEnabled(true);
}

void MainWindow::onWorkerError(const QString& msg) {
    progressBar_->setVisible(false);
    progressBar_->setRange(0, 100);

    sortBtn_->setEnabled(true);
    loadFileBtn_->setEnabled(true);
    syncBtn_->setEnabled(true);

    QMessageBox::critical(this, "Ошибка", msg);
}

void MainWindow::drawHeapTree(const ArraySequence<int>& heapData) {
    treeScene_->clear();

    if (heapData.GetLength() == 0) {
        treeView_->viewport()->update();
        return;
    }

    const int radius = 20;
    const int levelHeight = 70;

    std::function<void(int, int, int, int)> drawNode =
        [&](int index, int x, int y, int offset) {
            if (index >= heapData.GetLength())
                return;

            treeScene_->addEllipse(
                x - radius,
                y - radius,
                2 * radius,
                2 * radius,
                QPen(Qt::black),
                QBrush(Qt::white));

            auto* text = treeScene_->addText(QString::number(heapData.Get(index)));
            const QRectF textRect = text->boundingRect();
            text->setPos(x - textRect.width() / 2.0,
                         y - textRect.height() / 2.0);

            const int left = 2 * index + 1;
            const int right = 2 * index + 2;
            const int nextOffset = std::max(offset / 2, radius * 2);

            if (left < heapData.GetLength()) {
                const int childX = x - offset;
                const int childY = y + levelHeight;

                treeScene_->addLine(
                    x,
                    y + radius,
                    childX,
                    childY - radius,
                    QPen(Qt::black));

                drawNode(left, childX, childY, nextOffset);
            }

            if (right < heapData.GetLength()) {
                const int childX = x + offset;
                const int childY = y + levelHeight;

                treeScene_->addLine(
                    x,
                    y + radius,
                    childX,
                    childY - radius,
                    QPen(Qt::black));

                drawNode(right, childX, childY, nextOffset);
            }
        };

    drawNode(0, 500, 40, 220);

    const QRectF rect = treeScene_->itemsBoundingRect().adjusted(-30, -30, 30, 30);
    treeScene_->setSceneRect(rect);
    treeView_->fitInView(rect, Qt::KeepAspectRatio);
    treeView_->viewport()->update();
}

void MainWindow::func(const ArraySequence<int>& data) {
    auto seq = std::make_shared<LazySequence<int>>(data);
    SequenceReadStream<int> stream(seq);
    const ArraySequence<int> sorted = HeapSorter<int>::Sort(stream);

    displaySorted(sorted);
}
