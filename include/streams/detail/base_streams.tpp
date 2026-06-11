#pragma once

#include "streams/base_streams.h"

template <class T> Option<T> ReadOnlyStream<T>::try_read() {
    if (is_end_of_stream()) {
        return Option<T>::None();
    }
    return Option<T>::Some(read());
}

template <class T> ReadOnlyStream<T>::~ReadOnlyStream() {}

template <class T> WriteOnlyStream<T>::~WriteOnlyStream() {}
