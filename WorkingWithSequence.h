#ifndef LAZYSEQUENCEUI_H
#define LAZYSEQUENCEUI_H

#include <QMainWindow>
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
#include <QElapsedTimer>
#include <QDialog>
#include <QGridLayout>
#include <functional>

#include "LazySequence.h"
#include "ArraySequence.h"

template<typename T>
QString valueToString(const T& v) {
    if constexpr (std::is_same_v<T, int>) return QString::number(v);
    else if constexpr (std::is_same_v<T, double>) return QString::number(v);
    else if constexpr (std::is_same_v<T, char>) return QString(QLatin1Char(v));
    else if constexpr (std::is_same_v<T, std::string>) return QString::fromStdString(v);
    else return "";
}

class MainWindowLazySequence : public QMainWindow {
    Q_OBJECT
public:
    MainWindowLazySequence(QWidget *parent = nullptr);
    ~MainWindowLazySequence();

private slots:
    void onCreateSequence();
    void onSequenceClicked(QListWidgetItem *item);
    void onGetElement();
    void onAppend();
    void onPrepend();
    void onInsertAt();
    void onConcat();
    void onMap();
    void onWhere();
    void onZip();
    void onUnzip();
    void onTake();
    void onReduce();
    void onRunTests();

private:
    QListWidget *listSequences;
    QTextEdit *textDisplay;
    QLabel *labelCurrent;
    QLabel *labelInfo;
    QLineEdit *editIndex;
    QLineEdit *editValue;
    QLineEdit *editName;
    QComboBox *comboMapFunc;
    QComboBox *comboWherePred;
    QPushButton *btnCreate, *btnGet, *btnAppend, *btnPrepend, *btnInsertAt;
    QPushButton *btnConcat, *btnMap, *btnWhere, *btnZip, *btnUnzip, *btnTake, *btnReduce;
    QPushButton *btnRunTests;

    QMap<QString, LazySequence<int>*> seqMap;
    QString currentName;
    LazySequence<int>* currentSeq = nullptr;

    QMap<QString, LazySequence<std::pair<int,int>>*> zippedMap;
    QSet<QString> zippedNames;

    void setupUI();
    void updateSequenceList();
    void updateCurrentDisplay();
    void clearCurrent();
    void showMessage(const QString& msg, bool isError = false);
    LazySequence<int>* getCurrentSequence();
};

// ========== Реализации (inline) ==========

inline MainWindowLazySequence::MainWindowLazySequence(QWidget *parent)
    : QMainWindow(parent), currentSeq(nullptr)
{
    setupUI();
    updateSequenceList();
}

inline MainWindowLazySequence::~MainWindowLazySequence() {
    qDeleteAll(seqMap);
    qDeleteAll(zippedMap);
}

