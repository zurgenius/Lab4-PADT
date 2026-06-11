#pragma once

#include "lazy/lazy_sequence.h"
#include "streams/base_streams.h"

template <class T> class LazyReadOnlyStream : public ReadOnlyStream<T> {
  private:
    const LazySequence<T> *source;
    int position;
    bool opened;

  public:
    explicit LazyReadOnlyStream(const LazySequence<T> *source);

    bool is_end_of_stream() const override;
    T read() override;
    int get_position() const override;
    bool can_seek() const override;
    int seek(int index) override;
    bool can_go_back() const override;
    void open() override;
    void close() override;
};

#include "streams/detail/lazy_streams.tpp"
