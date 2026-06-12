#pragma once

template <class T>
ConcatGenerator<T>::ConcatGenerator(const LazySequence<T> &first,
                                    const LazySequence<T> &second)
    : first(new LazySequence<T>(first)), second(new LazySequence<T>(second)), position(0),
      length(OrdinalLength::add(first.get_length(), second.get_length())) {}

template <class T>
ConcatGenerator<T>::ConcatGenerator(const ConcatGenerator<T> &other)
    : first(new LazySequence<T>(*other.first)), second(new LazySequence<T>(*other.second)),
      position(other.position), length(other.length) {}

template <class T> ConcatGenerator<T>::~ConcatGenerator() {
    delete first;
    delete second;
}

template <class T> bool ConcatGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T ConcatGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void ConcatGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *ConcatGenerator<T>::clone() const {
    return new ConcatGenerator<T>(*this);
}

template <class T> OrdinalLength ConcatGenerator<T>::get_length() const { return length; }

template <class T> T ConcatGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (first->get_length().contains(index)) {
        return first->get(index);
    }
    OrdinalIndex second_index = first->get_length().subtract_prefix(index);
    return second->get(second_index);
}
