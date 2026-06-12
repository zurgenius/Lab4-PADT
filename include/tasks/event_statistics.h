#pragma once

#include "streams/base.h"

#include <string>

enum class EventType {
    Start,
    End,
    Measure,
    Error,
    Unknown
};

template <class T> struct Event {
    EventType type;
    T value;
    std::string message;

    Event();
    Event(EventType type, const T &value, const std::string &message);
};

template <class T> class EventParser {
  public:
    static Event<T> parse_line(const std::string &line);

  private:
    static std::string tail_after_tag(const std::string &line, const std::string &tag);
};

template <class T> class EventReadOnlyStream : public ReadOnlyStream<Event<T>> {
  private:
    ReadOnlyStream<std::string> *source;

  public:
    EventReadOnlyStream(ReadOnlyStream<std::string> *source);

    bool is_end_of_stream() const override;
    Event<T> read() override;
    Option<Event<T>> try_read() override;
    void open() override;
    void close() override;
};

template <class T> class OnlineEventStatistics {
  private:
    int total_events;
    int start_events;
    int end_events;
    int measure_events;
    int error_events;
    int unknown_events;
    T measure_sum;
    T measure_square_sum;
    T min_measure;
    T max_measure;
    bool has_measurements;

    void add_measure(const T &value);
    void require_measurement() const;

  public:
    OnlineEventStatistics();

    void add_event(const Event<T> &event);

    int get_total_events() const;
    int get_start_events() const;
    int get_end_events() const;
    int get_measure_events() const;
    int get_error_events() const;
    int get_unknown_events() const;
    bool has_any_measurements() const;

    T get_min_measure() const;
    T get_max_measure() const;
    T get_sum_measure() const;
    T get_average_measure() const;
    T get_variance_measure() const;
};

template <class T> class ProtocolStatisticsTask {
  public:
    static OnlineEventStatistics<T> process(ReadOnlyStream<Event<T>> &stream);
    static OnlineEventStatistics<T> process(ReadOnlyStream<std::string> &stream);
};

template <class T> class EventBatchProcessingTask {
  public:
    static OnlineEventStatistics<T> process(ReadOnlyStream<Event<T>> &stream, int batch_size);
    static OnlineEventStatistics<T> process(ReadOnlyStream<std::string> &stream, int batch_size);
};

#include "tasks/detail/event_statistics.tpp"
