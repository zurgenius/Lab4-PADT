#pragma once

#include "option.h"

template <class T> class ReadOnlyStream {
  public:
    virtual bool is_end_of_stream() const = 0;
    virtual T read() = 0;
    virtual Option<T> try_read();
    virtual int get_position() const = 0;
    virtual bool can_seek() const = 0;
    virtual int seek(int index) = 0;
    virtual bool can_go_back() const = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual ~ReadOnlyStream();
};

template <class T> class WriteOnlyStream {
  public:
    virtual int write(const T &item) = 0;
    virtual int get_position() const = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual ~WriteOnlyStream();
};

#include "streams/detail/base_streams.tpp"
