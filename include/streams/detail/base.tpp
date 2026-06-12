#pragma once

#include "streams/base.h"

// Безопасно читает элемент потока или возвращает None на конце.
template <class T> Option<T> ReadOnlyStream<T>::try_read() {
    if (is_end_of_stream()) {
        return Option<T>::None();
    }
    return Option<T>::Some(read());
}

// Позволяет удалять read-only stream через базовый указатель.
template <class T> ReadOnlyStream<T>::~ReadOnlyStream() {}

// Позволяет удалять write-only stream через базовый указатель.
template <class T> WriteOnlyStream<T>::~WriteOnlyStream() {}
