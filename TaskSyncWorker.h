#ifndef TASKSYNCWORKER_H
#define TASKSYNCWORKER_H

#include <QObject>
#include <vector>
#include <optional>
#include "LazySequence.h"
#include "TimeValue.h"
#include "ArraySequence.h"

class TaskSyncWorker : public QObject {
    Q_OBJECT
public:
    explicit TaskSyncWorker(QObject *parent = nullptr);
    void setStreams(const std::vector<TimeLazySequence<double>>& streams, double tau);

public slots:
    void run();

signals:
    void finished(const ArraySequence<ArraySequence<std::optional<double>>>& result);
    void progress(int percent);
    void error(const QString& msg);

private:
    std::vector<TimeLazySequence<double>> streams_;
    double tau_;
};

#endif // TASKSYNCWORKER_H
