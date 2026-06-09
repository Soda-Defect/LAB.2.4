#ifndef WINDOWDOUBLE_H
#define WINDOWDOUBLE_H

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
#include <string>
#include <functional>
#include <tuple>
#include "Sequence.h"
#include "ArraySequence.h"
#include "ListSequence.h"
#include "ZipUnzip.h"
#include "MapUtils.h"

class MainWindowDouble : public QMainWindow {
public:
    MainWindowDouble(QWidget *parent = nullptr);
    ~MainWindowDouble();

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


    QMap<QString, Sequence<double>*> sequences;

    QMap<QString, Sequence<std::tuple<double,double>>*> zippedSeqs;

    QSet<QString> zippedNames;

    QString currentName;
    Sequence<double>* currentSeq = nullptr;
    Sequence<std::tuple<double,double>>* currentZipped = nullptr;

    void setupUI();
    void updateSequenceList();
    void updateCurrentDisplay();
    Sequence<double>* createSequenceByType(int type);
};

MainWindowDouble::MainWindowDouble(QWidget *parent) : QMainWindow(parent) {
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

MainWindowDouble::~MainWindowDouble() {
    for (auto s : sequences) delete s;
    for (auto z : zippedSeqs) delete z;
}

void MainWindowDouble::setupUI() {
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
    labelCurrent = new QLabel("Текущая последовательность: не выбрана");
    labelFirstLast = new QLabel("Первый-последний: не запрошены");
    labelResult = new QLabel("Результат Reduce: не выполнено");
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
    comboMapFunc->addItems({"+1", "*2", "возвести в квадрат", "sin", "cos", "Прибавить...", "Умножить на...", "Поделить на..."});
    comboReduceFunc = new QComboBox;
    comboReduceFunc->addItems({"сумма", "product", "максимум"});
    comboWherePred = new QComboBox;
    comboWherePred->addItems({"положительное"});
    comboLayout->addWidget(new QLabel("Map:"));
    comboLayout->addWidget(comboMapFunc);
    comboLayout->addWidget(new QLabel("Reduce:"));
    comboLayout->addWidget(comboReduceFunc);
    comboLayout->addWidget(new QLabel("Where:"));
    comboLayout->addWidget(comboWherePred);
    mainLayout->addLayout(comboLayout);

    QHBoxLayout *buttonRow1 = new QHBoxLayout;
    QPushButton *btnCreate = new QPushButton("Создать");
    QPushButton *btnRefresh = new QPushButton("Обновить");
    QPushButton *btnAppend = new QPushButton("Добавить в конец");
    QPushButton *btnPrepend = new QPushButton("Добавить в начало");
    QPushButton *btnInsertAt = new QPushButton("Вставить на позицию");
    QPushButton *btnRemoveAt = new QPushButton("Удалить на позиции");
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

    connect(listSequences, &QListWidget::itemClicked, this, &MainWindowDouble::onSequenceClicked);
    connect(btnCreate, &QPushButton::clicked, this, &MainWindowDouble::onCreateSequence);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindowDouble::onRefreshList);
    connect(btnAppend, &QPushButton::clicked, this, &MainWindowDouble::onAppend);
    connect(btnPrepend, &QPushButton::clicked, this, &MainWindowDouble::onPrepend);
    connect(btnInsertAt, &QPushButton::clicked, this, &MainWindowDouble::onInsertAt);
    connect(btnRemoveAt, &QPushButton::clicked, this, &MainWindowDouble::onRemoveAt);
    connect(btnFirstLast, &QPushButton::clicked, this, &MainWindowDouble::onGetFirstLast);
    connect(btnSubsequence, &QPushButton::clicked, this, &MainWindowDouble::onGetSubsequence);
    connect(btnMap, &QPushButton::clicked, this, &MainWindowDouble::onMap);
    connect(btnReduce, &QPushButton::clicked, this, &MainWindowDouble::onReduce);
    connect(btnWhere, &QPushButton::clicked, this, &MainWindowDouble::onWhere);
    connect(btnConcat, &QPushButton::clicked, this, &MainWindowDouble::onConcat);
    connect(btnSlice, &QPushButton::clicked, this, &MainWindowDouble::onSlice);
    connect(btnZip, &QPushButton::clicked, this, &MainWindowDouble::onZip);
    connect(btnUnzip, &QPushButton::clicked, this, &MainWindowDouble::onUnzip);
}

