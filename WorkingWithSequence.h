#ifndef LAZYSEQUENCEUI_H
#define LAZYSEQUENCEUI_H

#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "ArraySequence.h"
#include "LazySequence.h"

template<typename T>
QString valueToString(const T& value)
{
    if constexpr (std::is_same_v<T, int>) {
        return QString::number(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return QString::number(value);
    } else if constexpr (std::is_same_v<T, char>) {
        return QString(QChar::fromLatin1(value));
    } else if constexpr (std::is_same_v<T, std::string>) {
        return QString::fromStdString(value);
    } else {
        return QStringLiteral("<неподдерживаемый тип>");
    }
}

inline qulonglong toQtSize(size_t value)
{
    return static_cast<qulonglong>(value);
}

inline std::string addDecimalStrings(const std::string& a, const std::string& b)
{
    std::string result;
    result.reserve(std::max(a.size(), b.size()) + 1);

    int carry = 0;
    std::ptrdiff_t i = static_cast<std::ptrdiff_t>(a.size()) - 1;
    std::ptrdiff_t j = static_cast<std::ptrdiff_t>(b.size()) - 1;

    while (i >= 0 || j >= 0 || carry != 0) {
        int sum = carry;

        if (i >= 0)
            sum += a[static_cast<size_t>(i--)] - '0';

        if (j >= 0)
            sum += b[static_cast<size_t>(j--)] - '0';

        result.push_back(static_cast<char>('0' + (sum % 10)));
        carry = sum / 10;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

class MainWindowLazySequence : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowLazySequence(QWidget* parent = nullptr);
    ~MainWindowLazySequence() override;

private slots:
    void onCreateSequence();
    void onSequenceClicked(QListWidgetItem* item);
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
    QListWidget* listSequences = nullptr;
    QTextEdit* textDisplay = nullptr;
    QLabel* labelCurrent = nullptr;
    QLabel* labelInfo = nullptr;

    QLineEdit* editIndex = nullptr;
    QLineEdit* editValue = nullptr;
    QLineEdit* editName = nullptr;

    QComboBox* comboMapFunc = nullptr;
    QComboBox* comboWherePred = nullptr;

    QPushButton* btnCreate = nullptr;
    QPushButton* btnGet = nullptr;
    QPushButton* btnAppend = nullptr;
    QPushButton* btnPrepend = nullptr;
    QPushButton* btnInsertAt = nullptr;
    QPushButton* btnConcat = nullptr;
    QPushButton* btnMap = nullptr;
    QPushButton* btnWhere = nullptr;
    QPushButton* btnZip = nullptr;
    QPushButton* btnUnzip = nullptr;
    QPushButton* btnTake = nullptr;
    QPushButton* btnReduce = nullptr;
    QPushButton* btnRunTests = nullptr;

    QMap<QString, LazySequence<int>*> seqMap;
    QMap<QString, LazySequence<std::pair<int, int>>*> zippedMap;
    QMap<QString, LazySequence<std::string>*> fibonacciMap;
    QSet<QString> zippedNames;

    QString currentName;
    LazySequence<int>* currentSeq = nullptr;
    LazySequence<std::string>* currentFibonacci = nullptr;

private:
    void setupUI();
    void updateSequenceList();
    void updateCurrentDisplay();
    void clearCurrent();
    void selectSequence(const QString& name);
    void selectZippedSequence(const QString& name);
    void selectFibonacciSequence(const QString& name);

    void showMessage(const QString& message, bool isError = false);
    LazySequence<int>* getCurrentSequence();

    bool nameExists(const QString& name) const;
};

inline MainWindowLazySequence::MainWindowLazySequence(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    updateSequenceList();
}

inline MainWindowLazySequence::~MainWindowLazySequence()
{
    qDeleteAll(seqMap);
    qDeleteAll(zippedMap);
    qDeleteAll(fibonacciMap);
}

inline void MainWindowLazySequence::setupUI()
{
    setWindowTitle(QStringLiteral("LazySequence Lab4"));
    resize(950, 650);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    

    QHBoxLayout* topLayout = new QHBoxLayout;

    listSequences = new QListWidget;
    listSequences->setFixedWidth(270);
    topLayout->addWidget(listSequences);

    textDisplay = new QTextEdit;
    textDisplay->setReadOnly(true);
    topLayout->addWidget(textDisplay, 1);

    mainLayout->addLayout(topLayout, 1);

    

    QHBoxLayout* infoLayout = new QHBoxLayout;

    labelCurrent = new QLabel(QStringLiteral("Текущая: не выбрана"));
    labelInfo = new QLabel(QStringLiteral("Информация: —"));

    infoLayout->addWidget(labelCurrent);
    infoLayout->addStretch();
    infoLayout->addWidget(labelInfo);

    mainLayout->addLayout(infoLayout);

    

    QHBoxLayout* inputLayout = new QHBoxLayout;

    editIndex = new QLineEdit;
    editIndex->setPlaceholderText(QStringLiteral("Индекс / N"));

    editValue = new QLineEdit;
    editValue->setPlaceholderText(QStringLiteral("Значение"));

    editName = new QLineEdit;
    editName->setPlaceholderText(QStringLiteral("Имя новой последовательности"));

    inputLayout->addWidget(editIndex);
    inputLayout->addWidget(editValue);
    inputLayout->addWidget(editName);

    mainLayout->addLayout(inputLayout);

    

    QHBoxLayout* comboLayout = new QHBoxLayout;

    comboMapFunc = new QComboBox;
    comboMapFunc->addItems({
        QStringLiteral("+1"),
        QStringLiteral("*2"),
        QStringLiteral("квадрат")
    });

    comboWherePred = new QComboBox;
    comboWherePred->addItems({
        QStringLiteral("чётные"),
        QStringLiteral("положительные")
    });

    comboLayout->addWidget(new QLabel(QStringLiteral("Map:")));
    comboLayout->addWidget(comboMapFunc);
    comboLayout->addSpacing(20);
    comboLayout->addWidget(new QLabel(QStringLiteral("Where:")));
    comboLayout->addWidget(comboWherePred);
    comboLayout->addStretch();

    mainLayout->addLayout(comboLayout);

    

    QGridLayout* buttonGrid = new QGridLayout;

    btnCreate = new QPushButton(QStringLiteral("Создать"));
    btnGet = new QPushButton(QStringLiteral("Get"));
    btnAppend = new QPushButton(QStringLiteral("Append"));
    btnPrepend = new QPushButton(QStringLiteral("Prepend"));
    btnInsertAt = new QPushButton(QStringLiteral("InsertAt"));
    btnConcat = new QPushButton(QStringLiteral("Concat"));
    btnMap = new QPushButton(QStringLiteral("Map"));
    btnWhere = new QPushButton(QStringLiteral("Where"));
    btnZip = new QPushButton(QStringLiteral("Zip"));
    btnUnzip = new QPushButton(QStringLiteral("Unzip"));
    btnTake = new QPushButton(QStringLiteral("Take"));
    btnReduce = new QPushButton(QStringLiteral("Reduce"));
    btnRunTests = new QPushButton(QStringLiteral("Запустить модульные тесты"));

    buttonGrid->addWidget(btnCreate, 0, 0);
    buttonGrid->addWidget(btnGet, 0, 1);
    buttonGrid->addWidget(btnAppend, 0, 2);
    buttonGrid->addWidget(btnPrepend, 0, 3);

    buttonGrid->addWidget(btnInsertAt, 1, 0);
    buttonGrid->addWidget(btnConcat, 1, 1);
    buttonGrid->addWidget(btnMap, 1, 2);
    buttonGrid->addWidget(btnWhere, 1, 3);

    buttonGrid->addWidget(btnZip, 2, 0);
    buttonGrid->addWidget(btnUnzip, 2, 1);
    buttonGrid->addWidget(btnTake, 2, 2);
    buttonGrid->addWidget(btnReduce, 2, 3);

    buttonGrid->addWidget(btnRunTests, 3, 0, 1, 4);

    mainLayout->addLayout(buttonGrid);

    

    connect(listSequences,
            &QListWidget::itemClicked,
            this,
            &MainWindowLazySequence::onSequenceClicked);

    connect(btnCreate,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onCreateSequence);

    connect(btnGet,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onGetElement);

    connect(btnAppend,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onAppend);

    connect(btnPrepend,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onPrepend);

    connect(btnInsertAt,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onInsertAt);

    connect(btnConcat,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onConcat);

    connect(btnMap,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onMap);

    connect(btnWhere,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onWhere);

    connect(btnZip,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onZip);

    connect(btnUnzip,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onUnzip);

    connect(btnTake,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onTake);

    connect(btnReduce,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onReduce);

    connect(btnRunTests,
            &QPushButton::clicked,
            this,
            &MainWindowLazySequence::onRunTests);
}

inline bool MainWindowLazySequence::nameExists(const QString& name) const
{
    return seqMap.contains(name) ||
           zippedMap.contains(name) ||
           fibonacciMap.contains(name);
}

inline void MainWindowLazySequence::showMessage(const QString& message,
                                                bool isError)
{
    if (isError) {
        QMessageBox::critical(this, QStringLiteral("Ошибка"), message);
    } else {
        QMessageBox::information(this, QStringLiteral("Результат"), message);
    }
}

inline LazySequence<int>* MainWindowLazySequence::getCurrentSequence()
{
    if (!currentSeq) {
        if (zippedMap.contains(currentName)) {
            showMessage(
                QStringLiteral(
                    "Выбрана zipped-последовательность. "
                    "Для этой операции выберите обычную последовательность."),
                true);
        } else if (fibonacciMap.contains(currentName)) {
            showMessage(
                QStringLiteral(
                    "Для последовательности Фибоначчи в этом окне доступен Get. "
                    "Она хранит значения как числа произвольной длины."),
                true);
        } else {
            showMessage(QStringLiteral("Нет выбранной последовательности"), true);
        }
        return nullptr;
    }

    return currentSeq;
}

inline void MainWindowLazySequence::updateSequenceList()
{
    listSequences->clear();

    for (const QString& name : seqMap.keys()) {
        listSequences->addItem(name);
    }

    for (const QString& name : zippedMap.keys()) {
        listSequences->addItem(name + QStringLiteral(" [zipped]"));
    }

    for (const QString& name : fibonacciMap.keys()) {
        listSequences->addItem(name + QStringLiteral(" [Fibonacci]"));
    }
}

inline void MainWindowLazySequence::clearCurrent()
{
    currentName.clear();
    currentSeq = nullptr;
    currentFibonacci = nullptr;

    labelCurrent->setText(QStringLiteral("Текущая: не выбрана"));
    updateCurrentDisplay();
}

inline void MainWindowLazySequence::selectSequence(const QString& name)
{
    if (!seqMap.contains(name)) {
        clearCurrent();
        return;
    }

    currentName = name;
    currentSeq = seqMap.value(name);
    currentFibonacci = nullptr;

    labelCurrent->setText(
        QStringLiteral("Текущая: %1").arg(name));

    updateCurrentDisplay();
}

inline void MainWindowLazySequence::selectZippedSequence(const QString& name)
{
    if (!zippedMap.contains(name)) {
        clearCurrent();
        return;
    }

    currentName = name;
    currentSeq = nullptr;
    currentFibonacci = nullptr;

    labelCurrent->setText(
        QStringLiteral("Текущая: %1 [zipped]").arg(name));

    updateCurrentDisplay();
}

inline void MainWindowLazySequence::selectFibonacciSequence(const QString& name)
{
    if (!fibonacciMap.contains(name)) {
        clearCurrent();
        return;
    }

    currentName = name;
    currentSeq = nullptr;
    currentFibonacci = fibonacciMap.value(name);

    labelCurrent->setText(
        QStringLiteral("Текущая: %1 [Fibonacci]").arg(name));

    updateCurrentDisplay();
}

inline void MainWindowLazySequence::updateCurrentDisplay()
{
    textDisplay->clear();

    if (currentName.isEmpty()) {
        labelInfo->setText(QStringLiteral("Информация: —"));
        return;
    }

    if (fibonacciMap.contains(currentName)) {
        LazySequence<std::string>* fibonacci =
            fibonacciMap.value(currentName, nullptr);

        if (!fibonacci) {
            clearCurrent();
            return;
        }

        const size_t computed = fibonacci->ComputedCount();
        const size_t shown = std::min<size_t>(120, computed);

        QString display;

        for (size_t i = 0; i < shown; ++i) {
            display += QString::fromStdString(fibonacci->Get(i));
            display += QLatin1Char(' ');
        }

        if (computed == 0) {
            display = QStringLiteral(
                "Элементы ещё не материализованы. Используйте Get.");
        } else if (computed > shown) {
            display += QStringLiteral("...");
        }

        display += QStringLiteral("\n(потенциально бесконечная)");

        labelInfo->setText(
            QStringLiteral("Кардинал: ℵ₀, вычислено: %1")
                .arg(toQtSize(computed)));

        textDisplay->setPlainText(display);
        return;
    }

    

    if (zippedMap.contains(currentName)) {
        LazySequence<std::pair<int, int>>* zipped =
            zippedMap.value(currentName, nullptr);

        if (!zipped) {
            clearCurrent();
            return;
        }

        const size_t computed = zipped->ComputedCount();
        const size_t shown = std::min<size_t>(20, computed);

        QString display;

        for (size_t i = 0; i < shown; ++i) {
            const auto pair = zipped->Get(i);
            display += QStringLiteral("(%1,%2) ")
                           .arg(pair.first)
                           .arg(pair.second);
        }

        if (computed == 0) {
            display = QStringLiteral(
                "Элементы ещё не материализованы.");
        } else if (computed > shown) {
            display += QStringLiteral("...");
        }

        if (zipped->IsEndless()) {
            display += QStringLiteral("\n(потенциально бесконечная)");

            labelInfo->setText(
                QStringLiteral("Кардинал: ℵ₀, вычислено: %1")
                    .arg(toQtSize(computed)));
        } else {
            const size_t length =
                zipped->GetCardinality().GetValue();

            labelInfo->setText(
                QStringLiteral(
                    "Кардинал: конечная, длина: %1, вычислено: %2")
                    .arg(toQtSize(length))
                    .arg(toQtSize(computed)));
        }

        textDisplay->setPlainText(display);
        return;
    }

    

    if (!currentSeq || !seqMap.contains(currentName)) {
        clearCurrent();
        return;
    }

    const size_t computed = currentSeq->ComputedCount();
    const size_t shown = std::min<size_t>(50, computed);

    QString display;

    for (size_t i = 0; i < shown; ++i) {
        display += QString::number(currentSeq->Get(i));
        display += QLatin1Char(' ');
    }

    if (computed == 0) {
        if (!currentSeq->IsEndless() &&
            currentSeq->GetCardinality().GetValue() == 0) {
            display = QStringLiteral("Пустая последовательность.");
        } else {
            display = QStringLiteral(
                "Элементы ещё не материализованы. "
                "Используйте Get или другую операцию.");
        }
    } else if (computed > shown) {
        display += QStringLiteral("...");
    }

    if (currentSeq->IsEndless()) {
        display += QStringLiteral("\n(потенциально бесконечная)");

        labelInfo->setText(
            QStringLiteral("Кардинал: ℵ₀, вычислено: %1")
                .arg(toQtSize(computed)));
    } else {

        const size_t length =
            currentSeq->GetCardinality().GetValue();

        labelInfo->setText(
            QStringLiteral(
                "Кардинал: конечная, длина: %1, вычислено: %2")
                .arg(toQtSize(length))
                .arg(toQtSize(computed)));
    }

    textDisplay->setPlainText(display);
}

inline void MainWindowLazySequence::onSequenceClicked(QListWidgetItem* item)
{
    if (!item)
        return;

    const QString displayName = item->text();
    const QString zippedSuffix = QStringLiteral(" [zipped]");
    const QString fibonacciSuffix = QStringLiteral(" [Fibonacci]");

    if (displayName.endsWith(zippedSuffix)) {
        const QString realName =
            displayName.left(displayName.size() - zippedSuffix.size());

        selectZippedSequence(realName);
        return;
    }

    if (displayName.endsWith(fibonacciSuffix)) {
        const QString realName =
            displayName.left(displayName.size() - fibonacciSuffix.size());

        selectFibonacciSequence(realName);
        return;
    }

    selectSequence(displayName);
}

inline void MainWindowLazySequence::onCreateSequence()
{
    bool ok = false;

    QString name = QInputDialog::getText(
        this,
        QStringLiteral("Создать"),
        QStringLiteral("Имя последовательности:"),
        QLineEdit::Normal,
        QString(),
        &ok);

    name = name.trimmed();

    if (!ok)
        return;

    if (name.isEmpty()) {
        showMessage(QStringLiteral("Имя не может быть пустым"), true);
        return;
    }

    if (nameExists(name)) {
        showMessage(QStringLiteral("Имя уже существует"), true);
        return;
    }

    const QStringList choices = {
        QStringLiteral("Из массива"),
        QStringLiteral("Фибоначчи (бесконечная)"),
        QStringLiteral("Натуральные числа (бесконечная)")
    };

    const QString choice = QInputDialog::getItem(
        this,
        QStringLiteral("Способ создания"),
        QStringLiteral("Выберите тип:"),
        choices,
        0,
        false,
        &ok);

    if (!ok)
        return;

    LazySequence<int>* sequence = nullptr;

    try {
        if (choice == QStringLiteral("Из массива")) {
            const int size = QInputDialog::getInt(
                this,
                QStringLiteral("Размер"),
                QStringLiteral("Количество элементов:"),
                0,
                0,
                1000000,
                1,
                &ok);

            if (!ok)
                return;

            ArraySequence<int> data;

            for (int i = 0; i < size; ++i) {
                const int value = QInputDialog::getInt(
                    this,
                    QStringLiteral("Элемент"),
                    QStringLiteral("Элемент %1:").arg(i),
                    0,
                    std::numeric_limits<int>::min(),
                    std::numeric_limits<int>::max(),
                    1,
                    &ok);

                if (!ok)
                    return;

                data.Append(value);
            }

            sequence = new LazySequence<int>(data);
        }
        else if (choice ==
                 QStringLiteral("Фибоначчи (бесконечная)")) {
            auto fibRule =
                [](size_t index,
                   const ArraySequence<std::string>& previous) -> std::string {
                    if (index == 0)
                        return "0";

                    if (index == 1)
                        return "1";

                    return addDecimalStrings(
                        previous.Get(static_cast<int>(index - 1)),
                        previous.Get(static_cast<int>(index - 2)));
                };

            ArraySequence<std::string> seed;
            seed.Append("0");
            seed.Append("1");

            auto* fibonacci =
                new LazySequence<std::string>(fibRule, seed);

            fibonacciMap.insert(name, fibonacci);
            updateSequenceList();
            selectFibonacciSequence(name);
            return;
        }
        else if (choice ==
                 QStringLiteral(
                     "Натуральные числа (бесконечная)")) {
            auto naturalRule =
                [](size_t index,
                   const ArraySequence<int>&) -> int {
                    if (index >=
                        static_cast<size_t>(
                            std::numeric_limits<int>::max())) {
                        throw std::overflow_error(
                            "Natural number exceeds int range");
                    }

                    return static_cast<int>(index + 1);
                };

            ArraySequence<int> seed;

            sequence =
                new LazySequence<int>(naturalRule, seed);
        }

        if (!sequence)
            return;

        seqMap.insert(name, sequence);
        updateSequenceList();
        selectSequence(name);
    }
    catch (const std::exception& e) {
        delete sequence;

        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onGetElement()
{
    bool ok = false;
    const int index = editIndex->text().trimmed().toInt(&ok);

    if (!ok) {
        showMessage(
            QStringLiteral("Введите целый индекс"),
            true);
        return;
    }

    if (index < 0) {
        showMessage(
            QStringLiteral("Индекс не может быть отрицательным"),
            true);
        return;
    }

    try {
        if (currentFibonacci && fibonacciMap.contains(currentName)) {
            const std::string value =
                currentFibonacci->Get(index);

            showMessage(
                QStringLiteral("Элемент [%1] = %2")
                    .arg(index)
                    .arg(QString::fromStdString(value)));

            updateCurrentDisplay();
            return;
        }

        LazySequence<int>* sequence = getCurrentSequence();

        if (!sequence)
            return;

        const int value = sequence->Get(index);

        showMessage(
            QStringLiteral("Элемент [%1] = %2")
                .arg(index)
                .arg(value));

        updateCurrentDisplay();
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onAppend()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    bool ok = false;
    const int value =
        editValue->text().trimmed().toInt(&ok);

    if (!ok) {
        showMessage(
            QStringLiteral("Введите целое значение"),
            true);
        return;
    }

    const QString newName = editName->text().trimmed();

    if (newName.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Введите имя новой последовательности"),
            true);
        return;
    }

    if (nameExists(newName)) {
        showMessage(
            QStringLiteral("Имя уже существует"),
            true);
        return;
    }

    const QString sourceName = currentName;

    try {
        auto* newSequence =
            new LazySequence<int>(sequence->Append(value));

        seqMap.insert(newName, newSequence);
        updateSequenceList();
        selectSequence(newName);

        showMessage(
            QStringLiteral(
                "Создана '%1' = %2.Append(%3)")
                .arg(newName)
                .arg(sourceName)
                .arg(value));
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onPrepend()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    bool ok = false;
    const int value =
        editValue->text().trimmed().toInt(&ok);

    if (!ok) {
        showMessage(
            QStringLiteral("Введите целое значение"),
            true);
        return;
    }

    const QString newName = editName->text().trimmed();

    if (newName.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Введите имя новой последовательности"),
            true);
        return;
    }

    if (nameExists(newName)) {
        showMessage(
            QStringLiteral("Имя уже существует"),
            true);
        return;
    }

    const QString sourceName = currentName;

    try {
        auto* newSequence =
            new LazySequence<int>(sequence->Prepend(value));

        seqMap.insert(newName, newSequence);
        updateSequenceList();
        selectSequence(newName);

        showMessage(
            QStringLiteral(
                "Создана '%1' = %2.Prepend(%3)")
                .arg(newName)
                .arg(sourceName)
                .arg(value));
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onInsertAt()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    bool ok = false;
    const int index =
        editIndex->text().trimmed().toInt(&ok);

    if (!ok) {
        showMessage(
            QStringLiteral("Введите целый индекс"),
            true);
        return;
    }

    if (index < 0) {
        showMessage(
            QStringLiteral("Индекс не может быть отрицательным"),
            true);
        return;
    }

    const QString newName = editName->text().trimmed();

    if (newName.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Введите имя новой последовательности"),
            true);
        return;
    }

    if (nameExists(newName)) {
        showMessage(
            QStringLiteral("Имя уже существует"),
            true);
        return;
    }

    const QMessageBox::StandardButton choice =
        QMessageBox::question(
            this,
            QStringLiteral("InsertAt"),
            QStringLiteral(
                "Да — вставить один элемент.\n"
                "Нет — вставить другую LazySequence."),
            QMessageBox::Yes |
                QMessageBox::No |
                QMessageBox::Cancel);

    if (choice == QMessageBox::Cancel)
        return;

    try {
        LazySequence<int>* newSequence = nullptr;

        if (choice == QMessageBox::Yes) {
            const int value =
                editValue->text().trimmed().toInt(&ok);

            if (!ok) {
                showMessage(
                    QStringLiteral("Введите целое значение"),
                    true);
                return;
            }

            newSequence =
                new LazySequence<int>(
                    sequence->InsertAt(index, value));
        } else {
            QStringList names = seqMap.keys();

            if (names.isEmpty()) {
                showMessage(
                    QStringLiteral(
                        "Нет последовательностей для вставки"),
                    true);
                return;
            }

            const QString otherName =
                QInputDialog::getItem(
                    this,
                    QStringLiteral(
                        "Вставить последовательность"),
                    QStringLiteral(
                        "Выберите последовательность:"),
                    names,
                    0,
                    false,
                    &ok);

            if (!ok || otherName.isEmpty())
                return;

            LazySequence<int>* other =
                seqMap.value(otherName, nullptr);

            if (!other) {
                showMessage(
                    QStringLiteral(
                        "Выбранная последовательность не найдена"),
                    true);
                return;
            }

            newSequence =
                new LazySequence<int>(
                    sequence->InsertAt(index, *other));
        }

        seqMap.insert(newName, newSequence);
        updateSequenceList();
        selectSequence(newName);

        showMessage(
            QStringLiteral(
                "Создана '%1': вставка в позицию %2")
                .arg(newName)
                .arg(index));
    }
    catch (const std::exception& e) {
        showMessage(
            QStringLiteral("Ошибка InsertAt: %1")
                .arg(QString::fromUtf8(e.what())),
            true);
    }
}

inline void MainWindowLazySequence::onConcat()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    QStringList names = seqMap.keys();
    names.removeAll(currentName);

    if (names.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Нет другой последовательности для Concat"),
            true);
        return;
    }

    bool ok = false;

    const QString otherName =
        QInputDialog::getItem(
            this,
            QStringLiteral("Concat"),
            QStringLiteral("Вторая последовательность:"),
            names,
            0,
            false,
            &ok);

    if (!ok || otherName.isEmpty())
        return;

    const QString newName = editName->text().trimmed();

    if (newName.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Введите имя новой последовательности"),
            true);
        return;
    }

    if (nameExists(newName)) {
        showMessage(
            QStringLiteral("Имя уже существует"),
            true);
        return;
    }

    LazySequence<int>* other =
        seqMap.value(otherName, nullptr);

    if (!other) {
        showMessage(
            QStringLiteral(
                "Вторая последовательность не найдена"),
            true);
        return;
    }

    const QString firstName = currentName;

    try {
        auto* result =
            new LazySequence<int>(
                sequence->Concat(*other));

        seqMap.insert(newName, result);
        updateSequenceList();
        selectSequence(newName);

        showMessage(
            QStringLiteral(
                "Создана '%1' = %2.Concat(%3)")
                .arg(newName)
                .arg(firstName)
                .arg(otherName));
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onMap()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    std::function<int(const int&)> function;

    switch (comboMapFunc->currentIndex()) {
    case 0:
        function = [](const int& value) {
            return value + 1;
        };
        break;

    case 1:
        function = [](const int& value) {
            return value * 2;
        };
        break;

    default:
        function = [](const int& value) {
            return value * value;
        };
        break;
    }

    const QString newName = editName->text().trimmed();

    if (newName.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Введите имя новой последовательности"),
            true);
        return;
    }

    if (nameExists(newName)) {
        showMessage(
            QStringLiteral("Имя уже существует"),
            true);
        return;
    }

    try {
        auto* mapped =
            new LazySequence<int>(
                sequence->Map<int>(function));

        seqMap.insert(newName, mapped);
        updateSequenceList();
        selectSequence(newName);

        showMessage(
            QStringLiteral("Создана '%1' = Map(%2)")
                .arg(newName)
                .arg(comboMapFunc->currentText()));
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onWhere()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    std::function<bool(const int&)> predicate;

    if (comboWherePred->currentIndex() == 0) {
        predicate = [](const int& value) {
            return value % 2 == 0;
        };
    } else {
        predicate = [](const int& value) {
            return value > 0;
        };
    }

    const QString newName = editName->text().trimmed();

    if (newName.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Введите имя новой последовательности"),
            true);
        return;
    }

    if (nameExists(newName)) {
        showMessage(
            QStringLiteral("Имя уже существует"),
            true);
        return;
    }

    try {
        auto* filtered =
            new LazySequence<int>(
                sequence->Where(predicate));

        seqMap.insert(newName, filtered);
        updateSequenceList();
        selectSequence(newName);

        showMessage(
            QStringLiteral("Создана '%1' = Where(%2)")
                .arg(newName)
                .arg(comboWherePred->currentText()));
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onZip()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    QStringList names = seqMap.keys();
    names.removeAll(currentName);

    if (names.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Нет другой последовательности для Zip"),
            true);
        return;
    }

    bool ok = false;

    const QString otherName =
        QInputDialog::getItem(
            this,
            QStringLiteral("Zip"),
            QStringLiteral("Вторая последовательность:"),
            names,
            0,
            false,
            &ok);

    if (!ok || otherName.isEmpty())
        return;

    const QString newName = editName->text().trimmed();

    if (newName.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Введите имя zipped-последовательности"),
            true);
        return;
    }

    if (nameExists(newName)) {
        showMessage(
            QStringLiteral("Имя уже существует"),
            true);
        return;
    }

    LazySequence<int>* other =
        seqMap.value(otherName, nullptr);

    if (!other) {
        showMessage(
            QStringLiteral(
                "Вторая последовательность не найдена"),
            true);
        return;
    }

    try {
        auto* zipped =
            new LazySequence<std::pair<int, int>>(
                sequence->Zip(*other));

        zippedMap.insert(newName, zipped);
        zippedNames.insert(newName);

        updateSequenceList();
        selectZippedSequence(newName);

        showMessage(
            QStringLiteral(
                "Создана zipped-последовательность '%1'")
                .arg(newName));
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onUnzip()
{
    if (!zippedMap.contains(currentName)) {
        showMessage(
            QStringLiteral(
                "Сначала выберите zipped-последовательность"),
            true);
        return;
    }

    LazySequence<std::pair<int, int>>* zipped =
        zippedMap.value(currentName, nullptr);

    if (!zipped) {
        showMessage(
            QStringLiteral(
                "Zipped-последовательность не найдена"),
            true);
        return;
    }

    bool ok = false;

    QString firstName =
        QInputDialog::getText(
            this,
            QStringLiteral("Unzip"),
            QStringLiteral(
                "Имя первой последовательности:"),
            QLineEdit::Normal,
            QString(),
            &ok);

    firstName = firstName.trimmed();

    if (!ok)
        return;

    QString secondName =
        QInputDialog::getText(
            this,
            QStringLiteral("Unzip"),
            QStringLiteral(
                "Имя второй последовательности:"),
            QLineEdit::Normal,
            QString(),
            &ok);

    secondName = secondName.trimmed();

    if (!ok)
        return;

    if (firstName.isEmpty() || secondName.isEmpty()) {
        showMessage(
            QStringLiteral("Имена не могут быть пустыми"),
            true);
        return;
    }

    if (firstName == secondName) {
        showMessage(
            QStringLiteral(
                "Имена результатов Unzip должны отличаться"),
            true);
        return;
    }

    if (nameExists(firstName) || nameExists(secondName)) {
        showMessage(
            QStringLiteral(
                "Одно из указанных имён уже существует"),
            true);
        return;
    }

    try {
        auto [first, second] = Unzip(*zipped);

        seqMap.insert(
            firstName,
            new LazySequence<int>(std::move(first)));

        seqMap.insert(
            secondName,
            new LazySequence<int>(std::move(second)));

        updateSequenceList();
        selectSequence(firstName);

        showMessage(
            QStringLiteral(
                "Unzip выполнен: созданы '%1' и '%2'")
                .arg(firstName)
                .arg(secondName));
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onTake()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    bool ok = false;
    const qlonglong nValue =
        editIndex->text().trimmed().toLongLong(&ok);

    if (!ok || nValue < 0) {
        showMessage(
            QStringLiteral(
                "Введите неотрицательное число N"),
            true);
        return;
    }

    const auto nUnsigned =
        static_cast<unsigned long long>(nValue);

    if (nUnsigned >
        static_cast<unsigned long long>(
            std::numeric_limits<size_t>::max())) {
        showMessage(
            QStringLiteral("N слишком велико"),
            true);
        return;
    }

    const size_t n =
        static_cast<size_t>(nUnsigned);

    const QString newName = editName->text().trimmed();

    if (newName.isEmpty()) {
        showMessage(
            QStringLiteral(
                "Введите имя новой последовательности"),
            true);
        return;
    }

    if (nameExists(newName)) {
        showMessage(
            QStringLiteral("Имя уже существует"),
            true);
        return;
    }

    try {

        auto* taken =
            new LazySequence<int>(sequence->Take(n));

        seqMap.insert(newName, taken);
        updateSequenceList();
        selectSequence(newName);

        showMessage(
            QStringLiteral(
                "Создана '%1' = первые %2 элементов")
                .arg(newName)
                .arg(toQtSize(n)));
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onReduce()
{
    LazySequence<int>* sequence = getCurrentSequence();

    if (!sequence)
        return;

    try {
        if (sequence->IsEndless()) {
            bool ok = false;

            const qulonglong count =
                editIndex->text().trimmed().toULongLong(&ok);

            if (!ok || count == 0) {
                showMessage(
                    QStringLiteral(
                        "Для бесконечной последовательности "
                        "укажите в поле индекса количество "
                        "элементов N > 0"),
                    true);
                return;
            }

            if (count >
                static_cast<qulonglong>(
                    std::numeric_limits<size_t>::max())) {
                showMessage(
                    QStringLiteral(
                        "Количество элементов слишком велико"),
                    true);
                return;
            }

            const size_t maxCount =
                static_cast<size_t>(count);

            const int sum =
                sequence->Reduce(
                    [](const int& a, const int& b) {
                        return a + b;
                    },
                    0,
                    maxCount);

            showMessage(
                QStringLiteral(
                    "Сумма первых %1 элементов = %2")
                    .arg(count)
                    .arg(sum));
        } else {
            const int sum =
                sequence->Reduce(
                    [](const int& a, const int& b) {
                        return a + b;
                    },
                    0);

            showMessage(
                QStringLiteral(
                    "Сумма всех элементов = %1")
                    .arg(sum));
        }

        updateCurrentDisplay();
    }
    catch (const std::exception& e) {
        showMessage(
            QString::fromUtf8(e.what()),
            true);
    }
}

inline void MainWindowLazySequence::onRunTests()
{
    QDialog* testDialog = new QDialog(this);
    testDialog->setWindowTitle(
        QStringLiteral("Результаты модульных тестов"));
    testDialog->resize(900, 700);

    QVBoxLayout* layout = new QVBoxLayout(testDialog);

    QTextEdit* output = new QTextEdit;
    output->setReadOnly(true);
    output->setFontFamily(QStringLiteral("Courier New"));

    layout->addWidget(output, 1);

    QPushButton* closeButton =
        new QPushButton(QStringLiteral("Закрыть"));

    layout->addWidget(closeButton);

    connect(closeButton,
            &QPushButton::clicked,
            testDialog,
            &QDialog::accept);

    output->append(
        QStringLiteral(
            "=== Модульное тестирование LazySequence<int> ===\n"));

    int passedCount = 0;
    int failedCount = 0;

    auto runTest =
        [&output, &passedCount, &failedCount](
            int testNumber,
            const QString& description,
            const QString& inputData,
            const QString& expected,
            const QString& actual,
            bool passed)
        {
            if (passed)
                ++passedCount;
            else
                ++failedCount;

            output->append(
                QStringLiteral("\nТест №%1")
                    .arg(testNumber));

            output->append(
                QStringLiteral("Описание: %1")
                    .arg(description));

            output->append(
                QStringLiteral("Входные данные: %1")
                    .arg(inputData));

            output->append(
                QStringLiteral("Ожидаемый вывод: %1")
                    .arg(expected));

            output->append(
                QStringLiteral("Вывод программы: %1")
                    .arg(actual));

            output->append(
                QStringLiteral("Вердикт: %1")
                    .arg(passed ?
                             QStringLiteral("Pass") :
                             QStringLiteral("Failed")));

            output->append(
                QStringLiteral(
                    "----------------------------------------"));
        };

    int testNumber = 1;

    

    {
        ArraySequence<int> data;

        for (int i = 1; i <= 5; ++i)
            data.Append(i);

        LazySequence<int> sequence(data);

        const size_t before = sequence.ComputedCount();
        const int first = sequence.Get(0);
        const size_t afterFirst = sequence.ComputedCount();
        const int last = sequence.Get(4);
        const size_t afterLast = sequence.ComputedCount();
        const size_t length =
            sequence.GetCardinality().GetValue();

        const bool passed =
            before == 0 &&
            first == 1 &&
            afterFirst == 1 &&
            last == 5 &&
            afterLast == 5 &&
            length == 5;

        runTest(
            testNumber++,
            QStringLiteral(
                "Создание из ArraySequence и мемоизация"),
            QStringLiteral("[1,2,3,4,5]"),
            QStringLiteral(
                "длина=5; до Get вычислено 0; "
                "после Get(0) вычислен 1 элемент; Get(4)=5"),
            QStringLiteral(
                "длина=%1; до=%2; после Get(0)=%3; "
                "Get(0)=%4; Get(4)=%5; после Get(4)=%6")
                .arg(toQtSize(length))
                .arg(toQtSize(before))
                .arg(toQtSize(afterFirst))
                .arg(first)
                .arg(last)
                .arg(toQtSize(afterLast)),
            passed);
    }

    

    {
        ArraySequence<int> data;

        for (int i = 1; i <= 5; ++i)
            data.Append(i);

        LazySequence<int> sequence(data);
        auto result = sequence.Append(6);

        const size_t length =
            result.GetCardinality().GetValue();

        const bool initiallyLazy =
            result.ComputedCount() == 0;

        bool valuesCorrect =
            length == 6 &&
            result.Get(0) == 1 &&
            result.Get(1) == 2 &&
            result.Get(2) == 3 &&
            result.Get(3) == 4 &&
            result.Get(4) == 5 &&
            result.Get(5) == 6;

        const bool passed =
            initiallyLazy && valuesCorrect;

        runTest(
            testNumber++,
            QStringLiteral("Append"),
            QStringLiteral(
                "[1,2,3,4,5].Append(6)"),
            QStringLiteral(
                "[1,2,3,4,5,6], длина=6"),
            QStringLiteral(
                "длина=%1; результат=[%2,%3,%4,%5,%6,%7]")
                .arg(toQtSize(length))
                .arg(result.Get(0))
                .arg(result.Get(1))
                .arg(result.Get(2))
                .arg(result.Get(3))
                .arg(result.Get(4))
                .arg(result.Get(5)),
            passed);
    }

    

    {
        ArraySequence<int> data;

        for (int i = 1; i <= 5; ++i)
            data.Append(i);

        LazySequence<int> sequence(data);
        auto result = sequence.Prepend(0);

        const size_t length =
            result.GetCardinality().GetValue();

        const int first = result.Get(0);
        const size_t computedAfterFirst =
            result.ComputedCount();

        const bool passed =
            length == 6 &&
            first == 0 &&
            computedAfterFirst == 1 &&
            result.Get(1) == 1 &&
            result.Get(2) == 2 &&
            result.Get(3) == 3 &&
            result.Get(4) == 4 &&
            result.Get(5) == 5;

        runTest(
            testNumber++,
            QStringLiteral("Prepend"),
            QStringLiteral(
                "[1,2,3,4,5].Prepend(0)"),
            QStringLiteral(
                "[0,1,2,3,4,5], длина=6; "
                "после Get(0) вычислен только 1 элемент"),
            QStringLiteral(
                "длина=%1; Get(0)=%2; "
                "вычислено после Get(0)=%3; "
                "последний=%4")
                .arg(toQtSize(length))
                .arg(first)
                .arg(toQtSize(computedAfterFirst))
                .arg(result.Get(5)),
            passed);
    }

    

    {
        ArraySequence<int> data;

        for (int i = 1; i <= 5; ++i)
            data.Append(i);

        LazySequence<int> sequence(data);
        auto result = sequence.InsertAt(2, 99);

        const size_t length =
            result.GetCardinality().GetValue();

        const int inserted = result.Get(2);
        const size_t computedAfterGet =
            result.ComputedCount();

        const bool passed =
            length == 6 &&
            inserted == 99 &&
            computedAfterGet == 3 &&
            result.Get(0) == 1 &&
            result.Get(1) == 2 &&
            result.Get(3) == 3 &&
            result.Get(4) == 4 &&
            result.Get(5) == 5;

        runTest(
            testNumber++,
            QStringLiteral("InsertAt"),
            QStringLiteral(
                "в [1,2,3,4,5] вставить 99 в позицию 2"),
            QStringLiteral(
                "[1,2,99,3,4,5], длина=6"),
            QStringLiteral(
                "длина=%1; element[2]=%2; "
                "вычислено после Get(2)=%3")
                .arg(toQtSize(length))
                .arg(inserted)
                .arg(toQtSize(computedAfterGet)),
            passed);
    }

    

    {
        ArraySequence<int> firstData;
        ArraySequence<int> secondData;

        for (int i = 1; i <= 5; ++i)
            firstData.Append(i);

        secondData.Append(10);
        secondData.Append(20);

        LazySequence<int> first(firstData);
        LazySequence<int> second(secondData);

        auto result = first.Concat(second);

        const size_t length =
            result.GetCardinality().GetValue();

        const bool passed =
            length == 7 &&
            result.Get(0) == 1 &&
            result.Get(4) == 5 &&
            result.Get(5) == 10 &&
            result.Get(6) == 20;

        runTest(
            testNumber++,
            QStringLiteral("Concat"),
            QStringLiteral(
                "[1,2,3,4,5] + [10,20]"),
            QStringLiteral(
                "[1,2,3,4,5,10,20], длина=7"),
            QStringLiteral(
                "длина=%1; [5]=%2; [6]=%3")
                .arg(toQtSize(length))
                .arg(result.Get(5))
                .arg(result.Get(6)),
            passed);
    }

    

    {
        ArraySequence<int> data;

        for (int i = 1; i <= 5; ++i)
            data.Append(i);

        LazySequence<int> sequence(data);

        auto mapped =
            sequence.Map<int>(
                [](const int& value) {
                    return value * 2;
                });

        const size_t length =
            mapped.GetCardinality().GetValue();

        const bool passed =
            length == 5 &&
            mapped.Get(0) == 2 &&
            mapped.Get(1) == 4 &&
            mapped.Get(2) == 6 &&
            mapped.Get(3) == 8 &&
            mapped.Get(4) == 10;

        runTest(
            testNumber++,
            QStringLiteral("Map"),
            QStringLiteral(
                "[1,2,3,4,5], функция x*2"),
            QStringLiteral("[2,4,6,8,10]"),
            QStringLiteral(
                "[%1,%2,%3,%4,%5], длина=%6")
                .arg(mapped.Get(0))
                .arg(mapped.Get(1))
                .arg(mapped.Get(2))
                .arg(mapped.Get(3))
                .arg(mapped.Get(4))
                .arg(toQtSize(length)),
            passed);
    }

    

    {
        ArraySequence<int> data;

        for (int i = 1; i <= 5; ++i)
            data.Append(i);

        LazySequence<int> sequence(data);

        auto filtered =
            sequence.Where(
                [](const int& value) {
                    return value % 2 == 0;
                });

        const size_t length =
            filtered.GetCardinality().GetValue();

        bool outOfRangeThrown = false;

        const int first = filtered.Get(0);
        const int second = filtered.Get(1);

        try {
            (void)filtered.Get(2);
        }
        catch (const IndexOutOFBoundsException&) {
            outOfRangeThrown = true;
        }

        const bool passed =
            length == 2 &&
            first == 2 &&
            second == 4 &&
            outOfRangeThrown;

        runTest(
            testNumber++,
            QStringLiteral("Where"),
            QStringLiteral(
                "[1,2,3,4,5], оставить чётные"),
            QStringLiteral(
                "[2,4], длина=2; Get(2) -> исключение"),
            QStringLiteral(
                "[%1,%2], длина=%3; "
                "исключение=%4")
                .arg(first)
                .arg(second)
                .arg(toQtSize(length))
                .arg(outOfRangeThrown ?
                         QStringLiteral("да") :
                         QStringLiteral("нет")),
            passed);
    }

    

    {
        ArraySequence<int> firstData;
        ArraySequence<int> secondData;

        for (int i = 1; i <= 5; ++i)
            firstData.Append(i);

        secondData.Append(10);
        secondData.Append(20);

        LazySequence<int> firstSequence(firstData);
        LazySequence<int> secondSequence(secondData);

        auto zipped =
            firstSequence.Zip(secondSequence);

        const size_t zippedLength =
            zipped.GetCardinality().GetValue();

        const auto pair0 = zipped.Get(0);
        const auto pair1 = zipped.Get(1);

        auto [first, second] = Unzip(zipped);

        const size_t firstLength =
            first.GetCardinality().GetValue();

        const size_t secondLength =
            second.GetCardinality().GetValue();

        const bool passed =
            zippedLength == 2 &&
            pair0.first == 1 &&
            pair0.second == 10 &&
            pair1.first == 2 &&
            pair1.second == 20 &&
            firstLength == 2 &&
            secondLength == 2 &&
            first.Get(0) == 1 &&
            first.Get(1) == 2 &&
            second.Get(0) == 10 &&
            second.Get(1) == 20;

        runTest(
            testNumber++,
            QStringLiteral("Zip и Unzip"),
            QStringLiteral(
                "[1,2,3,4,5] и [10,20]"),
            QStringLiteral(
                "Zip=[(1,10),(2,20)], длина=2; "
                "Unzip -> [1,2] и [10,20]"),
            QStringLiteral(
                "Zip=[(%1,%2),(%3,%4)], длина=%5; "
                "Unzip=[%6,%7] / [%8,%9]")
                .arg(pair0.first)
                .arg(pair0.second)
                .arg(pair1.first)
                .arg(pair1.second)
                .arg(toQtSize(zippedLength))
                .arg(first.Get(0))
                .arg(first.Get(1))
                .arg(second.Get(0))
                .arg(second.Get(1)),
            passed);
    }

    

    {
        ArraySequence<int> data;

        for (int i = 1; i <= 5; ++i)
            data.Append(i);

        LazySequence<int> sequence(data);
        auto taken = sequence.Take(3);

        const size_t length =
            taken.GetCardinality().GetValue();

        bool outOfRangeThrown = false;

        try {
            (void)taken.Get(3);
        }
        catch (const IndexOutOFBoundsException&) {
            outOfRangeThrown = true;
        }

        const bool passed =
            length == 3 &&
            taken.Get(0) == 1 &&
            taken.Get(1) == 2 &&
            taken.Get(2) == 3 &&
            outOfRangeThrown;

        runTest(
            testNumber++,
            QStringLiteral("Take"),
            QStringLiteral(
                "[1,2,3,4,5].Take(3)"),
            QStringLiteral(
                "[1,2,3], длина=3; Get(3) -> исключение"),
            QStringLiteral(
                "[%1,%2,%3], длина=%4; исключение=%5")
                .arg(taken.Get(0))
                .arg(taken.Get(1))
                .arg(taken.Get(2))
                .arg(toQtSize(length))
                .arg(outOfRangeThrown ?
                         QStringLiteral("да") :
                         QStringLiteral("нет")),
            passed);
    }

    

    {
        ArraySequence<int> data;
        data.Append(1);
        data.Append(2);
        data.Append(3);

        LazySequence<int> sequence(data);
        auto taken = sequence.Take(100);

        const size_t length =
            taken.GetCardinality().GetValue();

        bool outOfRangeThrown = false;

        try {
            (void)taken.Get(3);
        }
        catch (const IndexOutOFBoundsException&) {
            outOfRangeThrown = true;
        }

        const bool passed =
            length == 3 &&
            taken.Get(0) == 1 &&
            taken.Get(1) == 2 &&
            taken.Get(2) == 3 &&
            outOfRangeThrown;

        runTest(
            testNumber++,
            QStringLiteral(
                "Take(N), где N больше длины"),
            QStringLiteral(
                "[1,2,3].Take(100)"),
            QStringLiteral(
                "результат [1,2,3], длина=3"),
            QStringLiteral(
                "длина=%1; последний=%2; "
                "Get(3) исключение=%3")
                .arg(toQtSize(length))
                .arg(taken.Get(2))
                .arg(outOfRangeThrown ?
                         QStringLiteral("да") :
                         QStringLiteral("нет")),
            passed);
    }

    

    {
        ArraySequence<int> data;

        for (int i = 1; i <= 5; ++i)
            data.Append(i);

        LazySequence<int> sequence(data);

        const int sum =
            sequence.Reduce(
                [](const int& a, const int& b) {
                    return a + b;
                },
                0);

        const bool passed = sum == 15;

        runTest(
            testNumber++,
            QStringLiteral("Reduce"),
            QStringLiteral("[1,2,3,4,5]"),
            QStringLiteral("сумма=15"),
            QStringLiteral("сумма=%1").arg(sum),
            passed);
    }

    {
        auto fibRule =
            [](size_t index,
               const ArraySequence<std::string>& previous) -> std::string {
                if (index == 0)
                    return "0";

                if (index == 1)
                    return "1";

                return addDecimalStrings(
                    previous.Get(static_cast<int>(index - 1)),
                    previous.Get(static_cast<int>(index - 2)));
            };

        ArraySequence<std::string> seed;
        seed.Append("0");
        seed.Append("1");

        LazySequence<std::string> fibonacci(
            fibRule,
            seed);

        const bool isEndless =
            fibonacci.IsEndless();

        const bool cardinalityCorrect =
            fibonacci.GetCardinality().GetType() ==
            Cardinality::kCountableInfinity;

        const std::string f0 = fibonacci.Get(0);
        const std::string f1 = fibonacci.Get(1);
        const std::string f2 = fibonacci.Get(2);
        const std::string f3 = fibonacci.Get(3);
        const std::string f4 = fibonacci.Get(4);
        const std::string f100 = fibonacci.Get(100);

        const bool passed =
            isEndless &&
            cardinalityCorrect &&
            f0 == "0" &&
            f1 == "1" &&
            f2 == "1" &&
            f3 == "2" &&
            f4 == "3" &&
            f100 == "354224848179261915075" &&
            fibonacci.ComputedCount() == 101;

        runTest(
            testNumber++,
            QStringLiteral(
                "Бесконечная последовательность Fibonacci без переполнения"),
            QStringLiteral(
                "F(0)=0, F(1)=1, F(n)=F(n-1)+F(n-2)"),
            QStringLiteral(
                "F(100)=354224848179261915075; кардинал=ℵ₀"),
            QStringLiteral(
                "F(0..4)=%1,%2,%3,%4,%5; F(100)=%6; "
                "endless=%7; вычислено=%8")
                .arg(QString::fromStdString(f0))
                .arg(QString::fromStdString(f1))
                .arg(QString::fromStdString(f2))
                .arg(QString::fromStdString(f3))
                .arg(QString::fromStdString(f4))
                .arg(QString::fromStdString(f100))
                .arg(isEndless ?
                         QStringLiteral("да") :
                         QStringLiteral("нет"))
                .arg(toQtSize(
                    fibonacci.ComputedCount())),
            passed);
    }

    

    {
        ArraySequence<int> data;
        data.Append(10);
        data.Append(20);

        LazySequence<int> sequence(data);

        bool thrown = false;

        try {
            (void)sequence.Get(-1);
        }
        catch (const IndexOutOFBoundsException&) {
            thrown = true;
        }

        runTest(
            testNumber++,
            QStringLiteral(
                "Get с отрицательным индексом"),
            QStringLiteral("Get(-1)"),
            QStringLiteral(
                "IndexOutOFBoundsException"),
            thrown ?
                QStringLiteral(
                    "IndexOutOFBoundsException") :
                QStringLiteral(
                    "исключение не выброшено"),
            thrown);
    }

    

    {
        ArraySequence<int> data;
        data.Append(1);
        data.Append(2);
        data.Append(3);

        LazySequence<int> sequence(data);

        bool thrown = false;

        try {
            auto invalid =
                sequence.InsertAt(10, 99);
            (void)invalid;
        }
        catch (const IndexOutOFBoundsException&) {
            thrown = true;
        }

        runTest(
            testNumber++,
            QStringLiteral(
                "InsertAt с некорректным индексом"),
            QStringLiteral(
                "[1,2,3].InsertAt(10,99)"),
            QStringLiteral(
                "IndexOutOFBoundsException"),
            thrown ?
                QStringLiteral(
                    "IndexOutOFBoundsException") :
                QStringLiteral(
                    "исключение не выброшено"),
            thrown);
    }

    

    {
        LazySequence<int> empty;

        const size_t length =
            empty.GetCardinality().GetValue();

        bool thrown = false;

        try {
            (void)empty.GetFirst();
        }
        catch (const IndexOutOFBoundsException&) {
            thrown = true;
        }

        const bool passed =
            !empty.IsEndless() &&
            length == 0 &&
            empty.ComputedCount() == 0 &&
            thrown;

        runTest(
            testNumber++,
            QStringLiteral("Пустая последовательность"),
            QStringLiteral("LazySequence<int>()"),
            QStringLiteral(
                "длина=0; GetFirst -> исключение"),
            QStringLiteral(
                "длина=%1; вычислено=%2; "
                "GetFirst исключение=%3")
                .arg(toQtSize(length))
                .arg(toQtSize(empty.ComputedCount()))
                .arg(thrown ?
                         QStringLiteral("да") :
                         QStringLiteral("нет")),
            passed);
    }

    

    {
        ArraySequence<int> data;
        data.Append(1);
        data.Append(3);
        data.Append(5);

        LazySequence<int> sequence(data);

        auto filtered =
            sequence.Where(
                [](const int& value) {
                    return value % 2 == 0;
                });

        const size_t length =
            filtered.GetCardinality().GetValue();

        bool thrown = false;

        try {
            (void)filtered.Get(0);
        }
        catch (const IndexOutOFBoundsException&) {
            thrown = true;
        }

        const bool passed =
            length == 0 &&
            filtered.ComputedCount() == 0 &&
            thrown;

        runTest(
            testNumber++,
            QStringLiteral(
                "Where без подходящих элементов"),
            QStringLiteral(
                "[1,3,5], оставить чётные"),
            QStringLiteral(
                "пустая последовательность; длина=0"),
            QStringLiteral(
                "длина=%1; вычислено=%2; "
                "Get(0) исключение=%3")
                .arg(toQtSize(length))
                .arg(toQtSize(filtered.ComputedCount()))
                .arg(thrown ?
                         QStringLiteral("да") :
                         QStringLiteral("нет")),
            passed);
    }

    

    const int total = passedCount + failedCount;

    output->append(QString());
    output->append(
        QStringLiteral(
            "========================================"));

    output->append(
        QStringLiteral(
            "ИТОГО: %1/%2 тестов пройдено")
            .arg(passedCount)
            .arg(total));

    output->append(
        QStringLiteral(
            "Pass: %1 | Failed: %2")
            .arg(passedCount)
            .arg(failedCount));

    output->append(
        QStringLiteral(
            "========================================"));

    testDialog->exec();
    delete testDialog;
}

#endif 
