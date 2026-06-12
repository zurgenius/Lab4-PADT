#pragma once

#include "option.h"

template <class T> class ReadOnlyStream {
  public:
    virtual bool is_end_of_stream() const = 0;
    virtual T read() = 0;
    virtual Option<T> try_read();
    virtual void open() = 0;
    virtual void close() = 0;
    virtual ~ReadOnlyStream();
};

template <class T> class WriteOnlyStream {
  public:
    virtual int write(const T &item) = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual ~WriteOnlyStream();
};

#include "streams/detail/base_streams.tpp"
