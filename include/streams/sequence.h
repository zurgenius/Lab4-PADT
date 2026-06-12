#pragma once

#include "../sequence.h"
#include "streams/base.h"

// Превращает конечную Sequence в поток последовательного чтения.
template <class T> class SequenceReadOnlyStream : public ReadOnlyStream<T> {
  private:
    const Sequence<T> *source;
    int position;
    bool opened;

  public:
    SequenceReadOnlyStream(const Sequence<T> *source);

    bool is_end_of_stream() const override;
    T read() override;
    void open() override;
    void close() override;
};

// Превращает Sequence в приемник последовательной записи через append.
template <class T> class SequenceWriteOnlyStream : public WriteOnlyStream<T> {
  private:
    Sequence<T> *target;
    int position;
    bool opened;

  public:
    SequenceWriteOnlyStream(Sequence<T> *target);

    int write(const T &item) override;
    void open() override;
    void close() override;
};

#include "streams/detail/sequence.tpp"
