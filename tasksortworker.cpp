#include "tasksortworker.h"
#include "HeapSorter.h"
#include "Streams.h"
#include "LazySequence.h"
#include <QFile>
#include <QTextStream>


TaskSortWorker::TaskSortWorker(QObject *parent) : QObject(parent) {}

void TaskSortWorker::setData(const ArraySequence<int>& data) {
    data_ = data;
    useFile_ = false;
}

void TaskSortWorker::setFileMode(bool useFile, const QString& filePath) {
    useFile_ = useFile;
    filePath_ = filePath;
}

void TaskSortWorker::run() {
    try {
        if (useFile_) {
            QFile file(filePath_);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                emit error("Cannot open file");
                return;
            }
            QTextStream in(&file);
            ArraySequence<int> fileData;
            while (!in.atEnd()) {
                QString line = in.readLine();
                bool ok;
                int val = line.toInt(&ok);
                if (ok) fileData.Append(val);
            }
            data_ = fileData;
        }

        auto seq = std::make_shared<LazySequence<int>>(data_);
        SequenceReadStream<int> stream(seq);

        ArraySequence<int> result = HeapSorter<int>::Sort(stream);

        emit finished(result);
    } catch (const std::exception& e) {
        emit error(e.what());
    }
}
