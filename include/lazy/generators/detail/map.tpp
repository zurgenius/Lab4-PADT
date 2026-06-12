#pragma once

#include "lazy/lazy_sequence.h"

#include <stdexcept>

// Создает генератор ленивого применения функции к источнику.
template <class T>
MapGenerator<T>::MapGenerator(const LazySequence<T> &source, T (*func)(const T &item))
    : source(new LazySequence<T>(source)), func(func), position(0) {
    if (func == nullptr) {
        throw std::invalid_argument("Map function is nullptr");
    }
}

// Копирует генератор map вместе с источником и функцией.
template <class T>
MapGenerator<T>::MapGenerator(const MapGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), func(other.func), position(other.position) {}

// Освобождает копию исходной последовательности.
template <class T> MapGenerator<T>::~MapGenerator() { delete source; }

// Проверяет, остался ли следующий элемент для отображения.
template <class T> bool MapGenerator<T>::has_next() const {
    return source->get_length().is_infinite() ||
           position < source->get_length().get_finite_count();
}

// Возвращает следующий отображенный элемент.
template <class T> T MapGenerator<T>::get_next(const Sequence<T> &) {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

// Создает глубокую копию генератора map.
template <class T> Generator<T> *MapGenerator<T>::clone() const {
    return new MapGenerator<T>(*this);
}

// Возвращает длину исходника, потому что map ее не меняет.
template <class T> OrdinalLength MapGenerator<T>::get_length() const {
    return source->get_length();
}

// Возвращает отображенный элемент по ординальному индексу.
template <class T> T MapGenerator<T>::get_at(const OrdinalIndex &index) const {
    return func(source->get(index));
}
