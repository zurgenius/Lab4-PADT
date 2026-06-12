#pragma once

#include <stdexcept>

template <class T>
MapGenerator<T>::MapGenerator(const LazySequence<T> &source, T (*func)(const T &item))
    : source(new LazySequence<T>(source)), func(func), position(0) {
    if (func == nullptr) {
        throw std::invalid_argument("Map function is nullptr");
    }
}

template <class T>
MapGenerator<T>::MapGenerator(const MapGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), func(other.func), position(other.position) {}

template <class T> MapGenerator<T>::~MapGenerator() { delete source; }

template <class T> bool MapGenerator<T>::has_next() const {
    return source->get_length().is_infinite() ||
           position < source->get_length().get_finite_count();
}

template <class T> T MapGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void MapGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *MapGenerator<T>::clone() const {
    return new MapGenerator<T>(*this);
}

template <class T> OrdinalLength MapGenerator<T>::get_length() const {
    return source->get_length();
}

template <class T> T MapGenerator<T>::get_at(const OrdinalIndex &index) const {
    return func(source->get(index));
}