inline void MainWindowLazySequence::setupUI() {
    setWindowTitle("LazySequence Lab4");
    resize(900, 600);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QHBoxLayout *topLayout = new QHBoxLayout;
    listSequences = new QListWidget;
    listSequences->setFixedWidth(250);
    topLayout->addWidget(listSequences);
    textDisplay = new QTextEdit;
    textDisplay->setReadOnly(true);
    topLayout->addWidget(textDisplay);
    mainLayout->addLayout(topLayout);

    QHBoxLayout *infoLayout = new QHBoxLayout;
    labelCurrent = new QLabel("Текущая: не выбрана");
    labelInfo = new QLabel("Информация: —");
    infoLayout->addWidget(labelCurrent);
    infoLayout->addWidget(labelInfo);
    mainLayout->addLayout(infoLayout);

    QHBoxLayout *inputLayout = new QHBoxLayout;
    editIndex = new QLineEdit; editIndex->setPlaceholderText("Индекс");
    editValue = new QLineEdit; editValue->setPlaceholderText("Значение");
    editName = new QLineEdit; editName->setPlaceholderText("Имя для новой");
    inputLayout->addWidget(editIndex);
    inputLayout->addWidget(editValue);
    inputLayout->addWidget(editName);
    mainLayout->addLayout(inputLayout);

    QHBoxLayout *comboLayout = new QHBoxLayout;
    comboMapFunc = new QComboBox;
    comboMapFunc->addItems({"+1", "*2", "квадрат"});
    comboWherePred = new QComboBox;
    comboWherePred->addItems({"чётные", "положительные"});
    comboLayout->addWidget(new QLabel("Map:"));
    comboLayout->addWidget(comboMapFunc);
    comboLayout->addWidget(new QLabel("Where:"));
    comboLayout->addWidget(comboWherePred);
    mainLayout->addLayout(comboLayout);

    QGridLayout *buttonGrid = new QGridLayout;
    btnCreate = new QPushButton("Создать");
    btnGet = new QPushButton("Get");
    btnAppend = new QPushButton("Append");
    btnPrepend = new QPushButton("Prepend");
    btnInsertAt = new QPushButton("InsertAt");
    btnConcat = new QPushButton("Concat");
    btnMap = new QPushButton("Map");
    btnWhere = new QPushButton("Where");
    btnZip = new QPushButton("Zip");
    btnUnzip = new QPushButton("Unzip");
    btnTake = new QPushButton("Take");
    btnReduce = new QPushButton("Reduce");
    btnRunTests = new QPushButton("Тесты");

    buttonGrid->addWidget(btnCreate,0,0); buttonGrid->addWidget(btnGet,0,1);
    buttonGrid->addWidget(btnAppend,0,2); buttonGrid->addWidget(btnPrepend,0,3);
    buttonGrid->addWidget(btnInsertAt,1,0); buttonGrid->addWidget(btnConcat,1,1);
    buttonGrid->addWidget(btnMap,1,2); buttonGrid->addWidget(btnWhere,1,3);
    buttonGrid->addWidget(btnZip,2,0); buttonGrid->addWidget(btnUnzip,2,1);
    buttonGrid->addWidget(btnTake,2,2); buttonGrid->addWidget(btnReduce,2,3);
    buttonGrid->addWidget(btnRunTests,3,0,1,4);
    mainLayout->addLayout(buttonGrid);

    connect(listSequences, &QListWidget::itemClicked, this, &MainWindowLazySequence::onSequenceClicked);
    connect(btnCreate, &QPushButton::clicked, this, &MainWindowLazySequence::onCreateSequence);
    connect(btnGet, &QPushButton::clicked, this, &MainWindowLazySequence::onGetElement);
    connect(btnAppend, &QPushButton::clicked, this, &MainWindowLazySequence::onAppend);
    connect(btnPrepend, &QPushButton::clicked, this, &MainWindowLazySequence::onPrepend);
    connect(btnInsertAt, &QPushButton::clicked, this, &MainWindowLazySequence::onInsertAt);
    connect(btnConcat, &QPushButton::clicked, this, &MainWindowLazySequence::onConcat);
    connect(btnMap, &QPushButton::clicked, this, &MainWindowLazySequence::onMap);
    connect(btnWhere, &QPushButton::clicked, this, &MainWindowLazySequence::onWhere);
    connect(btnZip, &QPushButton::clicked, this, &MainWindowLazySequence::onZip);
    connect(btnUnzip, &QPushButton::clicked, this, &MainWindowLazySequence::onUnzip);
    connect(btnTake, &QPushButton::clicked, this, &MainWindowLazySequence::onTake);
    connect(btnReduce, &QPushButton::clicked, this, &MainWindowLazySequence::onReduce);
    connect(btnRunTests, &QPushButton::clicked, this, &MainWindowLazySequence::onRunTests);
}

inline void MainWindowLazySequence::updateSequenceList() {
    listSequences->clear();
    for (const QString& name : seqMap.keys()) listSequences->addItem(name);
    for (const QString& name : zippedMap.keys()) listSequences->addItem(name + " [zipped]");
}

