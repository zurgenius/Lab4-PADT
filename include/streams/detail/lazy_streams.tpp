#pragma once

#include "streams/lazy_streams.h"

#include <stdexcept>

template <class T>
LazyReadOnlyStream<T>::LazyReadOnlyStream(const LazySequence<T> *source)
    : source(source), position(0), opened(false) {
    if (source == nullptr) {
        throw std::invalid_argument("Lazy stream source is nullptr");
    }
}

template <class T> bool LazyReadOnlyStream<T>::is_end_of_stream() const {
    return !opened ||
           (source->get_length().is_finite() &&
            position >= source->get_length().get_finite_count());
}

template <class T> T LazyReadOnlyStream<T>::read() {
    if (is_end_of_stream()) {
        throw std::out_of_range("Lazy stream is ended");
    }
    return source->get(position++);
}

template <class T> void LazyReadOnlyStream<T>::open() { opened = true; }

template <class T> void LazyReadOnlyStream<T>::close() { opened = false; }
