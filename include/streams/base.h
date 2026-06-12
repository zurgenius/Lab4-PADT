#pragma once

#include "option.h"

// Базовый интерфейс последовательного чтения значений из источника.
template <class T> class ReadOnlyStream {
  public:
    virtual bool is_end_of_stream() const = 0;
    virtual T read() = 0;
    virtual Option<T> try_read();
    virtual void open() = 0;
    virtual void close() = 0;
    virtual ~ReadOnlyStream();
};

// Базовый интерфейс последовательной записи значений в приемник.
template <class T> class WriteOnlyStream {
  public:
    virtual int write(const T &item) = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual ~WriteOnlyStream();
};

#include "streams/detail/base.tpp"