inline void MainWindowLazySequence::updateCurrentDisplay() {
    if (currentName.isEmpty() || !currentSeq) {
        textDisplay->clear();
        labelInfo->setText("Информация: —");
        return;
    }
    bool isZipped = zippedNames.contains(currentName);
    QString display;
    if (!isZipped) {
        // Показываем все уже вычисленные элементы (но не более 50)
        size_t computed = currentSeq->ComputedCount();
        for (size_t i = 0; i < std::min((size_t)50, computed); ++i) {
            display += QString::number(currentSeq->Get(i)) + " ";
        }
        if (currentSeq->IsEndless()) {
            if (computed >= 50) display += "... ";
            display += "(бесконечная)";
            labelInfo->setText(QString("Кардинал: %1, Вычислено: %2")
                                   .arg(QString::fromStdString(currentSeq->GetCardinality().ToString()))
                                   .arg(computed));
        } else {
            labelInfo->setText(QString("Кардинал: конечная, Длина: %1")
                                   .arg(computed));
        }
    } else {
        auto zipped = zippedMap.value(currentName, nullptr);
        if (zipped) {
            size_t len = zipped->ComputedCount();
            for (size_t i = 0; i < std::min((size_t)20, len); ++i) {
                auto p = zipped->Get(i);
                display += QString("(%1,%2) ").arg(p.first).arg(p.second);
            }
            labelInfo->setText(zipped->IsEndless() ? "Бесконечная zipped" : QString("Длина: %1").arg(len));
        }
    }
    textDisplay->setPlainText(display);
}

inline void MainWindowLazySequence::clearCurrent() {
    currentName.clear();
    currentSeq = nullptr;
    labelCurrent->setText("Текущая: не выбрана");
    updateCurrentDisplay();
}

inline void MainWindowLazySequence::showMessage(const QString& msg, bool isError) {
    if (isError) QMessageBox::critical(this, "Ошибка", msg);
    else QMessageBox::information(this, "Результат", msg);
}

inline LazySequence<int>* MainWindowLazySequence::getCurrentSequence() {
    if (!currentSeq) { showMessage("Нет выбранной последовательности", true); return nullptr; }
    return currentSeq;
}

inline void MainWindowLazySequence::onSequenceClicked(QListWidgetItem *item) {
    QString displayName = item->text();
    bool isZipped = displayName.endsWith(" [zipped]");
    QString realName = isZipped ? displayName.left(displayName.length() - 9) : displayName;

    if (isZipped) {
        if (!zippedMap.contains(realName)) return;
        currentSeq = nullptr;
        currentName = realName;
        labelCurrent->setText(QString("Текущая: %1 [zipped]").arg(realName));
        auto zipped = zippedMap.value(realName);
        QString display;
        size_t len = std::min((size_t)20, zipped->ComputedCount());
        for (size_t i = 0; i < len; ++i) {
            auto p = zipped->Get(i);
            display += QString("(%1,%2) ").arg(p.first).arg(p.second);
        }
        textDisplay->setPlainText(display);
        labelInfo->setText(zipped->IsEndless() ? "Бесконечная zipped" : QString("Длина: %1").arg(zipped->ComputedCount()));
        return;
    }

    if (seqMap.contains(realName)) {
        currentSeq = seqMap[realName];
        currentName = realName;
        labelCurrent->setText(QString("Текущая: %1").arg(realName));
        updateCurrentDisplay();
    } else {
        clearCurrent();
    }
}

inline void MainWindowLazySequence::onCreateSequence() {
    QString name = QInputDialog::getText(this, "Создать", "Имя:");
    if (name.isEmpty()) return;
    if (seqMap.contains(name) || zippedMap.contains(name)) {
        showMessage("Имя уже существует", true);
        return;
    }

    QStringList choices = {"Из массива", "Фибоначчи (бесконечная)", "Натуральные числа (бесконечная)"};
    bool ok;
    QString choice = QInputDialog::getItem(this, "Способ создания", "Выберите тип:", choices, 0, false, &ok);
    if (!ok) return;

    LazySequence<int>* seq = nullptr;

    if (choice == "Из массива") {
        int size = QInputDialog::getInt(this, "Размер", "Количество элементов:", 0, 0, 100);
        ArraySequence<int> data;
        for (int i = 0; i < size; ++i) {
            int val = QInputDialog::getInt(this, "Элемент", QString("Элемент %1:").arg(i));
            data.Append(val);
        }
        seq = new LazySequence<int>(data);
    }
    else if (choice == "Фибоначчи (бесконечная)") {
        auto fibRule = [](size_t idx, const ArraySequence<int>& prev) -> int {
            if (idx == 0) return 0;
            if (idx == 1) return 1;
            return prev.Get(idx-1) + prev.Get(idx-2);
        };
        ArraySequence<int> seed;
        seed.Append(0);
        seed.Append(1);
        seq = new LazySequence<int>(fibRule, seed);
        showMessage("Создана бесконечная последовательность Фибоначчи");
    }
    else if (choice == "Натуральные числа (бесконечная)") {
        auto naturalRule = [](size_t idx, const ArraySequence<int>& /*prev*/) -> int {
            return static_cast<int>(idx + 1);  // 1,2,3,4,...
        };
        ArraySequence<int> seed;
        seq = new LazySequence<int>(naturalRule, seed);
        showMessage("Создана бесконечная последовательность натуральных чисел");
    }

    if (seq) {
        seqMap[name] = seq;
        updateSequenceList();
        clearCurrent();
    }
}

