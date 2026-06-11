#pragma once

#include "sequence.h"
#include "streams/base_streams.h"

template <class T> class SequenceReadOnlyStream : public ReadOnlyStream<T> {
  private:
    const Sequence<T> *source;
    int position;
    bool opened;

  public:
    explicit SequenceReadOnlyStream(const Sequence<T> *source);

    bool is_end_of_stream() const override;
    T read() override;
    int get_position() const override;
    bool can_seek() const override;
    int seek(int index) override;
    bool can_go_back() const override;
    void open() override;
    void close() override;
};

template <class T> class SequenceWriteOnlyStream : public WriteOnlyStream<T> {
  private:
    Sequence<T> *target;
    int position;
    bool opened;

  public:
    explicit SequenceWriteOnlyStream(Sequence<T> *target);

    int write(const T &item) override;
    int get_position() const override;
    void open() override;
    void close() override;
};

#include "streams/detail/sequence_streams.tpp"
