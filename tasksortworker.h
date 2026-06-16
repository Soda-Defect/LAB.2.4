#ifndef TASKSORTWORKER_H
#define TASKSORTWORKER_H

#include <QObject>
#include <QString>
#include "ArraySequence.h"

class TaskSortWorker : public QObject {
    Q_OBJECT
public:
    explicit TaskSortWorker(QObject *parent = nullptr);
    void setData(const ArraySequence<int>& data);
    void setFileMode(bool useFile, const QString& filePath);

public slots:
    void run();


signals:
    void finished(const ArraySequence<int>& sorted);
    void progress(int percent);
    void error(const QString& msg);


private:
    ArraySequence<int> data_;
    bool useFile_ = false;
    QString filePath_;
};

#endif // TASKSORTWORKER_H
