#pragma once
#include "IStream.h"
#include "ArraySequence.h"
#include "LazySequence.h"
#include <fstream>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <sstream>


template<typename T>
class SequenceReadStream : public IReadableStream<T> {
public:
    explicit SequenceReadStream(std::shared_ptr<LazySequence<T>> seq)
        : seq_(std::move(seq)), pos_(0), opened_(true) {}

    void Open() override { opened_ = true; }
    void Close() override { opened_ = false; }
    bool IsOpen() const override { return opened_; }

    bool IsEndOfStream() override {
        if (!opened_) return true;
        if (seq_->IsEndless()) return false;
        try {
            (void)seq_->Get(pos_);
            return false;
        } catch (const IndexOutOFBoundsException&) {
            return true;
        }
    }

    T Read() override {
        if (!opened_) throw StreamError("Stream is closed");
        try {
            T val = seq_->Get(pos_);
            ++pos_;
            return val;
        } catch (const IndexOutOFBoundsException&) {
            throw EndOfStream("No more elements in sequence");
        }
    }

    size_t GetPosition() const override { return pos_; }
    bool IsCanSeek() const override { return true; }
    size_t Seek(size_t index) override {
        if (!opened_) throw StreamError("Stream is closed");
        try {
            (void)seq_->Get(index);
            pos_ = index;
            return pos_;
        } catch (const IndexOutOFBoundsException&) {
            throw IndexOutOFBoundsException("Cannot seek to index " + std::to_string(index));
        }
    }
    bool IsCanGoBack() const override { return true; }

private:
    std::shared_ptr<LazySequence<T>> seq_;
    size_t pos_;
    bool opened_;
};


enum class FileMode { Text, Binary };


template<typename T>
class FileReadStream : public IReadableStream<T> {
public:

    FileReadStream(const std::string& path, std::function<T(const std::string&)> deserializer)
        : path_(path), deserializerText_(deserializer), mode_(FileMode::Text), isOpen_(false), pos_(0) {}


    FileReadStream(const std::string& path, std::function<T(const char*, size_t)> deserializer)
        : path_(path), deserializerBinary_(deserializer), mode_(FileMode::Binary), isOpen_(false), pos_(0) {}

    ~FileReadStream() { Close(); }

    void Open() override {
        if (isOpen_) return;
        if (mode_ == FileMode::Text)
            file_.open(path_, std::ios::in);
        else
            file_.open(path_, std::ios::in | std::ios::binary);
        if (!file_.is_open())
            throw StreamError("Cannot open file " + path_);
        isOpen_ = true;
        pos_ = 0;
    }

    void Close() override {
        if (isOpen_) {
            file_.close();
            isOpen_ = false;
        }
    }

    bool IsOpen() const override { return isOpen_; }

    bool IsEndOfStream() override {
        if (!isOpen_) return true;
        file_.peek();
        return file_.eof();
    }

    T Read() override {
        if (!isOpen_) throw StreamError("Stream is closed");
        if (IsEndOfStream()) throw EndOfStream("End of file");

        if (mode_ == FileMode::Text) {
            std::string line;
            if (!std::getline(file_, line))
                throw EndOfStream("Failed to read line");
            ++pos_;
            return deserializerText_(line);
        } else {

            uint32_t len;
            file_.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (!file_) throw EndOfStream("Failed to read binary length");
            std::vector<char> buffer(len);
            file_.read(buffer.data(), len);
            if (!file_) throw StreamError("Failed to read binary data");
            ++pos_;
            return deserializerBinary_(buffer.data(), len);
        }
    }

    size_t GetPosition() const override { return pos_; }
    bool IsCanSeek() const override { return true; }

    size_t Seek(size_t index) override {
        if (!isOpen_) throw StreamError("Stream is closed");
        if (index < pos_) {

            Close();
            Open();
            pos_ = 0;
            while (pos_ < index && !IsEndOfStream()) {
                (void)Read();
            }
        } else if (index > pos_) {
            while (pos_ < index && !IsEndOfStream()) {
                (void)Read();
            }
        }
        return pos_;
    }

