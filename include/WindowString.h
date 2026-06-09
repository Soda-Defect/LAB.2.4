#ifndef WINDOWSTRING_H
#define WINDOWSTRING_H

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

class MainWindowString : public QMainWindow {
public:
    MainWindowString(QWidget *parent = nullptr);
    ~MainWindowString();

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


    QMap<QString, Sequence<std::string>*> sequences;

    QMap<QString, Sequence<std::tuple<std::string, std::string>>*> zippedSeqs;

    QSet<QString> zippedNames;

    QString currentName;
    Sequence<std::string>* currentSeq = nullptr;

    Sequence<std::tuple<std::string, std::string>>* currentZipped = nullptr;

    void setupUI();
    void updateSequenceList();
    void updateCurrentDisplay();
    Sequence<std::string>* createSequenceByType(int type);
};

MainWindowString::MainWindowString(QWidget *parent) : QMainWindow(parent) {
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

MainWindowString::~MainWindowString() {
    for (auto s : sequences) delete s;
    for (auto z : zippedSeqs) delete z;
}

void MainWindowString::setupUI() {
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
    labelCurrent = new QLabel("Текущая последовательность: Не выбрана");
    labelFirstLast = new QLabel("Первый-последний: не были запрошены");
    labelResult = new QLabel("Результат Reduce: none");
    infoLayout->addWidget(labelCurrent);
    infoLayout->addWidget(labelFirstLast);
    infoLayout->addWidget(labelResult);
    mainLayout->addLayout(infoLayout);

    QHBoxLayout *inputLayout = new QHBoxLayout;
    editValue = new QLineEdit; editValue->setPlaceholderText("Строка");
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
    comboMapFunc->addItems({"В верхний регистр", "В нижний регистр", "Применить шифр цезаря", "ВеРбЛюЖиЙ шрифт"});
    comboReduceFunc = new QComboBox;
    comboReduceFunc->addItems({"concatenate"});
    comboWherePred = new QComboBox;
    comboWherePred->addItems({"содержит 'a'", "Длина>3"});
    comboLayout->addWidget(new QLabel("Map:"));
    comboLayout->addWidget(comboMapFunc);
    comboLayout->addWidget(new QLabel("Reduce:"));
    comboLayout->addWidget(comboReduceFunc);
    comboLayout->addWidget(new QLabel("Where:"));
    comboLayout->addWidget(comboWherePred);
    mainLayout->addLayout(comboLayout);

    QHBoxLayout *buttonRow1 = new QHBoxLayout;
    QPushButton *btnCreate = new QPushButton("Создать последовательность");
    QPushButton *btnRefresh = new QPushButton("Обновить");
    QPushButton *btnAppend = new QPushButton("Добавить строку в конец");
    QPushButton *btnPrepend = new QPushButton("Добавить строку в начало");
    QPushButton *btnInsertAt = new QPushButton("Вставить строку на позицию");
    QPushButton *btnRemoveAt = new QPushButton("Удалить строку на позиции");
    QPushButton *btnFirstLast = new QPushButton("Получить Первую-последнюю строку");
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

    connect(listSequences, &QListWidget::itemClicked, this, &MainWindowString::onSequenceClicked);
    connect(btnCreate, &QPushButton::clicked, this, &MainWindowString::onCreateSequence);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindowString::onRefreshList);
    connect(btnAppend, &QPushButton::clicked, this, &MainWindowString::onAppend);
    connect(btnPrepend, &QPushButton::clicked, this, &MainWindowString::onPrepend);
    connect(btnInsertAt, &QPushButton::clicked, this, &MainWindowString::onInsertAt);
    connect(btnRemoveAt, &QPushButton::clicked, this, &MainWindowString::onRemoveAt);
    connect(btnFirstLast, &QPushButton::clicked, this, &MainWindowString::onGetFirstLast);
    connect(btnSubsequence, &QPushButton::clicked, this, &MainWindowString::onGetSubsequence);
    connect(btnMap, &QPushButton::clicked, this, &MainWindowString::onMap);
    connect(btnReduce, &QPushButton::clicked, this, &MainWindowString::onReduce);
    connect(btnWhere, &QPushButton::clicked, this, &MainWindowString::onWhere);
    connect(btnConcat, &QPushButton::clicked, this, &MainWindowString::onConcat);
    connect(btnSlice, &QPushButton::clicked, this, &MainWindowString::onSlice);
    connect(btnZip, &QPushButton::clicked, this, &MainWindowString::onZip);
    connect(btnUnzip, &QPushButton::clicked, this, &MainWindowString::onUnzip);
}

