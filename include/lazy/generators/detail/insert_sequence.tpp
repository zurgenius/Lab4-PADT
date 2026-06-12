#pragma once

#include <stdexcept>

template <class T>
OrdinalLength InsertSequenceGenerator<T>::calculate_length(const LazySequence<T> &source,
                                                           const LazySequence<T> &inserted,
                                                           const OrdinalIndex &index) {
    if (index.is_finite()) {
        OrdinalLength prefix = OrdinalLength::finite(index.get_finite_index());
        if (source.get_length().is_finite()) {
            int suffix_count = source.get_length().get_finite_count() - index.get_finite_index();
            return OrdinalLength::add(OrdinalLength::add(prefix, inserted.get_length()),
                                      OrdinalLength::finite(suffix_count));
        }
        return OrdinalLength::add(OrdinalLength::add(prefix, inserted.get_length()),
                                  OrdinalLength::omega());
    }
    return OrdinalLength::add(source.get_length(), inserted.get_length());
}

template <class T>
InsertSequenceGenerator<T>::InsertSequenceGenerator(const LazySequence<T> &source,
                                                    const LazySequence<T> &inserted,
                                                    const OrdinalIndex &index)
    : source(new LazySequence<T>(source)), inserted(new LazySequence<T>(inserted)),
      insert_index(index), position(0), length(calculate_length(source, inserted, index)) {
    if (!source.get_length().contains(index) &&
        !(source.get_length().is_finite() && index.is_finite() &&
          index.get_finite_index() == source.get_length().get_finite_count())) {
        throw std::out_of_range("Insert index is out of range");
    }
}

template <class T>
InsertSequenceGenerator<T>::InsertSequenceGenerator(const InsertSequenceGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), inserted(new LazySequence<T>(*other.inserted)),
      insert_index(other.insert_index), position(other.position), length(other.length) {}

template <class T> InsertSequenceGenerator<T>::~InsertSequenceGenerator() {
    delete source;
    delete inserted;
}

template <class T> bool InsertSequenceGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T InsertSequenceGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> Generator<T> *InsertSequenceGenerator<T>::clone() const {
    return new InsertSequenceGenerator<T>(*this);
}

template <class T> OrdinalLength InsertSequenceGenerator<T>::get_length() const {
    return length;
}

template <class T> T InsertSequenceGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (insert_index.is_finite() && index.is_finite()) {
        int insert_at = insert_index.get_finite_index();
        int current = index.get_finite_index();
        if (current < insert_at) {
            return source->get(current);
        }
        OrdinalIndex inserted_index = OrdinalIndex::finite(current - insert_at);
        if (inserted->get_length().contains(inserted_index)) {
            return inserted->get(inserted_index);
        }
        OrdinalIndex after_inserted = inserted->get_length().subtract_prefix(inserted_index);
        return source->get(insert_at + after_inserted.get_finite_index());
    }
    if (source->get_length().contains(index)) {
        return source->get(index);
    }
    return inserted->get(source->get_length().subtract_prefix(index));
}