    bool IsCanGoBack() const override { return true; }

private:
    std::string path_;
    std::function<T(const std::string&)> deserializerText_;
    std::function<T(const char*, size_t)> deserializerBinary_;
    FileMode mode_;
    std::ifstream file_;
    bool isOpen_;
    size_t pos_;
};


template<typename T>
class FileWriteStream : public IWritableStream<T> {
public:

    FileWriteStream(const std::string& path, std::function<std::string(const T&)> serializer, bool append = false)
        : path_(path), serializerText_(serializer), mode_(FileMode::Text), append_(append), isOpen_(false) {}


    FileWriteStream(const std::string& path, std::function<void(const T&, std::vector<char>&)> serializer, bool append = false)
        : path_(path), serializerBinary_(serializer), mode_(FileMode::Binary), append_(append), isOpen_(false) {}

    ~FileWriteStream() { Close(); }

    void Open() override {
        if (isOpen_) return;
        std::ios::openmode mode = std::ios::out;
        if (append_) mode |= std::ios::app;
        if (mode_ == FileMode::Binary) mode |= std::ios::binary;
        file_.open(path_, mode);
        if (!file_.is_open())
            throw StreamError("Cannot open file for writing: " + path_);
        isOpen_ = true;
    }

    void Close() override {
        if (isOpen_) {
            Flush();
            file_.close();
            isOpen_ = false;
        }
    }

    bool IsOpen() const override { return isOpen_; }
    bool IsEndOfStream()  override { return false; }

    void Write(const T& value) override {
        if (!isOpen_) throw StreamError("Stream is closed");
        if (mode_ == FileMode::Text) {
            file_ << serializerText_(value) << '\n';
        } else {
            std::vector<char> buffer;
            serializerBinary_(value, buffer);
            uint32_t len = static_cast<uint32_t>(buffer.size());
            file_.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file_.write(buffer.data(), len);
        }
        if (!file_) throw StreamError("Write failed");
    }

    void Flush() override {
        if (isOpen_) file_.flush();
    }

private:
    std::string path_;
    std::function<std::string(const T&)> serializerText_;
    std::function<void(const T&, std::vector<char>&)> serializerBinary_;
    FileMode mode_;
    bool append_;
    std::ofstream file_;
    bool isOpen_;
};


template<typename T>
class FileReadWriteStream : public IReadWriteStream<T> {
public:

    FileReadWriteStream(const std::string& path,
                        std::function<T(const std::string&)> deserializer,
                        std::function<std::string(const T&)> serializer)
        : path_(path), deserializerText_(deserializer), serializerText_(serializer),
        mode_(FileMode::Text), isOpen_(false), pos_(0) {}


    FileReadWriteStream(const std::string& path,
                        std::function<T(const char*, size_t)> deserializer,
                        std::function<void(const T&, std::vector<char>&)> serializer)
        : path_(path), deserializerBinary_(deserializer), serializerBinary_(serializer),
        mode_(FileMode::Binary), isOpen_(false), pos_(0) {}

    ~FileReadWriteStream() { Close(); }

    void Open() override {
        if (isOpen_) return;
        std::ios::openmode mode = std::ios::in | std::ios::out;
        if (mode_ == FileMode::Binary) mode |= std::ios::binary;
        file_.open(path_, mode);
        if (!file_.is_open()) {

            file_.open(path_, std::ios::in | std::ios::out | std::ios::trunc);
        }
        if (!file_.is_open())
            throw StreamError("Cannot open file for read/write: " + path_);
        isOpen_ = true;
        pos_ = 0;
    }

    void Close() override {
        if (isOpen_) {
            Flush();
            file_.close();
            isOpen_ = false;
        }
    }

    bool IsOpen() const override { return isOpen_; }

    bool IsEndOfStream()  override {
        if (!isOpen_) return true;
        file_.peek();
        return file_.eof();
    }

    T Read() override {
        if (!isOpen_) throw StreamError("Stream is closed");
        if (IsEndOfStream()) throw EndOfStream("End of file");

        if (mode_ == FileMode::Text) {
            std::string line;
            if (!std::getline(file_, line))
                throw EndOfStream("Failed to read line");
            ++pos_;
            return deserializerText_(line);
        } else {
            uint32_t len;
            file_.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (!file_) throw EndOfStream("Failed to read binary length");
            std::vector<char> buffer(len);
            file_.read(buffer.data(), len);
            if (!file_) throw StreamError("Failed to read binary data");
            ++pos_;
            return deserializerBinary_(buffer.data(), len);
        }
    }