void MainWindowString::updateSequenceList() {
    listSequences->clear();

    for (const QString& n : sequences.keys())
        listSequences->addItem(n);

    for (const QString& n : zippedSeqs.keys())
        listSequences->addItem(n + " [zipped]");

    if (!currentName.isEmpty()) {
        QString displayName = currentName;
        if (zippedNames.contains(currentName))
            displayName = currentName + " [zipped]";
        auto items = listSequences->findItems(displayName, Qt::MatchExactly);
        if (!items.isEmpty())
            listSequences->setCurrentItem(items.first());
    }
}

void MainWindowString::updateCurrentDisplay() {
    if (!currentSeq && !currentZipped) {
        textDisplay->setPlainText("Ни одной последовательности не выделено");
        return;
    }
    if (currentZipped) {
        QString d = "[";
        for (int i = 0; i < currentZipped->GetLength(); ++i) {
            auto t = currentZipped->Get(i);
            d += "(" + QString::fromStdString(std::get<0>(t)) + "," + QString::fromStdString(std::get<1>(t)) + ")";
            if (i < currentZipped->GetLength() - 1) d += ", ";
        }
        d += "]";
        textDisplay->setPlainText(d);
        labelFirstLast->setText("Первый-последний: не применимо");
        labelResult->setText("Результат Reduce: не применимо");
        return;
    }
    QString d;
    for (int i = 0; i < currentSeq->GetLength(); ++i)
        d += QString::fromStdString(currentSeq->Get(i)) + " ";
    textDisplay->setPlainText(d);
    labelFirstLast->setText("Первый-последний: не выполнено");
    labelResult->setText("Результат Reduce: не выполнено");
}

Sequence<std::string>* MainWindowString::createSequenceByType(int type) {
    if (type == 1) return new ArraySequence<std::string>();
    if (type == 2) return new ListSequence<std::string>();
}


void MainWindowString::onCreateSequence() {
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
    Sequence<std::string>* seq = createSequenceByType(type);
    int fill = QMessageBox::question(this, "Заполнение", "Заполнить дефолтными значениями?", QMessageBox::Yes|QMessageBox::No);
    if (fill == QMessageBox::Yes) {
        int size = QInputDialog::getInt(this, "Размер", "Размер:", 0, 0, 10000);
        QString def = QInputDialog::getText(this, "Дефолт", "Дефолтная строка:");
        for (int i = 0; i < size; ++i) seq->Append(def.toStdString());
    } else {
        int size = QInputDialog::getInt(this, "Заполнить самому", "Число:", 0, 0, 1000);
        for (int i = 0; i < size; ++i) {
            QString val = QInputDialog::getText(this, "Элемент", QString("Элемент %1:").arg(i));
            seq->Append(val.toStdString());
        }
    }
    sequences[name] = seq;
    updateSequenceList();
    QMessageBox::information(this, "Успешно", "Последовательность создана");
}

void MainWindowString::onRefreshList() {
    updateSequenceList();
    if (currentSeq || currentZipped) updateCurrentDisplay();
}

