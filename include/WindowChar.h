#ifndef WINDOWCHAR_H
#define WINDOWCHAR_H

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

class MainWindowChar : public QMainWindow {
public:
    MainWindowChar(QWidget *parent = nullptr);
    ~MainWindowChar();

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


    QMap<QString, Sequence<char>*> sequences;

    QMap<QString, Sequence<std::tuple<char,char>>*> zippedSeqs;

    QSet<QString> zippedNames;

    QString currentName;
    Sequence<char>* currentSeq = nullptr;
    Sequence<std::tuple<char,char>>* currentZipped = nullptr;

    void setupUI();
    void updateSequenceList();
    void updateCurrentDisplay();
    Sequence<char>* createSequenceByType(int type);
};

MainWindowChar::MainWindowChar(QWidget *parent) : QMainWindow(parent) {
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

MainWindowChar::~MainWindowChar() {
    for (auto s : sequences) delete s;
    for (auto z : zippedSeqs) delete z;
}

void MainWindowChar::setupUI() {
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
    editValue = new QLineEdit; editValue->setPlaceholderText("Символ");
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
    comboMapFunc->addItems({"В верхний регистр", "В нижний регистр"});
    comboReduceFunc = new QComboBox;
    comboReduceFunc->addItems({"concatenate"});
    comboWherePred = new QComboBox;
    comboWherePred->addItems({"Буква", "Число"});
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

    connect(listSequences, &QListWidget::itemClicked, this, &MainWindowChar::onSequenceClicked);
    connect(btnCreate, &QPushButton::clicked, this, &MainWindowChar::onCreateSequence);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindowChar::onRefreshList);
    connect(btnAppend, &QPushButton::clicked, this, &MainWindowChar::onAppend);
    connect(btnPrepend, &QPushButton::clicked, this, &MainWindowChar::onPrepend);
    connect(btnInsertAt, &QPushButton::clicked, this, &MainWindowChar::onInsertAt);
    connect(btnRemoveAt, &QPushButton::clicked, this, &MainWindowChar::onRemoveAt);
    connect(btnFirstLast, &QPushButton::clicked, this, &MainWindowChar::onGetFirstLast);
    connect(btnSubsequence, &QPushButton::clicked, this, &MainWindowChar::onGetSubsequence);
    connect(btnMap, &QPushButton::clicked, this, &MainWindowChar::onMap);
    connect(btnReduce, &QPushButton::clicked, this, &MainWindowChar::onReduce);
    connect(btnWhere, &QPushButton::clicked, this, &MainWindowChar::onWhere);
    connect(btnConcat, &QPushButton::clicked, this, &MainWindowChar::onConcat);
    connect(btnSlice, &QPushButton::clicked, this, &MainWindowChar::onSlice);
    connect(btnZip, &QPushButton::clicked, this, &MainWindowChar::onZip);
    connect(btnUnzip, &QPushButton::clicked, this, &MainWindowChar::onUnzip);
}

void MainWindowChar::updateSequenceList() {
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

void MainWindowChar::updateCurrentDisplay() {
    if (!currentSeq && !currentZipped) {
        textDisplay->setPlainText("Последовательность не выбрана");
        return;
    }
    if (currentZipped) {
        QString display = "[Zipped последовательность - используйте Unzip чтобы разобрать]\n\nЭлементы:\n";
        for (int i = 0; i < currentZipped->GetLength(); ++i) {
            auto t = currentZipped->Get(i);
            display += QString("(%1,%2) ").arg(QChar(std::get<0>(t))).arg(QChar(std::get<1>(t)));
        }
        textDisplay->setPlainText(display);
        labelFirstLast->setText("Первый-последний: не применимо");
        labelResult->setText("Результат Reduce: не применимо");
        return;
    }
    QString d;
    for (int i = 0; i < currentSeq->GetLength(); ++i)
        d += QChar(currentSeq->Get(i));
    textDisplay->setPlainText(d);
    labelFirstLast->setText("Первый-последний: не запрошены");
    labelResult->setText("Результат Reduce: не выполнено");
}

Sequence<char>* MainWindowChar::createSequenceByType(int type) {
    if (type == 1) return new ArraySequence<char>();
    if (type == 2) return new ListSequence<char>();
}

void MainWindowChar::onCreateSequence() {
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
    Sequence<char>* seq = createSequenceByType(type);
    int fill = QMessageBox::question(this, "Заполнение", "Заполнить дефолтными значениями?", QMessageBox::Yes|QMessageBox::No);
    if (fill == QMessageBox::Yes) {
        int size = QInputDialog::getInt(this, "Размер", "Размер:", 0, 0, 10000);
        QChar def = QInputDialog::getText(this, "Дефолт", "Дефолтный символ:").isEmpty() ? ' ' : QInputDialog::getText(this, "Дефолт", "Дефолтный символ:")[0];
        for (int i = 0; i < size; ++i) seq->Append(def.toLatin1());
    } else {
        int size = QInputDialog::getInt(this, "Заполнить самому", "Количество элементов:", 0, 0, 1000);
        for (int i = 0; i < size; ++i) {
            QChar val = QInputDialog::getText(this, "Элемент", QString("Элемент %1:").arg(i)).isEmpty() ? ' ' : QInputDialog::getText(this, "Элемент", QString("Элемент %1:").arg(i))[0];
            seq->Append(val.toLatin1());
        }
    }
    sequences[name] = seq;
    updateSequenceList();
    QMessageBox::information(this, "Успешно", "Последовательность создана");
}

void MainWindowChar::onRefreshList() {
    updateSequenceList();
    if (currentSeq || currentZipped) updateCurrentDisplay();
}


void MainWindowChar::onAppend() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    char c = editValue->text().isEmpty() ? ' ' : editValue->text()[0].toLatin1();
    currentSeq->Append(c);
    updateCurrentDisplay();
}
void MainWindowChar::onPrepend() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    char c = editValue->text().isEmpty() ? ' ' : editValue->text()[0].toLatin1();
    currentSeq->Prepend(c);
    updateCurrentDisplay();
}
void MainWindowChar::onInsertAt() {
    if (!currentSeq) return;
    int idx = editIndex->text().toInt();
    char c = editValue->text().isEmpty() ? ' ' : editValue->text()[0].toLatin1();
    try { currentSeq->InsertAt(c, idx); updateCurrentDisplay(); }
    catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowChar::onRemoveAt() {
    if (!currentSeq) return;
    int idx = editIndex->text().toInt();
    try { currentSeq->RemoveAt(idx); updateCurrentDisplay(); }
    catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowChar::onGetFirstLast() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    if (currentSeq->GetLength() == 0) { QMessageBox::warning(this, "Ошибка", "Пустая последовательность"); return; }
    labelFirstLast->setText(QString("Первый: %1, Последний: %2")
                                .arg(QChar(currentSeq->GetFirst()))
                                .arg(QChar(currentSeq->GetLast())));
}
void MainWindowChar::onGetSubsequence() {
    if (!currentSeq) return;
    int start = QInputDialog::getInt(this, "Subsequence", "Начало:", 0, 0, currentSeq->GetLength()-1);
    int end = QInputDialog::getInt(this, "Subsequence", "Конец:", start, start, currentSeq->GetLength()-1);
    try {
        Sequence<char>* sub = currentSeq->GetSubsequence(start, end);
        QString name = QInputDialog::getText(this, "Сохранить", "Имя:");
        if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
            sequences[name] = sub;
            updateSequenceList();
        } else delete sub;
    } catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}
