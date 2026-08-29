#include "TaskSyncWorker.h"

#include "StreamSynchronizer.h"

#include <QString>

#include <exception>
#include <stdexcept>

TaskSyncWorker::TaskSyncWorker(QObject* parent)
    : QObject(parent), tau_(0.0) {}

void TaskSyncWorker::setStreams(
    const std::vector<TimeLazySequence<double>>& streams,
    double tau) {

    streams_ = streams;
    tau_ = tau;
}

void TaskSyncWorker::run() {
    try {
        if (streams_.empty())
            throw std::invalid_argument(
                "TaskSyncWorker: список входных потоков пуст");

        for (const auto& stream : streams_) {
            if (stream.IsEndless())
                throw std::invalid_argument(
                    "TaskSyncWorker: бесконечные входные потоки нельзя полностью "
                    "вывести в таблицу. Ограничьте их с помощью Take(N)");
        }

        auto resultSeq =
            StreamSynchronizer<double>::Synchronize(streams_, tau_);

        ArraySequence<ArraySequence<std::optional<double>>> events;
        size_t index = 0;

        while (true) {
            try {
                events.Append(resultSeq.Get(index));
                ++index;
            } catch (const IndexOutOFBoundsException&) {
                break;
            }
        }

        emit progress(100);
        emit finished(events);
    } catch (const std::exception& e) {
        emit error(QString::fromUtf8(e.what()));
    } catch (...) {
        emit error(QString::fromUtf8("Неизвестная ошибка синхронизации"));
    }
}