inline void MainWindowLazySequence::onGetElement() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    bool ok; int idx = editIndex->text().toInt(&ok);
    if (!ok) { showMessage("Введите индекс", true); return; }
    try {
        int val = seq->Get(idx);
        showMessage(QString("Элемент [%1] = %2").arg(idx).arg(val));
        updateCurrentDisplay();  // показываем обновлённый кэш
    } catch (const std::exception& e) {
        showMessage(e.what(), true);
    }
}

inline void MainWindowLazySequence::onAppend() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    bool ok; int val = editValue->text().toInt(&ok);
    if (!ok) { showMessage("Введите значение", true); return; }
    QString newName = editName->text();
    if (newName.isEmpty()) { showMessage("Введите имя для новой последовательности", true); return; }
    if (seqMap.contains(newName) || zippedMap.contains(newName)) {
        showMessage("Имя уже существует", true); return;
    }
    LazySequence<int>* newSeq = new LazySequence<int>(seq->Append(val));
    seqMap[newName] = newSeq;
    updateSequenceList();
    // Переключаемся на новую последовательность
    currentSeq = newSeq;
    currentName = newName;
    labelCurrent->setText(QString("Текущая: %1").arg(newName));
    updateCurrentDisplay();
    showMessage(QString("Создана '%1' = исходная + %2").arg(newName).arg(val));
}

inline void MainWindowLazySequence::onPrepend() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    bool ok; int val = editValue->text().toInt(&ok);
    if (!ok) { showMessage("Введите значение", true); return; }
    QString newName = editName->text();
    if (newName.isEmpty()) { showMessage("Введите имя", true); return; }
    if (seqMap.contains(newName) || zippedMap.contains(newName)) {
        showMessage("Имя уже существует", true); return;
    }
    LazySequence<int>* newSeq = new LazySequence<int>(seq->Prepend(val));
    seqMap[newName] = newSeq;
    updateSequenceList();
    currentSeq = newSeq;
    currentName = newName;
    labelCurrent->setText(QString("Текущая: %1").arg(newName));
    updateCurrentDisplay();
    showMessage(QString("Создана '%1' = %2 + исходная").arg(newName).arg(val));
}

inline void MainWindowLazySequence::onInsertAt() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;

    bool ok;
    int idx = editIndex->text().toInt(&ok);
    if (!ok) {
        showMessage("Введите индекс (целое число)", true);
        return;
    }
    if (idx < 0) {
        showMessage("Индекс не может быть отрицательным", true);
        return;
    }

    QMessageBox::StandardButton choice = QMessageBox::question(
        this, "Вставка",
        "Вставить один элемент или последовательность?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (choice == QMessageBox::Cancel) return;

    QString newName = editName->text();
    if (newName.isEmpty()) {
        showMessage("Введите имя для новой последовательности", true);
        return;
    }
    if (seqMap.contains(newName) || zippedMap.contains(newName)) {
        showMessage("Имя уже существует", true);
        return;
    }

    LazySequence<int>* newSeq = nullptr;

    if (choice == QMessageBox::Yes) {

        int val = editValue->text().toInt(&ok);
        if (!ok) {
            showMessage("Введите значение элемента (целое число)", true);
            return;
        }
        try {
            newSeq = new LazySequence<int>(seq->InsertAt(static_cast<size_t>(idx), val));
        } catch (const std::exception& e) {
            showMessage(QString("Ошибка вставки: %1").arg(e.what()), true);
            return;
        }
    } else {

        QStringList names = seqMap.keys();
        if (names.isEmpty()) {
            showMessage("Нет других последовательностей для вставки", true);
            return;
        }
        QString otherName = QInputDialog::getItem(this, "Вставить последовательность",
                                                  "Выберите последовательность для вставки:", names, 0, false);
        if (otherName.isEmpty() || otherName == currentName) return;
        LazySequence<int>* other = seqMap[otherName];
        try {
            newSeq = new LazySequence<int>(seq->InsertAt(static_cast<size_t>(idx), *other));
        } catch (const std::exception& e) {
            showMessage(QString("Ошибка вставки: %1").arg(e.what()), true);
            return;
        }
    }


    seqMap[newName] = newSeq;
    updateSequenceList();
    currentSeq = newSeq;
    currentName = newName;
    labelCurrent->setText(QString("Текущая: %1").arg(newName));


    updateCurrentDisplay();

    showMessage(QString("Создана последовательность '%1' = вставка на позицию %2")
                    .arg(newName).arg(idx));
}

