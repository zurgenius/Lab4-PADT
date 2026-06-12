#pragma once

template <class T> Option<T> Generator<T>::try_get_next() {
    if (!has_next()) {
        return Option<T>::None();
    }
    return Option<T>::Some(get_next());
}

template <class T> void Generator<T>::bind_source(const Sequence<T> *) {}

template <class T> Generator<T>::~Generator() {}
