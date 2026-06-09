#ifndef WINDOWINT_H
#define WINDOWINT_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QMap>
#include <QSet>
#include <cmath>
#include <functional>
#include <tuple>
#include "Sequence.h"
#include "ArraySequence.h"
#include "ListSequence.h"
#include "ZipUnzip.h"
#include "MapUtils.h"

class MainWindowInt : public QMainWindow {
public:
    MainWindowInt(QWidget *parent = nullptr);
    ~MainWindowInt();

private slots:
    void onCreateSequence();
    void onRefreshList();
    void onAppend();
    void onPrepend();
    void onInsertAt();
    void onRemoveAt();
    void onGetFirstLast();
    void onGetSubsequence();
    void onMap();
    void onReduce();
    void onWhere();
    void onConcat();
    void onSlice();
    void onZip();
    void onUnzip();
    void onSequenceClicked(QListWidgetItem *item);

private:
    QListWidget *listSequences;
    QTextEdit *textDisplay;
    QLabel *labelCurrent;
    QLabel *labelFirstLast;
    QLabel *labelResult;
    QLineEdit *editValue;
    QLineEdit *editIndex;
    QLineEdit *editStart;
    QLineEdit *editCount;
    QComboBox *comboMapFunc;
    QComboBox *comboReduceFunc;
    QComboBox *comboWherePred;

    QMap<QString, Sequence<int>*> sequences;

    QMap<QString, Sequence<std::tuple<int,int>>*> zippedSeqs;

    QSet<QString> zippedNames;

    QString currentName;
    Sequence<int>* currentSeq = nullptr;
    Sequence<std::tuple<int,int>>* currentZipped = nullptr;

    void setupUI();
    void updateSequenceList();
    void updateCurrentDisplay();
    Sequence<int>* createSequenceByType(int type);
};

MainWindowInt::MainWindowInt(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    updateSequenceList();
    this->setStyleSheet(
        "QMainWindow {"
        "    border-image: url(D:/Laba2/Sakura.jpg);"
        "}"
        "QPushButton {"
        "    background-color: white;"
        "    color: black;"
        "    border: 1px solid gray;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f0f0f0;"
        "}"
        "QLabel, QLineEdit, QComboBox, QTextEdit {"
        "    background-color: rgba(255, 255, 255, 200);"
        "    color: black;"
        "}"
        );
}

MainWindowInt::~MainWindowInt() {
    for (auto s : sequences) delete s;
    for (auto z : zippedSeqs) delete z;
}

