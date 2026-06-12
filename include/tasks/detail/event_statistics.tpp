#pragma once

#include "tasks/event_statistics.h"

#include <sstream>
#include <stdexcept>

// Создает событие Unknown без значения и сообщения.
template <class T> Event<T>::Event() : type(EventType::Unknown), value(T()), message("") {}

// Создает событие заданного типа со значением и сообщением.
template <class T>
Event<T>::Event(EventType type, const T &value, const std::string &message)
    : type(type), value(value), message(message) {}

// Разбирает строку протокола в событие.
template <class T> Event<T> EventParser<T>::parse_line(const std::string &line) {
    std::istringstream input(line);
    std::string tag;
    input >> tag;

    if (tag == "START") {
        return Event<T>(EventType::Start, T(), tail_after_tag(line, tag));
    }
    if (tag == "END") {
        return Event<T>(EventType::End, T(), tail_after_tag(line, tag));
    }
    if (tag == "ERROR") {
        return Event<T>(EventType::Error, T(), tail_after_tag(line, tag));
    }
    if (tag == "MEASURE") {
        T value = T();
        input >> value;
        if (input.fail()) {
            return Event<T>(EventType::Unknown, T(), line);
        }
        return Event<T>(EventType::Measure, value, tail_after_tag(line, tag));
    }

    return Event<T>(EventType::Unknown, T(), line);
}

// Возвращает текст строки после ключевого слова события.
template <class T>
std::string EventParser<T>::tail_after_tag(const std::string &line, const std::string &tag) {
    if (line.size() <= tag.size()) {
        return "";
    }
    int start = static_cast<int>(tag.size());
    while (start < static_cast<int>(line.size()) && line[start] == ' ') {
        start++;
    }
    return line.substr(static_cast<std::string::size_type>(start));
}

// Создает поток событий поверх потока строк.
template <class T>
EventReadOnlyStream<T>::EventReadOnlyStream(ReadOnlyStream<std::string> *source)
    : source(source) {
    if (source == nullptr) {
        throw std::invalid_argument("Event stream source is nullptr");
    }
}

// Проверяет конец исходного строкового потока.
template <class T> bool EventReadOnlyStream<T>::is_end_of_stream() const {
    return source->is_end_of_stream();
}

// Читает строку и сразу парсит ее в событие.
template <class T> Event<T> EventReadOnlyStream<T>::read() {
    return EventParser<T>::parse_line(source->read());
}

// Безопасно читает и парсит следующее событие.
template <class T> Option<Event<T>> EventReadOnlyStream<T>::try_read() {
    Option<std::string> line = source->try_read();
    if (!line.has_value()) {
        return Option<Event<T>>::None();
    }
    return Option<Event<T>>::Some(EventParser<T>::parse_line(line.get_value()));
}

// Открывает исходный строковый поток.
template <class T> void EventReadOnlyStream<T>::open() { source->open(); }

// Закрывает исходный строковый поток.
template <class T> void EventReadOnlyStream<T>::close() { source->close(); }

// Создает пустой аккумулятор онлайн-статистики событий.
template <class T>
OnlineEventStatistics<T>::OnlineEventStatistics()
    : total_events(0), start_events(0), end_events(0), measure_events(0),
      error_events(0), unknown_events(0), measure_sum(T()), measure_square_sum(T()),
      min_measure(T()), max_measure(T()), has_measurements(false) {}

// Добавляет событие в статистику и обновляет нужные счетчики.
template <class T> void OnlineEventStatistics<T>::add_event(const Event<T> &event) {
    total_events++;
    switch (event.type) {
    case EventType::Start:
        start_events++;
        break;
    case EventType::End:
        end_events++;
        break;
    case EventType::Measure:
        measure_events++;
        add_measure(event.value);
        break;
    case EventType::Error:
        error_events++;
        break;
    case EventType::Unknown:
        unknown_events++;
        break;
    }
}

// Возвращает общее количество обработанных событий.
template <class T> int OnlineEventStatistics<T>::get_total_events() const {
    return total_events;
}

// Возвращает количество START-событий.
template <class T> int OnlineEventStatistics<T>::get_start_events() const {
    return start_events;
}

