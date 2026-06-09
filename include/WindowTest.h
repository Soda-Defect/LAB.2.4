#ifndef WINDOWTEST_H
#define WINDOWTEST_H

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QTextEdit>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>

class TestResultsDialog : public QDialog {
public:
    TestResultsDialog(const QString& results, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Результаты тестирования");
        setMinimumSize(600, 400);
        
        QVBoxLayout* layout = new QVBoxLayout(this);
        
        QTextEdit* textEdit = new QTextEdit(this);
        textEdit->setReadOnly(true);
        textEdit->setPlainText(results);
        
        QFont font("Courier New", 10);
        textEdit->setFont(font);
        
        QPushButton* closeButton = new QPushButton("Закрыть", this);
        connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
        
        layout->addWidget(textEdit);
        layout->addWidget(closeButton);
    }
};

#endif