void MainWindowInt::setupUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QHBoxLayout *topLayout = new QHBoxLayout;
    listSequences = new QListWidget;
    listSequences->setFixedWidth(200);
    topLayout->addWidget(listSequences);
    textDisplay = new QTextEdit;
    textDisplay->setReadOnly(true);
    topLayout->addWidget(textDisplay);
    mainLayout->addLayout(topLayout);

    QHBoxLayout *infoLayout = new QHBoxLayout;
    labelCurrent = new QLabel("Текущая последовательность: не выбрано");
    labelFirstLast = new QLabel("Первый последний: не выполнено");
    labelResult = new QLabel("Результат Reduce: не выполнена");
    infoLayout->addWidget(labelCurrent);
    infoLayout->addWidget(labelFirstLast);
    infoLayout->addWidget(labelResult);
    mainLayout->addLayout(infoLayout);

    QHBoxLayout *inputLayout = new QHBoxLayout;
    editValue = new QLineEdit; editValue->setPlaceholderText("Значение");
    editIndex = new QLineEdit; editIndex->setPlaceholderText("Индекс");
    editStart = new QLineEdit; editStart->setPlaceholderText("Начало");
    editCount = new QLineEdit; editCount->setPlaceholderText("Количество");
    inputLayout->addWidget(editValue);
    inputLayout->addWidget(editIndex);
    inputLayout->addWidget(editStart);
    inputLayout->addWidget(editCount);
    mainLayout->addLayout(inputLayout);

    QHBoxLayout *comboLayout = new QHBoxLayout;
    comboMapFunc = new QComboBox;
    comboMapFunc->addItems({"+1", "*2", "Возвести в квадрат", "sin", "cos", "Прибавить...", "Умножить на...", "Разделить на..."});
    comboReduceFunc = new QComboBox;
    comboReduceFunc->addItems({"сумма", "product", "максимум"});
    comboWherePred = new QComboBox;
    comboWherePred->addItems({"четные", "положительные"});
    comboLayout->addWidget(new QLabel("Map:"));
    comboLayout->addWidget(comboMapFunc);
    comboLayout->addWidget(new QLabel("Reduce:"));
    comboLayout->addWidget(comboReduceFunc);
    comboLayout->addWidget(new QLabel("Where:"));
    comboLayout->addWidget(comboWherePred);
    mainLayout->addLayout(comboLayout);

    QHBoxLayout *buttonRow1 = new QHBoxLayout;
    QPushButton *btnCreate = new QPushButton("Создать...");
    QPushButton *btnRefresh = new QPushButton("Обновить...");
    QPushButton *btnAppend = new QPushButton("Добавить в конец");
    QPushButton *btnPrepend = new QPushButton("Добавить в начало");
    QPushButton *btnInsertAt = new QPushButton("Вставить на позицию");
    QPushButton *btnRemoveAt = new QPushButton("Удалить с позиции");
    QPushButton *btnFirstLast = new QPushButton("Первый-последний");
    buttonRow1->addWidget(btnCreate);
    buttonRow1->addWidget(btnRefresh);
    buttonRow1->addWidget(btnAppend);
    buttonRow1->addWidget(btnPrepend);
    buttonRow1->addWidget(btnInsertAt);
    buttonRow1->addWidget(btnRemoveAt);
    buttonRow1->addWidget(btnFirstLast);
    mainLayout->addLayout(buttonRow1);

    QHBoxLayout *buttonRow2 = new QHBoxLayout;
    QPushButton *btnSubsequence = new QPushButton("Subsequence");
    QPushButton *btnMap = new QPushButton("Map");
    QPushButton *btnReduce = new QPushButton("Reduce");
    QPushButton *btnWhere = new QPushButton("Where");
    QPushButton *btnConcat = new QPushButton("Concat");
    QPushButton *btnSlice = new QPushButton("Slice");
    QPushButton *btnZip = new QPushButton("Zip");
    QPushButton *btnUnzip = new QPushButton("Unzip");
    buttonRow2->addWidget(btnSubsequence);
    buttonRow2->addWidget(btnMap);
    buttonRow2->addWidget(btnReduce);
    buttonRow2->addWidget(btnWhere);
    buttonRow2->addWidget(btnConcat);
    buttonRow2->addWidget(btnSlice);
    buttonRow2->addWidget(btnZip);
    buttonRow2->addWidget(btnUnzip);
    mainLayout->addLayout(buttonRow2);

    connect(listSequences, &QListWidget::itemClicked, this, &MainWindowInt::onSequenceClicked);
    connect(btnCreate, &QPushButton::clicked, this, &MainWindowInt::onCreateSequence);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindowInt::onRefreshList);
    connect(btnAppend, &QPushButton::clicked, this, &MainWindowInt::onAppend);
    connect(btnPrepend, &QPushButton::clicked, this, &MainWindowInt::onPrepend);
    connect(btnInsertAt, &QPushButton::clicked, this, &MainWindowInt::onInsertAt);
    connect(btnRemoveAt, &QPushButton::clicked, this, &MainWindowInt::onRemoveAt);
    connect(btnFirstLast, &QPushButton::clicked, this, &MainWindowInt::onGetFirstLast);
    connect(btnSubsequence, &QPushButton::clicked, this, &MainWindowInt::onGetSubsequence);
    connect(btnMap, &QPushButton::clicked, this, &MainWindowInt::onMap);
    connect(btnReduce, &QPushButton::clicked, this, &MainWindowInt::onReduce);
    connect(btnWhere, &QPushButton::clicked, this, &MainWindowInt::onWhere);
    connect(btnConcat, &QPushButton::clicked, this, &MainWindowInt::onConcat);
    connect(btnSlice, &QPushButton::clicked, this, &MainWindowInt::onSlice);
    connect(btnZip, &QPushButton::clicked, this, &MainWindowInt::onZip);
    connect(btnUnzip, &QPushButton::clicked, this, &MainWindowInt::onUnzip);
}

