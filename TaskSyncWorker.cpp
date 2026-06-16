#include "TaskSyncWorker.h"
#include "StreamSynchronizer.h"
#include <QDebug>

TaskSyncWorker::TaskSyncWorker(QObject *parent) : QObject(parent) {}

void TaskSyncWorker::setStreams(const std::vector<TimeLazySequence<double>>& streams, double tau) {
    streams_ = streams;
    tau_ = tau;
}

void TaskSyncWorker::run() {
    try {
        auto resultSeq = StreamSynchronizer<double>::Synchronize(streams_, tau_);
        ArraySequence<ArraySequence<std::optional<double>>> events;
        size_t idx = 0;
        while (true) {
            try {
                events.Append(resultSeq.Get(idx++));
            } catch (const IndexOutOFBoundsException&) {
                break;
            }
        }
        emit finished(events);
    } catch (const std::exception& e) {
        emit error(e.what());
    }
}