inline void MainWindowLazySequence::onConcat() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    QStringList names = seqMap.keys();
    if (names.isEmpty()) { showMessage("Нет других последовательностей", true); return; }
    QString otherName = QInputDialog::getItem(this, "Concat", "Вторая:", names, 0, false);
    if (otherName.isEmpty() || otherName == currentName) return;
    LazySequence<int>* other = seqMap[otherName];
    QString newName = editName->text();
    if (newName.isEmpty()) { showMessage("Введите имя", true); return; }
    if (seqMap.contains(newName) || zippedMap.contains(newName)) {
        showMessage("Имя уже существует", true); return;
    }
    LazySequence<int>* newSeq = new LazySequence<int>(seq->Concat(*other));
    seqMap[newName] = newSeq;
    updateSequenceList();
    currentSeq = newSeq;
    currentName = newName;
    labelCurrent->setText(QString("Текущая: %1").arg(newName));
    updateCurrentDisplay();
    showMessage(QString("Создана '%1' = %2 + %3").arg(newName).arg(currentName).arg(otherName));
}

inline void MainWindowLazySequence::onMap() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    int idx = comboMapFunc->currentIndex();
    std::function<int(int)> func;
    if (idx == 0) func = [](int x){ return x+1; };
    else if (idx == 1) func = [](int x){ return x*2; };
    else func = [](int x){ return x*x; };
    QString newName = editName->text();
    if (newName.isEmpty()) { showMessage("Введите имя", true); return; }
    if (seqMap.contains(newName) || zippedMap.contains(newName)) {
        showMessage("Имя уже существует", true); return;
    }
    LazySequence<int>* mapped = new LazySequence<int>(seq->Map<int>(func));
    seqMap[newName] = mapped;
    updateSequenceList();
    currentSeq = mapped;
    currentName = newName;
    labelCurrent->setText(QString("Текущая: %1").arg(newName));
    updateCurrentDisplay();
    showMessage(QString("Создана '%1' = Map(%2)").arg(newName).arg(comboMapFunc->currentText()));
}

inline void MainWindowLazySequence::onWhere() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    int idx = comboWherePred->currentIndex();
    std::function<bool(int)> pred;
    if (idx == 0) pred = [](int x){ return x%2==0; };
    else pred = [](int x){ return x>0; };
    QString newName = editName->text();
    if (newName.isEmpty()) { showMessage("Введите имя", true); return; }
    if (seqMap.contains(newName) || zippedMap.contains(newName)) {
        showMessage("Имя уже существует", true); return;
    }
    LazySequence<int>* filtered = new LazySequence<int>(seq->Where(pred));
    seqMap[newName] = filtered;
    updateSequenceList();
    currentSeq = filtered;
    currentName = newName;
    labelCurrent->setText(QString("Текущая: %1").arg(newName));
    updateCurrentDisplay();
    showMessage(QString("Создана '%1' = Where(%2)").arg(newName).arg(comboWherePred->currentText()));
}

