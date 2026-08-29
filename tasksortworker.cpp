#include "tasksortworker.h"

#include "HeapSorter.h"
#include "LazySequence.h"
#include "Streams.h"

#include <QFile>
#include <QIODevice>
#include <QRegularExpression>
#include <QStringList>
#include <QTextStream>

#include <cstddef>
#include <exception>
#include <memory>
#include <stdexcept>

namespace {
class IntegerFileReadStream final : public IReadableStream<int> {
public:
    explicit IntegerFileReadStream(const QString& path)
        : path_(path) {}

    ~IntegerFileReadStream() override {
        Close();
    }

    void Open() override {
        if (opened_)
            return;

        if (path_.trimmed().isEmpty())
            throw StreamError("File path is empty");

        file_.setFileName(path_);

        if (!file_.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw StreamError(
                QString("Не удалось открыть файл: %1").arg(path_)
                    .toUtf8()
                    .constData());
        }

        text_ = std::make_unique<QTextStream>(&file_);

        tokens_.clear();
        tokenIndex_ = 0;
        position_ = 0;
        lineNumber_ = 0;
        opened_ = true;
    }

    void Close() override {
        if (!opened_)
            return;

        text_.reset();
        file_.close();

        tokens_.clear();
        tokenIndex_ = 0;
        opened_ = false;
    }

    bool IsOpen() const override {
        return opened_;
    }

    bool IsEndOfStream() override {
        if (!opened_)
            return true;

        return !ensureTokenAvailable();
    }

    int Read() override {
        if (!opened_)
            throw StreamError("Stream is closed");

        if (!ensureTokenAvailable())
            throw EndOfStream("End of file");

        const QString token = tokens_.at(tokenIndex_++);
        bool ok = false;
        const int value = token.toInt(&ok);

        if (!ok) {
            throw StreamError(
                QString("Некорректное целое число \"%1\" в строке %2")
                    .arg(token)
                    .arg(lineNumber_)
                    .toUtf8()
                    .constData());
        }

        ++position_;
        return value;
    }

    size_t GetPosition() const override {
        return position_;
    }

    bool IsCanSeek() const override {
        return true;
    }

    size_t Seek(size_t index) override {
        if (!opened_)
            throw StreamError("Stream is closed");

        if (index == position_)
            return position_;

        if (index < position_) {
            Close();
            Open();
        }

        while (position_ < index) {
            if (IsEndOfStream())
                break;

            (void)Read();
        }

        return position_;
    }

    bool IsCanGoBack() const override {
        return true;
    }

private:
    bool ensureTokenAvailable() {
        if (!opened_ || !text_)
            return false;

        while (tokenIndex_ >= tokens_.size()) {
            if (text_->atEnd()) {
                tokens_.clear();
                tokenIndex_ = 0;
                return false;
            }

            const QString line = text_->readLine();
            ++lineNumber_;

            tokens_ = line.split(
                QRegularExpression(QStringLiteral(R"([,;\s]+)")),
                Qt::SkipEmptyParts);

            tokenIndex_ = 0;

            if (!tokens_.isEmpty())
                return true;
        }

        return true;
    }

private:
    QString path_;
    QFile file_;
    std::unique_ptr<QTextStream> text_;

    QStringList tokens_;
    qsizetype tokenIndex_ = 0;

    size_t position_ = 0;
    qint64 lineNumber_ = 0;
    bool opened_ = false;
};

} 


TaskSortWorker::TaskSortWorker(QObject* parent)
    : QObject(parent) {}


void TaskSortWorker::setData(const ArraySequence<int>& data) {
    data_ = data;

    useFile_ = false;
    filePath_.clear();
}


void TaskSortWorker::setFileMode(bool useFile, const QString& filePath) {
    useFile_ = useFile;

    if (useFile_) {
        filePath_ = filePath;
    } else {
        filePath_.clear();
    }
}


void TaskSortWorker::run() {
    try {
        emit progress(0);

        ArraySequence<int> result;

        if (useFile_) {
            if (filePath_.trimmed().isEmpty())
                throw std::invalid_argument("Не указан файл для сортировки");
            IntegerFileReadStream stream(filePath_);
            result = HeapSorter<int>::Sort(stream);
        } else {
            auto sequence =
                std::make_shared<LazySequence<int>>(data_);

            SequenceReadStream<int> stream(sequence);
            result = HeapSorter<int>::Sort(stream);
        }

        emit progress(100);
        emit finished(result);
    }
    catch (const std::exception& e) {
        emit error(QString::fromUtf8(e.what()));
    }
    catch (...) {
        emit error(
            QString::fromUtf8("Неизвестная ошибка при сортировке потока"));
    }
}