void MainWindowChar::onMap() {
    if (!currentSeq) return;
    int idx = comboMapFunc->currentIndex();
    std::function<char(const char&)> func;
    if (idx == 0) func = [](char c){ if(c>='a'&&c<='z') return static_cast<char>(c-32); return c; };
    else if (idx == 1) func = [](char c){ if(c>='A'&&c<='Z') return static_cast<char>(c+32); return c; };
    else return;
    Sequence<char>* res = currentSeq->Map(func);
    QString name = QInputDialog::getText(this, "Сохранить Map", "Имя:");
    if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
        sequences[name] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowChar::onReduce() {
    if (!currentSeq) return;
    if (currentSeq->GetLength() == 0) { QMessageBox::warning(this, "Ошибка", "Пустая последовательность"); return; }
    std::string result;
    for (int i = 0; i < currentSeq->GetLength(); ++i) result.push_back(currentSeq->Get(i));
    labelResult->setText(QString("Concatenated: %1").arg(QString::fromStdString(result)));
}
void MainWindowChar::onWhere() {
    if (!currentSeq) return;
    int idx = comboWherePred->currentIndex();
    std::function<bool(const char&)> pred;
    if (idx == 0) pred = [](char c){ char l = tolower(c); return l=='a'||l=='e'||l=='i'||l=='o'||l=='u'; };
    else if (idx == 1) pred = [](char c){ return isdigit(c); };
    else return;
    Sequence<char>* res = currentSeq->Where(pred);
    QString name = QInputDialog::getText(this, "Сохранить Where", "Имя:");
    if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
        sequences[name] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowChar::onConcat() {
    if (!currentSeq) return;
    if (sequences.size() < 2) { QMessageBox::warning(this, "Ошибка", "Нужны 2 последовательности"); return; }
    QStringList names = sequences.keys();
    QString other = QInputDialog::getItem(this, "Concat", "Вторая последовательность:", names, 0, false);
    if (other.isEmpty() || other == currentName) return;
    Sequence<char>* res = currentSeq->Concat(sequences[other]);
    QString newName = QInputDialog::getText(this, "Сохранить", "Имя:");
    if (!newName.isEmpty() && !sequences.contains(newName) && !zippedSeqs.contains(newName)) {
        sequences[newName] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowChar::onSlice() {
    if (!currentSeq) return;
    int start = editStart->text().toInt();
    int count = editCount->text().toInt();
    try {
        currentSeq->Slice(start, count, nullptr);
        updateCurrentDisplay();
    } catch (std::exception& e) { QMessageBox::warning(this, "Ошибка", e.what()); }
}


void MainWindowChar::onZip() {
    if (!currentSeq || sequences.size() < 2) {
        QMessageBox::warning(this, "Ошибка", "Нужны 2 последовательности");
        return;
    }
    QStringList names = sequences.keys();
    QString other = QInputDialog::getItem(this, "Zip", "Вторая последовательность:", names, 0, false);
    if (other.isEmpty() || other == currentName) return;

    Sequence<std::tuple<char,char>>* zipped = ZipSequences(currentSeq, sequences[other]);
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


void MainWindowChar::onUnzip() {
    if (!currentZipped) {
        QMessageBox::warning(this, "Ошибка", "Выбранная последовательность не является zipped");
        return;
    }
    auto it = zippedSeqs.find(currentName);
    if (it == zippedSeqs.end()) {
        QMessageBox::warning(this, "Ошибка", "Zipped последовательность не найдена");
        return;
    }
    auto [first, second] = UnzipSequences<char>(it.value());
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


void MainWindowChar::onSequenceClicked(QListWidgetItem *item) {
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

#endif // WINDOWCHAR_H