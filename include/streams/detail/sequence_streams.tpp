#pragma once

#include "streams/sequence_streams.h"

#include <stdexcept>

template <class T>
SequenceReadOnlyStream<T>::SequenceReadOnlyStream(const Sequence<T> *source)
    : source(source), position(0), opened(false) {
    if (source == nullptr) {
        throw std::invalid_argument("Sequence stream source is nullptr");
    }
}

template <class T> bool SequenceReadOnlyStream<T>::is_end_of_stream() const {
    return !opened || position >= source->get_count();
}

template <class T> T SequenceReadOnlyStream<T>::read() {
    if (is_end_of_stream()) {
        throw std::out_of_range("Sequence stream is ended");
    }
    return source->get(position++);
}

template <class T> void SequenceReadOnlyStream<T>::open() { opened = true; }

template <class T> void SequenceReadOnlyStream<T>::close() { opened = false; }

template <class T>
SequenceWriteOnlyStream<T>::SequenceWriteOnlyStream(Sequence<T> *target)
    : target(target), position(0), opened(false) {
    if (target == nullptr) {
        throw std::invalid_argument("Sequence stream target is nullptr");
    }
}

template <class T> int SequenceWriteOnlyStream<T>::write(const T &item) {
    if (!opened) {
        throw std::logic_error("Sequence write stream is closed");
    }
    Sequence<T> *result = target->append(item);
    target = result;
    position++;
    return position;
}

template <class T> void SequenceWriteOnlyStream<T>::open() { opened = true; }

template <class T> void SequenceWriteOnlyStream<T>::close() { opened = false; }
