#pragma once

#include <stdexcept>

template <class T>
AppendGenerator<T>::AppendGenerator(const LazySequence<T> &source, const T &item)
    : source(new LazySequence<T>(source)), item(item), position(0),
      length(OrdinalLength::add(source.get_length(), OrdinalLength::finite(1))) {}

template <class T>
AppendGenerator<T>::AppendGenerator(const AppendGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), item(other.item), position(other.position),
      length(other.length) {}

template <class T> AppendGenerator<T>::~AppendGenerator() { delete source; }

template <class T> bool AppendGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T AppendGenerator<T>::get_next() {
    if (!has_next()) {
        throw std::out_of_range("AppendGenerator has no next item");
    }
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> Generator<T> *AppendGenerator<T>::clone() const {
    return new AppendGenerator<T>(*this);
}

template <class T> OrdinalLength AppendGenerator<T>::get_length() const { return length; }

template <class T> T AppendGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (source->get_length().contains(index)) {
        return source->get(index);
    }
    OrdinalIndex local = source->get_length().subtract_prefix(index);
    if (local.is_finite() && local.get_finite_index() == 0) {
        return item;
    }
    throw std::out_of_range("AppendGenerator index is out of range");
}
