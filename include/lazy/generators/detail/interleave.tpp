#pragma once

#include <stdexcept>

template <class T>
InterleaveGenerator<T>::InterleaveGenerator(const LazySequence<T> **items, int count)
    : sources(nullptr), source_count(count), position(0), length(OrdinalLength::finite(0)) {
    if (items == nullptr || count <= 0) {
        throw std::invalid_argument("Interleave sources are empty");
    }
    sources = new LazySequence<T> *[count];
    bool has_infinite = false;
    int finite_min =
        items[0]->get_length().is_finite() ? items[0]->get_length().get_finite_count() : 0;
    for (int index = 0; index < count; index++) {
        if (items[index] == nullptr) {
            delete[] sources;
            sources = nullptr;
            throw std::invalid_argument("Interleave source is nullptr");
        }
        sources[index] = new LazySequence<T>(*items[index]);
        if (items[index]->get_length().is_infinite()) {
            has_infinite = true;
        } else if (items[index]->get_length().get_finite_count() < finite_min) {
            finite_min = items[index]->get_length().get_finite_count();
        }
    }
    length = has_infinite ? OrdinalLength::omega() : OrdinalLength::finite(finite_min * count);
}

template <class T>
InterleaveGenerator<T>::InterleaveGenerator(const InterleaveGenerator<T> &other)
    : sources(new LazySequence<T> *[other.source_count]), source_count(other.source_count),
      position(other.position), length(other.length) {
    for (int index = 0; index < source_count; index++) {
        sources[index] = new LazySequence<T>(*other.sources[index]);
    }
}

template <class T> InterleaveGenerator<T>::~InterleaveGenerator() {
    for (int index = 0; index < source_count; index++) {
        delete sources[index];
    }
    delete[] sources;
}

template <class T> bool InterleaveGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T InterleaveGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void InterleaveGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *InterleaveGenerator<T>::clone() const {
    return new InterleaveGenerator<T>(*this);
}

template <class T> OrdinalLength InterleaveGenerator<T>::get_length() const { return length; }

template <class T> T InterleaveGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (!index.is_finite()) {
        throw std::out_of_range("InterleaveGenerator supports finite indexes only");
    }
    int flat = index.get_finite_index();
    int source_index = flat % source_count;
    int item_index = flat / source_count;
    return sources[source_index]->get(item_index);
}