// Возвращает количество END-событий.
template <class T> int OnlineEventStatistics<T>::get_end_events() const { return end_events; }

// Возвращает количество MEASURE-событий.
template <class T> int OnlineEventStatistics<T>::get_measure_events() const {
    return measure_events;
}

// Возвращает количество ERROR-событий.
template <class T> int OnlineEventStatistics<T>::get_error_events() const {
    return error_events;
}

// Возвращает количество нераспознанных событий.
template <class T> int OnlineEventStatistics<T>::get_unknown_events() const {
    return unknown_events;
}

// Проверяет, были ли обработаны измерения.
template <class T> bool OnlineEventStatistics<T>::has_any_measurements() const {
    return has_measurements;
}

// Возвращает минимальное измерение.
template <class T> T OnlineEventStatistics<T>::get_min_measure() const {
    require_measurement();
    return min_measure;
}

// Возвращает максимальное измерение.
template <class T> T OnlineEventStatistics<T>::get_max_measure() const {
    require_measurement();
    return max_measure;
}

// Возвращает сумму всех измерений.
template <class T> T OnlineEventStatistics<T>::get_sum_measure() const { return measure_sum; }

// Возвращает среднее значение измерений.
template <class T> T OnlineEventStatistics<T>::get_average_measure() const {
    require_measurement();
    return measure_sum / static_cast<T>(measure_events);
}

// Возвращает дисперсию измерений по накопленным суммам.
template <class T> T OnlineEventStatistics<T>::get_variance_measure() const {
    require_measurement();
    T average = get_average_measure();
    return measure_square_sum / static_cast<T>(measure_events) - average * average;
}

// Добавляет числовое измерение в min/max/sum/square_sum.
template <class T> void OnlineEventStatistics<T>::add_measure(const T &value) {
    if (!has_measurements) {
        min_measure = value;
        max_measure = value;
        has_measurements = true;
    } else {
        if (value < min_measure) {
            min_measure = value;
        }
        if (value > max_measure) {
            max_measure = value;
        }
    }
    measure_sum = measure_sum + value;
    measure_square_sum = measure_square_sum + value * value;
}

// Проверяет, что статистика измерений не пустая.
template <class T> void OnlineEventStatistics<T>::require_measurement() const {
    if (!has_measurements) {
        throw std::logic_error("No measurements");
    }
}

// Обрабатывает поток событий за один проход.
template <class T>
OnlineEventStatistics<T> ProtocolStatisticsTask<T>::process(ReadOnlyStream<Event<T>> &stream) {
    OnlineEventStatistics<T> statistics;
    stream.open();
    while (!stream.is_end_of_stream()) {
        Option<Event<T>> event = stream.try_read();
        if (event.has_value()) {
            statistics.add_event(event.get_value());
        }
    }
    stream.close();
    return statistics;
}

// Оборачивает поток строк в поток событий и обрабатывает его.
template <class T>
OnlineEventStatistics<T> ProtocolStatisticsTask<T>::process(ReadOnlyStream<std::string> &stream) {
    EventReadOnlyStream<T> events(&stream);
    return process(events);
}

// Обрабатывает поток событий логическими батчами заданного размера.
template <class T>
OnlineEventStatistics<T> EventBatchProcessingTask<T>::process(ReadOnlyStream<Event<T>> &stream,
                                                              int batch_size) {
    if (batch_size <= 0) {
        throw std::invalid_argument("Batch size must be positive");
    }
    OnlineEventStatistics<T> statistics;
    int current_batch_count = 0;
    stream.open();
    while (!stream.is_end_of_stream()) {
        Option<Event<T>> event = stream.try_read();
        if (event.has_value()) {
            statistics.add_event(event.get_value());
            current_batch_count++;
            if (current_batch_count == batch_size) {
                current_batch_count = 0;
            }
        }
    }
    stream.close();
    return statistics;
}

// Оборачивает поток строк в события и обрабатывает его батчами.
template <class T>
OnlineEventStatistics<T> EventBatchProcessingTask<T>::process(ReadOnlyStream<std::string> &stream,
                                                              int batch_size) {
    EventReadOnlyStream<T> events(&stream);
    return process(events, batch_size);
}
