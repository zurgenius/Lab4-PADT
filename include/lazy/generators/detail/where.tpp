#pragma once

#include <stdexcept>

template <class T>
WhereGenerator<T>::WhereGenerator(const LazySequence<T> &source,
                                  bool (*predicate)(const T &item))
    : source(new LazySequence<T>(source)), predicate(predicate), source_position(0) {
    if (predicate == nullptr) {
        throw std::invalid_argument("Where predicate is nullptr");
    }
}

template <class T>
WhereGenerator<T>::WhereGenerator(const WhereGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), predicate(other.predicate),
      source_position(other.source_position) {}

template <class T> WhereGenerator<T>::~WhereGenerator() { delete source; }

template <class T> bool WhereGenerator<T>::has_next() const {
    return source->get_length().is_infinite() ||
           source_position < source->get_length().get_finite_count();
}

template <class T> T WhereGenerator<T>::get_next() {
    while (has_next()) {
        T value = source->get(source_position++);
        if (predicate(value)) {
            return value;
        }
    }
    throw std::out_of_range("WhereGenerator has no next item");
}

template <class T> void WhereGenerator<T>::reset() { source_position = 0; }

template <class T> Generator<T> *WhereGenerator<T>::clone() const {
    return new WhereGenerator<T>(*this);
}

template <class T> OrdinalLength WhereGenerator<T>::get_length() const {
    return source->get_length();
}

template <class T> T WhereGenerator<T>::get_at(const OrdinalIndex &) const {
    throw std::logic_error("WhereGenerator random access is not supported");
}