inline void MainWindowLazySequence::onZip() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    QStringList names = seqMap.keys();
    if (names.isEmpty()) { showMessage("Нет других последовательностей", true); return; }
    QString otherName = QInputDialog::getItem(this, "Zip", "Вторая:", names, 0, false);
    if (otherName.isEmpty() || otherName == currentName) return;
    LazySequence<int>* other = seqMap[otherName];
    QString newName = editName->text();
    if (newName.isEmpty()) { showMessage("Введите имя", true); return; }
    if (seqMap.contains(newName) || zippedMap.contains(newName)) {
        showMessage("Имя уже существует", true); return;
    }
    LazySequence<std::pair<int,int>>* zipped = new LazySequence<std::pair<int,int>>(seq->Zip(*other));
    zippedMap[newName] = zipped;
    zippedNames.insert(newName);
    updateSequenceList();
    // Для zip не переключаемся автоматически, т.к. это другой тип (zipped)
    showMessage(QString("Создана zipped '%1'").arg(newName));
}

inline void MainWindowLazySequence::onUnzip() {
    if (!zippedNames.contains(currentName)) {
        showMessage("Выберите zipped последовательность", true);
        return;
    }
    auto it = zippedMap.find(currentName);
    if (it == zippedMap.end()) return;
    LazySequence<std::pair<int,int>>* zipped = it.value();
    auto [first, second] = Unzip(*zipped);
    QString name1 = QInputDialog::getText(this, "Unzip", "Имя первой:");
    QString name2 = QInputDialog::getText(this, "Unzip", "Имя второй:");
    if (name1.isEmpty() || name2.isEmpty()) return;
    if (seqMap.contains(name1) || seqMap.contains(name2)) {
        showMessage("Имена уже существуют", true);
        return;
    }
    seqMap[name1] = new LazySequence<int>(first);
    seqMap[name2] = new LazySequence<int>(second);
    delete it.value();
    zippedMap.erase(it);
    zippedNames.remove(currentName);
    updateSequenceList();
    clearCurrent();
    showMessage("Unzip выполнен");
}

inline void MainWindowLazySequence::onTake() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    bool ok; int n = editIndex->text().toInt(&ok);
    if (!ok || n < 0) { showMessage("Введите неотрицательное число N", true); return; }
    QString newName = editName->text();
    if (newName.isEmpty()) { showMessage("Введите имя", true); return; }
    if (seqMap.contains(newName) || zippedMap.contains(newName)) {
        showMessage("Имя уже существует", true); return;
    }


    for (int i = 0; i < n; ++i) {
        try {
            seq->Get(i);
        } catch (...) {
            break;
        }
    }


    LazySequence<int>* taken = new LazySequence<int>(seq->Take(n));
    seqMap[newName] = taken;
    updateSequenceList();

    currentSeq = taken;
    currentName = newName;
    labelCurrent->setText(QString("Текущая: %1").arg(newName));
    updateCurrentDisplay();
    showMessage(QString("Создана '%1' = первые %2 элементов").arg(newName).arg(n));
}

inline void MainWindowLazySequence::onReduce() {
    LazySequence<int>* seq = getCurrentSequence();
    if (!seq) return;
    if (seq->IsEndless()) {
        bool ok; size_t maxCount = editIndex->text().toUInt(&ok);
        if (!ok) { showMessage("Введите максимальное количество элементов", true); return; }
        try {
            int sum = seq->Reduce([](int a, int b) { return a + b; }, 0, maxCount);
            showMessage(QString("Сумма первых %1 элементов = %2").arg(maxCount).arg(sum));
        } catch (const std::exception& e) {
            showMessage(e.what(), true);
        }
    } else {
        int sum = seq->Reduce([](int a, int b) { return a + b; }, 0);
        showMessage(QString("Сумма всех элементов = %1").arg(sum));
    }
}