void MainWindowString::onAppend() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    currentSeq->Append(editValue->text().toStdString());
    updateCurrentDisplay();
}
void MainWindowString::onPrepend() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    currentSeq->Prepend(editValue->text().toStdString());
    updateCurrentDisplay();
}
void MainWindowString::onInsertAt() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    int idx = editIndex->text().toInt();
    std::string val = editValue->text().toStdString();
    try {
        currentSeq->InsertAt(val, idx);
        updateCurrentDisplay();
    } catch (std::exception& e) {
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}
void MainWindowString::onRemoveAt() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    int idx = editIndex->text().toInt();
    try {
        currentSeq->RemoveAt(idx);
        updateCurrentDisplay();
    } catch (std::exception& e) {
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}
void MainWindowString::onGetFirstLast() {
    if (!currentSeq) { QMessageBox::warning(this, "Ошибка", "Нет выбранной обычной последовательности"); return; }
    if (currentSeq->GetLength() == 0) { QMessageBox::warning(this, "Ошибка", "Пустая последовательность"); return; }
    labelFirstLast->setText(QString("Первый: %1, Последний: %2")
                                .arg(QString::fromStdString(currentSeq->GetFirst()))
                                .arg(QString::fromStdString(currentSeq->GetLast())));
}
void MainWindowString::onGetSubsequence() {
    if (!currentSeq) return;
    int start = QInputDialog::getInt(this, "Subsequence", "Начало:", 0, 0, currentSeq->GetLength() - 1);
    int end = QInputDialog::getInt(this, "Subsequence", "Конец:", start, start, currentSeq->GetLength() - 1);
    try {
        Sequence<std::string>* sub = currentSeq->GetSubsequence(start, end);
        QString name = QInputDialog::getText(this, "Сохранить", "Имя:");
        if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
            sequences[name] = sub;
            updateSequenceList();
        } else delete sub;
    } catch (std::exception& e) {
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}
void MainWindowString::onMap() {
    if (!currentSeq) return;
    int idx = comboMapFunc->currentIndex();
    std::function<std::string(const std::string&)> func;
    if (idx == 0) func = [](const std::string& s) { std::string r; for (char c : s) r += toupper(c); return r; };
    else if (idx == 1) func = [](const std::string& s) { std::string r; for (char c : s) r += tolower(c); return r; };
    else if (idx == 2) func = [](const std::string& s) {
            std::string r; for (char c : s) {
                if (c >= 'A' && c <= 'Z') r += ((c - 'A' + 4) % 26) + 'A';
                else if (c >= 'a' && c <= 'z') r += ((c - 'a' + 4) % 26) + 'a';
                else r += c;
            } return r;
        };
    else if (idx == 3) func = [](const std::string& s) {
            std::string r; for (size_t i = 0; i < s.size(); ++i) {
                if (i % 2 == 0) r += toupper(s[i]);
                else r += tolower(s[i]);
            } return r;
        };
    else return;
    Sequence<std::string>* res = currentSeq->Map(func);
    QString name = QInputDialog::getText(this, "Сохранить Map", "Имя:");
    if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
        sequences[name] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowString::onReduce() {
    if (!currentSeq) return;
    if (currentSeq->GetLength() == 0) { QMessageBox::warning(this, "Ошибка", "Пустая последовательность"); return; }
    std::string result;
    for (int i = 0; i < currentSeq->GetLength(); ++i) result += currentSeq->Get(i);
    labelResult->setText(QString("Concatenated: %1").arg(QString::fromStdString(result)));
}
void MainWindowString::onWhere() {
    if (!currentSeq) return;
    int idx = comboWherePred->currentIndex();
    std::function<bool(const std::string&)> pred;
    if (idx == 0) pred = [](const std::string& s) { return s.find('a') != std::string::npos; };
    else if (idx == 1) pred = [](const std::string& s) { return s.length() > 3; };
    else return;
    Sequence<std::string>* res = currentSeq->Where(pred);
    QString name = QInputDialog::getText(this, "Сохранить Where", "Имя:");
    if (!name.isEmpty() && !sequences.contains(name) && !zippedSeqs.contains(name)) {
        sequences[name] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowString::onConcat() {
    if (!currentSeq) return;
    if (sequences.size() < 2) { QMessageBox::warning(this, "Ошибка", "Нужно 2 последовательности"); return; }
    QStringList names = sequences.keys();
    QString other = QInputDialog::getItem(this, "Concat", "Вторая последовательность:", names, 0, false);
    if (other.isEmpty() || other == currentName) return;
    Sequence<std::string>* res = currentSeq->Concat(sequences[other]);
    QString newName = QInputDialog::getText(this, "Save", "Name:");
    if (!newName.isEmpty() && !sequences.contains(newName) && !zippedSeqs.contains(newName)) {
        sequences[newName] = res;
        updateSequenceList();
    } else delete res;
}
void MainWindowString::onSlice() {
    if (!currentSeq) return;
    int start = editStart->text().toInt();
    int count = editCount->text().toInt();
    try {
        currentSeq->Slice(start, count, nullptr);
        updateCurrentDisplay();
    } catch (std::exception& e) {
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}


void MainWindowString::onZip() {
    if (!currentSeq || sequences.size() < 2) {
        QMessageBox::warning(this, "Ошибка", "Нужна вторая последовательность");
        return;
    }
    QStringList names = sequences.keys();
    QString other = QInputDialog::getItem(this, "Zip", "Вторая последовательность:", names, 0, false);
    if (other.isEmpty() || other == currentName) return;

    Sequence<std::tuple<std::string, std::string>>* zipped = ZipSequences(currentSeq, sequences[other]);
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


void MainWindowString::onUnzip() {
    if (!currentZipped) {
        QMessageBox::warning(this, "Ошибка", "Выбранная последовательность не является zipped");
        return;
    }
    auto it = zippedSeqs.find(currentName);
    if (it == zippedSeqs.end()) {
        QMessageBox::warning(this, "Ошибка", "Zipped последовательность не найдена");
        return;
    }
    auto [first, second] = UnzipSequences<std::string>(it.value());
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


void MainWindowString::onSequenceClicked(QListWidgetItem *item) {
    QString displayName = item->text();

    QString realName = displayName;
    bool isZipped = false;
    if (displayName.endsWith(" [zipped]")) {
        realName = displayName.left(displayName.length() - 9);
        isZipped = true;
    }
    if (isZipped) {
        if (!zippedSeqs.contains(realName)) return;
        currentName = realName;
        currentZipped = zippedSeqs[realName];
        currentSeq = nullptr;
        labelCurrent->setText(QString("Current (zipped): %1").arg(realName));
    } else {
        if (!sequences.contains(realName)) return;
        currentName = realName;
        currentSeq = sequences[realName];
        currentZipped = nullptr;
        labelCurrent->setText(QString("Current: %1").arg(realName));
    }
    updateCurrentDisplay();
}

#endif // WINDOWSTRING_H