#pragma once

#include <stdexcept>

template <class T>
SequenceGenerator<T>::SequenceGenerator(const Sequence<T> &source) : data(), position(0) {
    for (int index = 0; index < source.get_count(); index++) {
        data.append(source.get(index));
    }
}

template <class T>
SequenceGenerator<T>::SequenceGenerator(const SequenceGenerator<T> &other)
    : data(other.data), position(other.position) {}

template <class T> bool SequenceGenerator<T>::has_next() const {
    return position < data.get_count();
}

template <class T> T SequenceGenerator<T>::get_next() {
    if (!has_next()) {
        throw std::out_of_range("SequenceGenerator has no next item");
    }
    return data.get(position++);
}

template <class T> void SequenceGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *SequenceGenerator<T>::clone() const {
    return new SequenceGenerator<T>(*this);
}

template <class T> OrdinalLength SequenceGenerator<T>::get_length() const {
    return OrdinalLength::finite(data.get_count());
}

template <class T> T SequenceGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (!index.is_finite()) {
        throw std::out_of_range("SequenceGenerator index is transfinite");
    }
    return data.get(index.get_finite_index());
}