inline void MainWindowLazySequence::onRunTests() {
    QDialog *testDialog = new QDialog(this);
    testDialog->setWindowTitle("Результаты модульных тестов");
    testDialog->resize(800, 600);
    QVBoxLayout *layout = new QVBoxLayout(testDialog);
    QTextEdit *output = new QTextEdit;
    output->setReadOnly(true);
    output->setFontFamily("Courier New");
    layout->addWidget(output);
    QPushButton *closeBtn = new QPushButton("Закрыть");
    layout->addWidget(closeBtn);
    connect(closeBtn, &QPushButton::clicked, testDialog, &QDialog::accept);

    output->append("=== Модульное тестирование LazySequence<int> ===\n");

    auto runTest = [&output](int testNum, const QString& desc, const QString& inputData,
                             const QString& expected, const QString& actual, bool passed) {
        output->append(QString("\nТест №%1").arg(testNum));
        output->append(QString("Описание: %1").arg(desc));
        output->append(QString("Входные данные: %1").arg(inputData));
        output->append(QString("Ожидаемый вывод: %1").arg(expected));
        output->append(QString("Вывод программы: %1").arg(actual));
        output->append(QString("Вердикт: %1").arg(passed ? "Pass" : "Failed"));
        output->append("----------------------------------------");
    };

    int testNum = 1;

    {
        ArraySequence<int> arr;
        for (int i = 1; i <= 5; ++i) arr.Append(i);
        LazySequence<int> seq(arr);
        bool passed = (seq.Get(0)==1 && seq.Get(4)==5 && seq.ComputedCount()==5);
        runTest(testNum++, "Создание LazySequence из ArraySequence",
                "Исходный массив: [1,2,3,4,5]",
                "seq.Get(0)=1, seq.Get(4)=5, длина=5",
                QString("seq.Get(0)=%1, seq.Get(4)=%2, длина=%3").arg(seq.Get(0)).arg(seq.Get(4)).arg(seq.ComputedCount()),
                passed);
    }

    {
        ArraySequence<int> arr;
        for (int i = 1; i <= 5; ++i) arr.Append(i);
        LazySequence<int> seq(arr);
        auto seq2 = seq.Append(6);
        bool passed = (seq2.Get(5)==6 && seq2.ComputedCount()==6);
        runTest(testNum++, "Append (добавление элемента в конец)",
                "Исходная: [1,2,3,4,5]; добавляем 6",
                "Новая последовательность: [1,2,3,4,5,6]",
                QString("Длина=%1, последний элемент=%2").arg(seq2.ComputedCount()).arg(seq2.Get(5)),
                passed);
    }

    {
        ArraySequence<int> arr;
        for (int i = 1; i <= 5; ++i) arr.Append(i);
        LazySequence<int> seq(arr);
        auto seq2 = seq.Prepend(0);
        bool passed = (seq2.Get(0)==0 && seq2.ComputedCount()==6);
        runTest(testNum++, "Prepend (добавление элемента в начало)",
                "Исходная: [1,2,3,4,5]; добавляем 0 в начало",
                "Новая последовательность: [0,1,2,3,4,5]",
                QString("Длина=%1, первый элемент=%2").arg(seq2.ComputedCount()).arg(seq2.Get(0)),
                passed);
    }

    {
        ArraySequence<int> arr;
        for (int i = 1; i <= 5; ++i) arr.Append(i);
        LazySequence<int> seq(arr);
        auto seq2 = seq.InsertAt(2, 99);
        bool passed = (seq2.Get(2)==99 && seq2.ComputedCount()==6);
        runTest(testNum++, "InsertAt (вставка по индексу)",
                "Исходная: [1,2,3,4,5]; вставляем 99 на позицию 2",
                "Новая последовательность: [1,2,99,3,4,5]",
                QString("Длина=%1, элемент[2]=%2").arg(seq2.ComputedCount()).arg(seq2.Get(2)),
                passed);
    }

    {
        ArraySequence<int> arr1, arr2;
        for (int i = 1; i <= 5; ++i) arr1.Append(i);
        arr2.Append(10); arr2.Append(20);
        LazySequence<int> seq1(arr1), seq2(arr2);
        auto concat = seq1.Concat(seq2);
        bool passed = (concat.Get(5)==10 && concat.Get(6)==20 && concat.ComputedCount()==7);
        runTest(testNum++, "Concat (конкатенация двух последовательностей)",
                "Первая: [1,2,3,4,5]; Вторая: [10,20]",
                "Результат: [1,2,3,4,5,10,20]",
                QString("Длина=%1, элементы[5]=%2, [6]=%3").arg(concat.ComputedCount()).arg(concat.Get(5)).arg(concat.Get(6)),
                passed);
    }

    {
        ArraySequence<int> arr;
        for (int i = 1; i <= 5; ++i) arr.Append(i);
        LazySequence<int> seq(arr);
        auto mapped = seq.Map<int>([](int x){ return x*2; });
        bool passed = (mapped.Get(0)==2 && mapped.Get(4)==10);
        runTest(testNum++, "Map (применение функции x*2)",
                "Исходная: [1,2,3,4,5]",
                "Результат: [2,4,6,8,10]",
                QString("[%1,%2,%3,%4,%5]").arg(mapped.Get(0)).arg(mapped.Get(1)).arg(mapped.Get(2)).arg(mapped.Get(3)).arg(mapped.Get(4)),
                passed);
    }

    {
        ArraySequence<int> arr;
        for (int i = 1; i <= 5; ++i) arr.Append(i);
        LazySequence<int> seq(arr);
        auto filtered = seq.Where([](int x){ return x%2==0; });
        bool passed = (filtered.ComputedCount()==2 && filtered.Get(0)==2 && filtered.Get(1)==4);
        runTest(testNum++, "Where (фильтрация чётных)",
                "Исходная: [1,2,3,4,5]",
                "Результат: [2,4]",
                QString("[%1,%2]").arg(filtered.Get(0)).arg(filtered.Get(1)),
                passed);
    }

    {
        ArraySequence<int> arr1, arr2;
        for (int i = 1; i <= 5; ++i) arr1.Append(i);
        arr2.Append(10); arr2.Append(20);
        LazySequence<int> seq1(arr1), seq2(arr2);
        auto zipped = seq1.Zip(seq2);
        auto [first, second] = Unzip(zipped);
        bool passed = (first.ComputedCount() == std::min(seq1.ComputedCount(), seq2.ComputedCount()) &&
                       first.Get(0)==1 && second.Get(0)==10);
        runTest(testNum++, "Zip и Unzip (объединение в пары и обратно)",
                "Первая: [1,2,3,4,5]; Вторая: [10,20]",
                "После Zip: [(1,10),(2,20)]; после Unzip: [1,2] и [10,20]",
                QString("Первая после Unzip: [%1,%2]; Вторая: [%3,%4]")
                    .arg(first.Get(0)).arg(first.Get(1)).arg(second.Get(0)).arg(second.Get(1)),
                passed);
    }

    {
        ArraySequence<int> arr;
        for (int i = 1; i <= 5; ++i) arr.Append(i);
        LazySequence<int> seq(arr);
        auto taken = seq.Take(3);
        bool passed = (taken.ComputedCount()==3 && taken.Get(2)==3);
        runTest(testNum++, "Take (взять первые N элементов)",
                "Исходная: [1,2,3,4,5]; N=3",
                "Результат: [1,2,3]",
                QString("[%1,%2,%3]").arg(taken.Get(0)).arg(taken.Get(1)).arg(taken.Get(2)),
                passed);
    }

    {
        ArraySequence<int> arr;
        for (int i = 1; i <= 5; ++i) arr.Append(i);
        LazySequence<int> seq(arr);
        int sum = seq.Reduce([](int a, int b){ return a+b; }, 0);
        bool passed = (sum == 15);
        runTest(testNum++, "Reduce (сумма всех элементов)",
                "Исходная: [1,2,3,4,5]",
                "Сумма = 15",
                QString("Сумма = %1").arg(sum),
                passed);
    }

    {
        auto fibRule = [](size_t idx, const ArraySequence<int>& prev) -> int {
            if (idx==0) return 0; if (idx==1) return 1;
            return prev.Get(idx-1)+prev.Get(idx-2);
        };
        ArraySequence<int> seed;
        seed.Append(0); seed.Append(1);
        LazySequence<int> fib(fibRule, seed);
        bool passed = (fib.Get(0)==0 && fib.Get(1)==1 && fib.Get(2)==1 && fib.Get(3)==2 && fib.Get(4)==3);
        runTest(testNum++, "Бесконечная последовательность (Фибоначчи)",
                "Правило: F(0)=0, F(1)=1, F(n)=F(n-1)+F(n-2)",
                "Первые 5 элементов: 0,1,1,2,3",
                QString("%1,%2,%3,%4,%5").arg(fib.Get(0)).arg(fib.Get(1)).arg(fib.Get(2)).arg(fib.Get(3)).arg(fib.Get(4)),
                passed);
    }

    testDialog->exec();
    delete testDialog;
}

#endif // LAZYSEQUENCEUI_H
