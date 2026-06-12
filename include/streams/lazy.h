#pragma once

#include "lazy/lazy_sequence.h"
#include "streams/base.h"

// Читает LazySequence как поток, материализуя элементы только по мере чтения.
template <class T> class LazyReadOnlyStream : public ReadOnlyStream<T> {
  private:
    const LazySequence<T> *source;
    int position;
    bool opened;

  public:
    explicit LazyReadOnlyStream(const LazySequence<T> *source);

    bool is_end_of_stream() const override;
    T read() override;
    void open() override;
    void close() override;
};

#include "streams/detail/lazy.tpp"
