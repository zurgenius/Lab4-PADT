#pragma once

#include "streams/file.h"

#include <stdexcept>

// Создает поток построчного чтения из файла.
inline FileLineReadOnlyStream::FileLineReadOnlyStream(const std::string &filename)
    : filename(filename), position(0), opened(false), ended(false) {}

// Проверяет, закрыт ли файл или достигнут конец.
inline bool FileLineReadOnlyStream::is_end_of_stream() const { return !opened || ended; }

// Читает следующую строку файла или бросает ошибку.
inline std::string FileLineReadOnlyStream::read() {
    if (!opened) {
        throw std::logic_error("File stream is closed");
    }
    std::string line;
    if (!std::getline(file, line)) {
        ended = true;
        throw std::out_of_range("File stream is ended");
    }
    position++;
    return line;
}

// Безопасно читает следующую строку файла.
inline Option<std::string> FileLineReadOnlyStream::try_read() {
    if (!opened || ended) {
        return Option<std::string>::None();
    }
    std::string line;
    if (!std::getline(file, line)) {
        ended = true;
        return Option<std::string>::None();
    }
    position++;
    return Option<std::string>::Some(line);
}

// Открывает файл для чтения с начала.
inline void FileLineReadOnlyStream::open() {
    close();
    file.open(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for reading");
    }
    position = 0;
    opened = true;
    ended = false;
}

// Закрывает файл чтения.
inline void FileLineReadOnlyStream::close() {
    if (file.is_open()) {
        file.close();
    }
    opened = false;
}

// Создает поток построчной записи в файл.
inline FileLineWriteOnlyStream::FileLineWriteOnlyStream(const std::string &filename)
    : filename(filename), position(0), opened(false) {}

// Записывает строку в файл и сразу сбрасывает буфер.
inline int FileLineWriteOnlyStream::write(const std::string &item) {
    if (!opened) {
        throw std::logic_error("File write stream is closed");
    }
    file << item << '\n';
    file.flush();
    position++;
    return position;
}

// Открывает файл для записи с перезаписью содержимого.
inline void FileLineWriteOnlyStream::open() {
    close();
    file.open(filename.c_str(), std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing");
    }
    position = 0;
    opened = true;
}

// Закрывает файл записи.
inline void FileLineWriteOnlyStream::close() {
    if (file.is_open()) {
        file.close();
    }
    opened = false;
}