void MainWindowDouble::updateSequenceList() {
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

void MainWindowDouble::updateCurrentDisplay() {
    if (!currentSeq && !currentZipped) {
        textDisplay->setPlainText("Ни одна последовательность не выбрана");
        return;
    }
    if (currentZipped) {
        QString display = "[Zipped последовательность - используйте Unzip чтоб распаковать]\n\nЭлементы:\n";
        for (int i = 0; i < currentZipped->GetLength(); ++i) {
            auto t = currentZipped->Get(i);
            display += QString("(%1,%2) ").arg(std::get<0>(t)).arg(std::get<1>(t));
        }
        textDisplay->setPlainText(display);
        labelFirstLast->setText("Первый-последний: не применимо");
        labelResult->setText("Результат Reduce: не применимо");
        return;
    }
    QString d;
    for (int i = 0; i < currentSeq->GetLength(); ++i)
        d += QString::number(currentSeq->Get(i)) + " ";
    textDisplay->setPlainText(d);
    labelFirstLast->setText("Первый-последний: не запрошены");
    labelResult->setText("Результат Reduce: не выполнено");
}

Sequence<double>* MainWindowDouble::createSequenceByType(int type) {
    if (type == 1) return new ArraySequence<double>();
    if (type == 2) return new ListSequence<double>();
}

void MainWindowDouble::onCreateSequence() {
    QString name = QInputDialog::getText(this, "Создать последовательность", "Имя:");
    if (name.isEmpty() || sequences.contains(name) || zippedSeqs.contains(name)) {
        QMessageBox::warning(this, "Ошибка", "Невалидное имя");
        return;
    }
    QStringList types = {"ArraySequence", "ListSequence"};
    bool ok;
    QString typeStr = QInputDialog::getItem(this, "Тип", "Тип:", types, 0, false, &ok);
    if (!ok) return;
    int type = types.indexOf(typeStr) + 1;
    Sequence<double>* seq = createSequenceByType(type);
    int fill = QMessageBox::question(this, "Заполнить", "Заполнить дефолтными значениями?", QMessageBox::Yes|QMessageBox::No);
    if (fill == QMessageBox::Yes) {
        int size = QInputDialog::getInt(this, "Размер", "Размер:", 0, 0, 10000);
        double def = QInputDialog::getDouble(this, "Дефолт", "Дефолтное значение:", 0);
        for (int i = 0; i < size; ++i) seq->Append(def);
    } else {
        int size = QInputDialog::getInt(this, "Вручную", "Количество элементов:", 0, 0, 1000);
        for (int i = 0; i < size; ++i) {
            double val = QInputDialog::getDouble(this, "Элемент", QString("Элемент %1:").arg(i));
            seq->Append(val);
        }
    }
    sequences[name] = seq;
    updateSequenceList();
    QMessageBox::information(this, "Успешно", "Последовательность создана");
}

void MainWindowDouble::onRefreshList() {
    updateSequenceList();
    if (currentSeq || currentZipped) updateCurrentDisplay();
}


void MainWindowDouble::onAppend() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    currentSeq->Append(editValue->text().toDouble());
    updateCurrentDisplay();
}
void MainWindowDouble::onPrepend() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    currentSeq->Prepend(editValue->text().toDouble());
    updateCurrentDisplay();
}
void MainWindowDouble::onInsertAt() {
    if (!currentSeq) return;
    int idx = editIndex->text().toInt();
    double val = editValue->text().toDouble();
    try { currentSeq->InsertAt(val, idx); updateCurrentDisplay(); }
    catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowDouble::onRemoveAt() {
    if (!currentSeq) return;
    int idx = editIndex->text().toInt();
    try { currentSeq->RemoveAt(idx); updateCurrentDisplay(); }
    catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowDouble::onGetFirstLast() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    if (currentSeq->GetLength() == 0) { QMessageBox::warning(this, "Ошибка", "Пустая последовательность"); return; }
    labelFirstLast->setText(QString("Первый: %1, Последний: %2").arg(currentSeq->GetFirst()).arg(currentSeq->GetLast()));
}
void MainWindowDouble::onGetSubsequence() {
    if (!currentSeq) return;
    int start = QInputDialog::getInt(this, "Subsequence", "Начало:", 0, 0, currentSeq->GetLength()-1);
    int end = QInputDialog::getInt(this, "Subsequence", "Конец:", start, start, currentSeq->GetLength()-1);
    try {
        Sequence<double>* sub = currentSeq->GetSubsequence(start, end);
        QString name = QInputDialog::getText(this, "Сохранить", "Имя:");
        if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
            sequences[name] = sub;
            updateSequenceList();
        } else delete sub;
    } catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowDouble::onMap() {
    if (!currentSeq) return;
    int idx = comboMapFunc->currentIndex();
    std::function<double(const double&)> func;
    if (idx == 0) func = [](double x){ return x+1.0; };
    else if (idx == 1) func = [](double x){ return x*2.0; };
    else if (idx == 2) func = [](double x){ return x*x; };
    else if (idx == 3) func = [](double x){ return std::sin(x); };
    else if (idx == 4) func = [](double x){ return std::cos(x); };
    else if (idx == 5) {
        bool ok; double v = QInputDialog::getDouble(this, "Добавить", "Число:", 0, -10000, 10000, 1, &ok);
        if (!ok) return; func = [v](double x){ return x+v; };
    }
    else if (idx == 6) {
        bool ok; double v = QInputDialog::getDouble(this, "Умножить на", "Число:", 1, -10000, 10000, 1, &ok);
        if (!ok) return; func = [v](double x){ return x*v; };
    }
    else if (idx == 7) {
        bool ok; double v = QInputDialog::getDouble(this, "Разделить на", "Делитель:", 1, -10000, 10000, 1, &ok);
        if (!ok || v == 0) { QMessageBox::warning(this, "Ошибка", "Нельзя делить на ноль"); return; }
        func = [v](double x){ return x/v; };
    }
    else return;
    Sequence<double>* res = currentSeq->Map(func);
    QString name = QInputDialog::getText(this, "Сохранить Map", "Имя:");
    if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
        sequences[name] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowDouble::onReduce() {
    if (!currentSeq) return;
    if (currentSeq->GetLength() == 0) { QMessageBox::warning(this, "Ошибка", "Пустая последовательность"); return; }
    int idx = comboReduceFunc->currentIndex();
    std::function<double(const double&,const double&)> func;
    double init;
    if (idx == 0) { func = [](double a,double b){ return a+b; }; init = 0.0; }
    else if (idx == 1) { func = [](double a,double b){ return a*b; }; init = 1.0; }
    else if (idx == 2) { func = [](double a,double b){ return std::max(a,b); }; init = currentSeq->Get(0); }
    else return;
    double res = currentSeq->Reduce(func, init);
    labelResult->setText(QString("Результат Reduce: %1").arg(res));
}
void MainWindowDouble::onWhere() {
    if (!currentSeq) return;
    int idx = comboWherePred->currentIndex();
    std::function<bool(const double&)> pred;
    if (idx == 0) pred = [](double x){ return x > 0.0; };
    else return;
    Sequence<double>* res = currentSeq->Where(pred);
    QString name = QInputDialog::getText(this, "Сохранить Where", "Имя:");
    if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
        sequences[name] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowDouble::onConcat() {
    if (!currentSeq) return;
    if (sequences.size() < 2) { QMessageBox::warning(this, "Ошибка", "Нужны две последовательности"); return; }
    QStringList names = sequences.keys();
    QString other = QInputDialog::getItem(this, "Concat", "Вторая последовательность:", names, 0, false);
    if (other.isEmpty() || other == currentName) return;
    Sequence<double>* res = currentSeq->Concat(sequences[other]);
    QString newName = QInputDialog::getText(this, "Сохранить", "Имя:");
    if (!newName.isEmpty() && !sequences.contains(newName) && !zippedSeqs.contains(newName)) {
        sequences[newName] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowDouble::onSlice() {
    if (!currentSeq) return;
    int start = editStart->text().toInt();
    int count = editCount->text().toInt();
    try {
        currentSeq->Slice(start, count, nullptr);
        updateCurrentDisplay();
    } catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}


void MainWindowDouble::onZip() {
    if (!currentSeq || sequences.size() < 2) {
        QMessageBox::warning(this, "Ошибка", "Нужны 2 последовательности");
        return;
    }
    QStringList names = sequences.keys();
    QString other = QInputDialog::getItem(this, "Zip", "Вторая последовательность:", names, 0, false);
    if (other.isEmpty() || other == currentName) return;

    Sequence<std::tuple<double,double>>* zipped = ZipSequences(currentSeq, sequences[other]);
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


void MainWindowDouble::onUnzip() {
    if (!currentZipped) {
        QMessageBox::warning(this, "Ошибка", "Выбранная последовательность не является zipped");
        return;
    }
    auto it = zippedSeqs.find(currentName);
    if (it == zippedSeqs.end()) {
        QMessageBox::warning(this, "Ошибка", "Zipped последовательность не найдена");
        return;
    }
    auto [first, second] = UnzipSequences<double>(it.value());
    QString fName = QInputDialog::getText(this, "Unzip", "Имя первой последовательности:");
    QString sName = QInputDialog::getText(this, "Unzip", "Имя второй последовательности:");
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


void MainWindowDouble::onSequenceClicked(QListWidgetItem *item) {
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

#endif //WINDOWDOUBLE_H