    size_t GetPosition() const override { return pos_; }
    bool IsCanSeek() const override { return true; }

    size_t Seek(size_t index) override {
        if (!isOpen_) throw StreamError("Stream is closed");
        if (index < pos_) {
            Close();
            Open();
            pos_ = 0;
            while (pos_ < index && !IsEndOfStream()) {
                (void)Read();
            }
        } else if (index > pos_) {
            while (pos_ < index && !IsEndOfStream()) {
                (void)Read();
            }
        }
        return pos_;
    }

    bool IsCanGoBack() const override { return true; }

    void Write(const T& value) override {
        if (!isOpen_) throw StreamError("Stream is closed");
        if (mode_ == FileMode::Text) {
            file_ << serializerText_(value) << '\n';
        } else {
            std::vector<char> buffer;
            serializerBinary_(value, buffer);
            uint32_t len = static_cast<uint32_t>(buffer.size());
            file_.write(reinterpret_cast<const char*>(&len), sizeof(len));
            file_.write(buffer.data(), len);
        }
        if (!file_) throw StreamError("Write failed");
    }

    void Flush() override {
        if (isOpen_) file_.flush();
    }

private:
    std::string path_;
    std::function<T(const std::string&)> deserializerText_;
    std::function<std::string(const T&)> serializerText_;
    std::function<T(const char*, size_t)> deserializerBinary_;
    std::function<void(const T&, std::vector<char>&)> serializerBinary_;
    FileMode mode_;
    std::fstream file_;
    bool isOpen_;
    size_t pos_;
};


template<typename T>
class StringReadStream : public IReadableStream<T> {
public:
    StringReadStream(const std::string& data, std::function<T(const std::string&)> deserializer)
        : data_(data), deserializer_(deserializer), pos_(0), opened_(true) {}

    void Open() override { opened_ = true; }
    void Close() override { opened_ = false; }
    bool IsOpen() const override { return opened_; }

    bool IsEndOfStream()  override {
        if (!opened_) return true;
        auto lines = splitLines();
        return pos_ >= lines.GetLength();
    }

    T Read() override {
        if (!opened_) throw StreamError("Stream is closed");
        auto lines = splitLines();
        if (pos_ >= static_cast<size_t>(lines.GetLength()))
            throw EndOfStream("No more lines");
        T value = deserializer_(lines.Get(static_cast<int>(pos_)));
        ++pos_;
        return value;
    }

    size_t GetPosition() const override { return pos_; }
    bool IsCanSeek() const override { return true; }

    size_t Seek(size_t index) override {
        if (!opened_) throw StreamError("Stream is closed");
        auto lines = splitLines();
        if (index >= static_cast<size_t>(lines.GetLength()))
            throw IndexOutOFBoundsException("Seek out of range");
        pos_ = index;
        return pos_;
    }

    bool IsCanGoBack() const override { return true; }

private:
    std::string data_;
    std::function<T(const std::string&)> deserializer_;
    size_t pos_;
    bool opened_;

    ArraySequence<std::string> splitLines() const {
        ArraySequence<std::string> lines;
        std::stringstream ss(data_);
        std::string line;
        while (std::getline(ss, line)) {
            lines.Append(line);
        }
        return lines;
    }
};

template<typename T>
class StringWriteStream : public IWritableStream<T> {
public:
    explicit StringWriteStream(std::function<std::string(const T&)> serializer)
        : serializer_(serializer), opened_(true) {}

    void Open() override { opened_ = true;data_ = ArraySequence<std::string>(); }
    void Close() override { opened_ = false; }
    bool IsOpen() const override { return opened_; }
    bool IsEndOfStream() override { return false; }

    void Write(const T& value) override {
        if (!opened_) throw StreamError("Stream is closed");
        data_.Append(serializer_(value));
    }

    void Flush() override {}

    ArraySequence<std::string> GetData() const { return data_; }

private:
    std::function<std::string(const T&)> serializer_;
    ArraySequence<std::string> data_;
    bool opened_;
};