void MainWindowInt::updateSequenceList() {
    listSequences->clear();
    for (const QString& name : sequences.keys())
        listSequences->addItem(name);
    for (const QString& name : zippedSeqs.keys())
        listSequences->addItem(name + " [zipped]");
    if (!currentName.isEmpty()) {
        QString displayName = currentName;
        if (zippedNames.contains(currentName))
            displayName = currentName + " [zipped]";
        auto items = listSequences->findItems(displayName, Qt::MatchExactly);
        if (!items.isEmpty())
            listSequences->setCurrentItem(items.first());
    }
}

void MainWindowInt::updateCurrentDisplay() {
    if (!currentSeq && !currentZipped) {
        textDisplay->setPlainText("Последовательность не выбрана");
        return;
    }
    if (currentZipped) {
        QString display = "[Zipped последовательность - используйте Unzip чтоб разобрать]\n\nЭлементы:\n";
        for (int i = 0; i < currentZipped->GetLength(); ++i) {
            auto t = currentZipped->Get(i);
            display += QString("(%1,%2) ").arg(std::get<0>(t)).arg(std::get<1>(t));
        }
        textDisplay->setPlainText(display);
        labelFirstLast->setText("Первый последний: не применимо");
        labelResult->setText("Результат Reduce: не применимо");
        return;
    }
    QString display;
    for (int i = 0; i < currentSeq->GetLength(); ++i)
        display += QString::number(currentSeq->Get(i)) + " ";
    textDisplay->setPlainText(display);
    labelFirstLast->setText("Первый последний: не запрошены");
    labelResult->setText("Результат Reduce: none");
}

Sequence<int>* MainWindowInt::createSequenceByType(int type) {
    if (type == 1) return new ArraySequence<int>();
    if (type == 2) return new ListSequence<int>();
}

void MainWindowInt::onCreateSequence() {
    QString name = QInputDialog::getText(this, "Создать", "Имя:");
    if (name.isEmpty() || sequences.contains(name) || zippedSeqs.contains(name)) {
        QMessageBox::warning(this, "Ошибка", "Недопустимое имя");
        return;
    }
    QStringList types = {"ArraySequence", "ListSequence"};
    bool ok;
    QString typeStr = QInputDialog::getItem(this, "Тип", "Тип:", types, 0, false, &ok);
    if (!ok) return;
    int type = types.indexOf(typeStr) + 1;
    Sequence<int>* seq = createSequenceByType(type);
    int fill = QMessageBox::question(this, "Заполнение", "Заполнить дефолтными значениями?", QMessageBox::Yes|QMessageBox::No);
    if (fill == QMessageBox::Yes) {
        int size = QInputDialog::getInt(this, "Размер", "Размер:", 0, 0, 10000);
        int def = QInputDialog::getInt(this, "Дефолт", "Дефолтный элемент:", 0);
        for (int i = 0; i < size; ++i) seq->Append(def);
    } else {
        int size = QInputDialog::getInt(this, "Заполнить вручную", "Количество элементов:", 0, 0, 1000);
        for (int i = 0; i < size; ++i) {
            int val = QInputDialog::getInt(this, "Элементы", QString("Элемент %1:").arg(i));
            seq->Append(val);
        }
    }
    sequences[name] = seq;
    updateSequenceList();
    QMessageBox::information(this, "Успешно", "Последовательность создана");
}

void MainWindowInt::onRefreshList() {
    updateSequenceList();
    if (currentSeq || currentZipped) updateCurrentDisplay();
}


