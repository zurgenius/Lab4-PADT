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

template <class T> int LazyReadOnlyStream<T>::get_position() const { return position; }

template <class T> bool LazyReadOnlyStream<T>::can_seek() const { return true; }

template <class T> int LazyReadOnlyStream<T>::seek(int index) {
    if (index < 0) {
        throw std::out_of_range("Seek index is out of range");
    }
    if (source->get_length().is_finite() && index > source->get_length().get_finite_count()) {
        throw std::out_of_range("Seek index is out of range");
    }
    position = index;
    return position;
}

template <class T> bool LazyReadOnlyStream<T>::can_go_back() const { return true; }

template <class T> void LazyReadOnlyStream<T>::open() { opened = true; }

template <class T> void LazyReadOnlyStream<T>::close() { opened = false; }
