#pragma once

#include "tasks/event_statistics.h"

#include <sstream>
#include <stdexcept>

template <class T> Event<T>::Event() : type(EventType::Unknown), value(T()), message("") {}

template <class T>
Event<T>::Event(EventType type, const T &value, const std::string &message)
    : type(type), value(value), message(message) {}

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

template <class T>
EventReadOnlyStream<T>::EventReadOnlyStream(ReadOnlyStream<std::string> *source)
    : source(source) {
    if (source == nullptr) {
        throw std::invalid_argument("Event stream source is nullptr");
    }
}

template <class T> bool EventReadOnlyStream<T>::is_end_of_stream() const {
    return source->is_end_of_stream();
}

template <class T> Event<T> EventReadOnlyStream<T>::read() {
    return EventParser<T>::parse_line(source->read());
}

template <class T> Option<Event<T>> EventReadOnlyStream<T>::try_read() {
    Option<std::string> line = source->try_read();
    if (!line.has_value()) {
        return Option<Event<T>>::None();
    }
    return Option<Event<T>>::Some(EventParser<T>::parse_line(line.get_value()));
}

template <class T> void EventReadOnlyStream<T>::open() { source->open(); }

template <class T> void EventReadOnlyStream<T>::close() { source->close(); }

template <class T>
OnlineEventStatistics<T>::OnlineEventStatistics()
    : total_events(0), start_events(0), end_events(0), measure_events(0),
      error_events(0), unknown_events(0), measure_sum(T()), measure_square_sum(T()),
      min_measure(T()), max_measure(T()), has_measurements(false) {}

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

template <class T> int OnlineEventStatistics<T>::get_total_events() const {
    return total_events;
}

template <class T> int OnlineEventStatistics<T>::get_start_events() const {
    return start_events;
}

template <class T> int OnlineEventStatistics<T>::get_end_events() const { return end_events; }

template <class T> int OnlineEventStatistics<T>::get_measure_events() const {
    return measure_events;
}

template <class T> int OnlineEventStatistics<T>::get_error_events() const {
    return error_events;
}

template <class T> int OnlineEventStatistics<T>::get_unknown_events() const {
    return unknown_events;
}

template <class T> bool OnlineEventStatistics<T>::has_any_measurements() const {
    return has_measurements;
}

template <class T> T OnlineEventStatistics<T>::get_min_measure() const {
    require_measurement();
    return min_measure;
}

template <class T> T OnlineEventStatistics<T>::get_max_measure() const {
    require_measurement();
    return max_measure;
}

template <class T> T OnlineEventStatistics<T>::get_sum_measure() const { return measure_sum; }

template <class T> T OnlineEventStatistics<T>::get_average_measure() const {
    require_measurement();
    return measure_sum / static_cast<T>(measure_events);
}

template <class T> T OnlineEventStatistics<T>::get_variance_measure() const {
    require_measurement();
    T average = get_average_measure();
    return measure_square_sum / static_cast<T>(measure_events) - average * average;
}

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

template <class T> void OnlineEventStatistics<T>::require_measurement() const {
    if (!has_measurements) {
        throw std::logic_error("No measurements");
    }
}

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

template <class T>
OnlineEventStatistics<T> ProtocolStatisticsTask<T>::process(ReadOnlyStream<std::string> &stream) {
    EventReadOnlyStream<T> events(&stream);
    return process(events);
}

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

template <class T>
OnlineEventStatistics<T> EventBatchProcessingTask<T>::process(ReadOnlyStream<std::string> &stream,
                                                              int batch_size) {
    EventReadOnlyStream<T> events(&stream);
    return process(events, batch_size);
}