void MainWindowInt::onAppend() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    currentSeq->Append(editValue->text().toInt());
    updateCurrentDisplay();
}
void MainWindowInt::onPrepend() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    currentSeq->Prepend(editValue->text().toInt());
    updateCurrentDisplay();
}
void MainWindowInt::onInsertAt() {
    if (!currentSeq) return;
    int idx = editIndex->text().toInt();
    int val = editValue->text().toInt();
    try { currentSeq->InsertAt(val, idx); updateCurrentDisplay(); }
    catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowInt::onRemoveAt() {
    if (!currentSeq) return;
    int idx = editIndex->text().toInt();
    try { currentSeq->RemoveAt(idx); updateCurrentDisplay(); }
    catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowInt::onGetFirstLast() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    if (currentSeq->GetLength() == 0) { QMessageBox::warning(this, "Ошибка", "Пустая последовательность"); return; }
    labelFirstLast->setText(QString("Первый: %1, Последний: %2").arg(currentSeq->GetFirst()).arg(currentSeq->GetLast()));
}
void MainWindowInt::onGetSubsequence() {
    if (!currentSeq) return;
    int start = QInputDialog::getInt(this, "Subsequence", "Начало:", 0, 0, currentSeq->GetLength()-1);
    int end = QInputDialog::getInt(this, "Subsequence", "Конец:", start, start, currentSeq->GetLength()-1);
    try {
        Sequence<int>* sub = currentSeq->GetSubsequence(start, end);
        QString name = QInputDialog::getText(this, "Сохранить", "Имя:");
        if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
            sequences[name] = sub;
            updateSequenceList();
        } else delete sub;
    } catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowInt::onMap() {
    if (!currentSeq) return;
    int idx = comboMapFunc->currentIndex();
    std::function<int(const int&)> func;
    if (idx == 0) func = [](int x) { return x + 1; };
    else if (idx == 1) func = [](int x) { return x * 2; };
    else if (idx == 2) func = [](int x) { return x * x; };
    else if (idx == 3) func = [](int x) { return static_cast<int>(std::sin(x)); };
    else if (idx == 4) func = [](int x) { return static_cast<int>(std::cos(x)); };
    else if (idx == 5) { bool ok; int v = QInputDialog::getInt(this, "Добавить", "Число:", 0, -10000, 10000, 1, &ok); if (!ok) return; func = [v](int x) { return x + v; }; }
    else if (idx == 6) { bool ok; int v = QInputDialog::getInt(this, "Умножить", "Число:", 1, -10000, 10000, 1, &ok); if (!ok) return; func = [v](int x) { return x * v; }; }
    else if (idx == 7) { bool ok; int v = QInputDialog::getInt(this, "Разделить", "Делитель:", 1, -10000, 10000, 1, &ok); if (!ok || v == 0) { QMessageBox::warning(this, "Ошибка", "Нельзя делить на 0"); return; } func = [v](int x) { return x / v; }; }
    else return;
    Sequence<int>* res = currentSeq->Map(func);
    QString name = QInputDialog::getText(this, "Сохранить Map", "Имя:");
    if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
        sequences[name] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowInt::onReduce() {
    if (!currentSeq) return;
    if (currentSeq->GetLength() == 0) { QMessageBox::warning(this, "Ошибка", "Пустая последовательность"); return; }
    int idx = comboReduceFunc->currentIndex();
    std::function<int(const int&,const int&)> func;
    int init;
    if (idx == 0) { func = [](int a,int b){ return a+b; }; init = 0; }
    else if (idx == 1) { func = [](int a,int b){ return a*b; }; init = 1; }
    else if (idx == 2) { func = [](int a,int b){ return std::max(a,b); }; init = currentSeq->Get(0); }
    else return;
    int res = currentSeq->Reduce(func, init);
    labelResult->setText(QString("Результат Reduce: %1").arg(res));
}
void MainWindowInt::onWhere() {
    if (!currentSeq) return;
    int idx = comboWherePred->currentIndex();
    std::function<bool(const int&)> pred;
    if (idx == 0) pred = [](int x){ return x % 2 == 0; };
    else if (idx == 1) pred = [](int x){ return x > 0; };
    else return;
    Sequence<int>* res = currentSeq->Where(pred);
    QString name = QInputDialog::getText(this, "Сохранить Where", "Имя:");
    if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
        sequences[name] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowInt::onConcat() {
    if (!currentSeq) return;
    if (sequences.size() < 2) { QMessageBox::warning(this, "Ошибка", "Нужны 2 последовательности"); return; }
    QStringList names = sequences.keys();
    QString other = QInputDialog::getItem(this, "Concat", "Вторая последовательность:", names, 0, false);
    if (other.isEmpty() || other == currentName) return;
    Sequence<int>* res = currentSeq->Concat(sequences[other]);
    QString newName = QInputDialog::getText(this, "Сохранить", "Имя:");
    if (!newName.isEmpty() && !sequences.contains(newName) && !zippedSeqs.contains(newName)) {
        sequences[newName] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowInt::onSlice() {
    if (!currentSeq) return;
    int start = editStart->text().toInt();
    int count = editCount->text().toInt();
    try {
        currentSeq->Slice(start, count, nullptr);
        updateCurrentDisplay();
    } catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}


void MainWindowInt::onZip() {
    if (!currentSeq || sequences.size() < 2) {
        QMessageBox::warning(this, "Ошибка", "Нужна вторая последовательность");
        return;
    }
    QStringList names = sequences.keys();
    QString other = QInputDialog::getItem(this, "Zip", "Вторая последовательность:", names, 0, false);
    if (other.isEmpty() || other == currentName) return;

    Sequence<std::tuple<int,int>>* zipped = ZipSequences(currentSeq, sequences[other]);
    QString zName = QInputDialog::getText(this, "Сохранить Zip", "Имя:");
    if (zName.isEmpty()) {
        delete zipped;
        return;
    }
    if (zippedSeqs.contains(zName) || sequences.contains(zName)) {
        QMessageBox::warning(this, "Ошибка", "Имя уже существует");
        delete zipped;
        return;
    }
    zippedSeqs[zName] = zipped;
    zippedNames.insert(zName);
    updateSequenceList();
    QMessageBox::information(this, "Zip", "Zipped последовательность сохранена как '" + zName + " [zipped]'");
}


void MainWindowInt::onUnzip() {
    if (!currentZipped) {
        QMessageBox::warning(this, "Ошибка", "Выбранная последовательность не является zipped");
        return;
    }
    auto it = zippedSeqs.find(currentName);
    if (it == zippedSeqs.end()) {
        QMessageBox::warning(this, "Ошибка", "Zipped последовательность не найдена");
        return;
    }
    auto [first, second] = UnzipSequences<int>(it.value());
    QString fName = QInputDialog::getText(this, "Unzip", "Имя для первой последовательности:");
    QString sName = QInputDialog::getText(this, "Unzip", "Имя для второй последовательности:");
    if (!fName.isEmpty() && !sName.isEmpty() && !sequences.contains(fName) && !zippedSeqs.contains(fName) &&
        !sequences.contains(sName) && !zippedSeqs.contains(sName)) {
        sequences[fName] = first;
        sequences[sName] = second;
        delete it.value();
        zippedSeqs.erase(it);
        zippedNames.remove(currentName);
        updateSequenceList();
        currentZipped = nullptr;
        currentName.clear();
        currentSeq = nullptr;
        labelCurrent->setText("Текущая последовательность: не выбрана");
        updateCurrentDisplay();
        QMessageBox::information(this, "Unzip", "Последовательности созданы");
    } else {
        delete first;
        delete second;
        QMessageBox::warning(this, "Ошибка", "Невалидные имена или имена уже существуют");
    }
}


void MainWindowInt::onSequenceClicked(QListWidgetItem *item) {
    QString displayName = item->text();
    bool isZipped = false;
    QString realName = displayName;
    if (displayName.endsWith(" [zipped]")) {
        realName = displayName.left(displayName.length() - 9);
        isZipped = true;
    }
    if (isZipped) {
        if (!zippedSeqs.contains(realName)) return;
        currentName = realName;
        currentZipped = zippedSeqs[realName];
        currentSeq = nullptr;
        labelCurrent->setText(QString("Текущая (zipped): %1").arg(realName));
    } else {
        if (!sequences.contains(realName)) return;
        currentName = realName;
        currentSeq = sequences[realName];
        currentZipped = nullptr;
        labelCurrent->setText(QString("Текущая: %1").arg(realName));
    }
    updateCurrentDisplay();
}

#endif // WINDOWINT_H