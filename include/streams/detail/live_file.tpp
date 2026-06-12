#pragma once

#include "streams/live_file.h"

#include <chrono>
#include <stdexcept>
#include <thread>

// Создает live-поток чтения файла с настройками ожидания новых строк.
inline LiveFileLineReadOnlyStream::LiveFileLineReadOnlyStream(const std::string &filename,
                                                              int max_empty_polls,
                                                              int wait_ms)
    : filename(filename), position(0), opened(false), ended(false), empty_polls(0),
      max_empty_polls(max_empty_polls), wait_ms(wait_ms) {
    if (max_empty_polls < 0 || wait_ms < 0) {
        throw std::invalid_argument("Live stream polling settings cannot be negative");
    }
}

// Проверяет, закрыт ли live-поток или исчерпан лимит ожидания.
inline bool LiveFileLineReadOnlyStream::is_end_of_stream() const { return !opened || ended; }

// Читает строку из live-файла или бросает ошибку при временном отсутствии строки.
inline std::string LiveFileLineReadOnlyStream::read() {
    Option<std::string> value = try_read();
    if (!value.has_value()) {
        throw std::out_of_range("Live stream has no line now");
    }
    return value.get_value();
}

// Пытается прочитать новую строку, ожидая дописывание файла по правилу polling.
inline Option<std::string> LiveFileLineReadOnlyStream::try_read() {
    if (!opened || ended) {
        return Option<std::string>::None();
    }
    std::string line;
    if (std::getline(file, line)) {
        empty_polls = 0;
        position++;
        return Option<std::string>::Some(line);
    }
    file.clear();
    empty_polls++;
    if (empty_polls > max_empty_polls) {
        ended = true;
    } else if (wait_ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
    }
    return Option<std::string>::None();
}

// Открывает live-файл для чтения с текущего начала.
inline void LiveFileLineReadOnlyStream::open() {
    close();
    file.open(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open live file for reading");
    }
    position = 0;
    opened = true;
    ended = false;
    empty_polls = 0;
}

// Закрывает live-файл.
inline void LiveFileLineReadOnlyStream::close() {
    if (file.is_open()) {
        file.close();
    }
    opened = false;
}
