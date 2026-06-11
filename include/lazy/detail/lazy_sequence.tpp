#pragma once

#include <stdexcept>


template <class T>
LazySequence<T>::LazySequence() : materialized(), generator(nullptr), length(OrdinalLength::finite(0)) {}

template <class T>
LazySequence<T>::LazySequence(const T *items, int count)
    : materialized(), generator(nullptr), length(OrdinalLength::finite(count)) {
    if (count < 0) {
        throw std::invalid_argument("Count cannot be negative");
    }
    if (items == nullptr && count > 0) {
        throw std::invalid_argument("Items cannot be nullptr");
    }
    for (int index = 0; index < count; index++) {
        append_materialized(items[index]);
    }
}

template <class T>
LazySequence<T>::LazySequence(const Sequence<T> &source)
    : materialized(), generator(nullptr), length(OrdinalLength::finite(source.get_count())) {
    for (int index = 0; index < source.get_count(); index++) {
        append_materialized(source.get(index));
    }
}

template <class T>
LazySequence<T>::LazySequence(T (*rule)(const Sequence<T> &source), const Sequence<T> &initial_values)
    : materialized(), generator(nullptr), length(OrdinalLength::omega()) {
    if (initial_values.get_count() == 0) {
        throw std::invalid_argument("Initial values cannot be empty");
    }
    for (int index = 0; index < initial_values.get_count(); index++) {
        append_materialized(initial_values.get(index));
    }
    generator = new RuleGenerator<T>(rule, &materialized);
}

template <class T>
LazySequence<T>::LazySequence(Generator<T> *generator)
    : materialized(), generator(generator), length(generator == nullptr ? OrdinalLength::finite(0) : generator->get_length()) {
    if (generator == nullptr) {
        throw std::invalid_argument("Generator cannot be nullptr");
    }
    generator->bind_materialized(&materialized);
}

template <class T>
LazySequence<T>::LazySequence(const LazySequence<T> &other)
    : materialized(other.materialized), generator(nullptr), length(other.length) {
    if (other.generator != nullptr) {
        generator = other.generator->clone();
        generator->bind_materialized(&materialized);
    }
}

template <class T> LazySequence<T> &LazySequence<T>::operator=(const LazySequence<T> &other) {
    if (this == &other) {
        return *this;
    }
    Generator<T> *new_generator = nullptr;
    if (other.generator != nullptr) {
        new_generator = other.generator->clone();
    }
    delete generator;
    materialized = other.materialized;
    generator = new_generator;
    length = other.length;
    if (generator != nullptr) {
        generator->bind_materialized(&materialized);
    }
    return *this;
}

template <class T> LazySequence<T>::~LazySequence() { delete generator; }

template <class T> void LazySequence<T>::append_materialized(const T &item) const {
    Sequence<T> *result = materialized.append(item);
    if (result != &materialized) {
        delete result;
        throw std::logic_error("LazySequence materialized storage is not mutable");
    }
}

template <class T> void LazySequence<T>::materialize_to(int index) const {
    if (index < 0) {
        throw std::out_of_range("Index cannot be negative");
    }
    if (length.is_finite() && index >= length.get_finite_count()) {
        throw std::out_of_range("Index is out of range");
    }
    while (materialized.get_count() <= index) {
        if (generator == nullptr || !generator->has_next()) {
            throw std::out_of_range("Generator has no next item");
        }
        append_materialized(generator->get_next());
    }
}

template <class T> const T &LazySequence<T>::get_first() const { return get(0); }

template <class T> const T &LazySequence<T>::get_last() const {
    if (length.is_infinite()) {
        throw std::logic_error("Infinite LazySequence has no last item");
    }
    if (length.get_finite_count() == 0) {
        throw std::out_of_range("LazySequence is empty");
    }
    return get(length.get_finite_count() - 1);
}

template <class T> const T &LazySequence<T>::get(int index) const {
    materialize_to(index);
    return materialized.get(index);
}

template <class T> T LazySequence<T>::get(const OrdinalIndex &index) const {
    if (index.is_finite()) {
        return get(index.get_finite_index());
    }
    if (!length.contains(index)) {
        throw std::out_of_range("Ordinal index is out of range");
    }
    if (generator == nullptr) {
        throw std::logic_error("Ordinal access requires generator");
    }
    return generator->get_at(index);
}

template <class T> const T &LazySequence<T>::operator[](int index) const { return get(index); }

template <class T> int LazySequence<T>::get_count() const {
    if (length.is_infinite()) {
        throw std::logic_error("Infinite LazySequence has no finite count");
    }
    return length.get_finite_count();
}

template <class T> OrdinalLength LazySequence<T>::get_length() const { return length; }
template <class T> int LazySequence<T>::get_materialized_count() const { return materialized.get_count(); }
template <class T> bool LazySequence<T>::is_infinite() const { return length.is_infinite(); }

template <class T> LazySequence<T> *LazySequence<T>::append(const T &item) const {
    return new LazySequence<T>(new AppendGenerator<T>(*this, item));
}

template <class T> LazySequence<T> *LazySequence<T>::prepend(const T &item) const {
    return new LazySequence<T>(new PrependGenerator<T>(*this, item));
}

template <class T> LazySequence<T> *LazySequence<T>::insert_at(const T &item, int index) const {
    return new LazySequence<T>(new InsertItemGenerator<T>(*this, item, OrdinalIndex::finite(index)));
}

template <class T>
LazySequence<T> *LazySequence<T>::insert_sequence_at(const LazySequence<T> &items,
                                                     const OrdinalIndex &index) const {
    return new LazySequence<T>(new InsertSequenceGenerator<T>(*this, items, index));
}

template <class T> LazySequence<T> *LazySequence<T>::concat(const LazySequence<T> &other) const {
    return new LazySequence<T>(new ConcatGenerator<T>(*this, other));
}

template <class T> LazySequence<T> *LazySequence<T>::map(T (*func)(const T &item)) const {
    return new LazySequence<T>(new MapGenerator<T>(*this, func));
}

template <class T> LazySequence<T> *LazySequence<T>::where(bool (*predicate)(const T &item)) const {
    return new LazySequence<T>(new WhereGenerator<T>(*this, predicate));
}

template <class T> LazySequence<T> *LazySequence<T>::take(int count) const {
    return new LazySequence<T>(new TakeGenerator<T>(*this, count));
}

template <class T>
LazySequence<T> *LazySequence<T>::interleave(const LazySequence<T> **sources, int source_count) {
    return new LazySequence<T>(new InterleaveGenerator<T>(sources, source_count));
}
