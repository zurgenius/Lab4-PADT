#pragma once

#include <stdexcept>

template <class T>
LazySequence<T>::LazySequence()
    : cache(kDefaultHistoryCapacity), generator(nullptr), length(OrdinalLength::finite(0)) {}

template <class T>
LazySequence<T>::LazySequence(const T *items, int count)
    : LazySequence(items, count, kDefaultHistoryCapacity) {}

template <class T>
LazySequence<T>::LazySequence(const T *items, int count, int history_capacity)
    : cache(history_capacity), generator(nullptr), length(OrdinalLength::finite(count)) {
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
    : LazySequence(source, kDefaultHistoryCapacity) {}

template <class T>
LazySequence<T>::LazySequence(const Sequence<T> &source, int history_capacity)
    : cache(history_capacity), generator(nullptr), length(OrdinalLength::finite(source.get_count())) {
    for (int index = 0; index < source.get_count(); index++) {
        append_materialized(source.get(index));
    }
}

template <class T>
LazySequence<T>::LazySequence(T (*rule)(const Sequence<T> &source),
                              const Sequence<T> &initial_values)
    : LazySequence(rule, initial_values, kDefaultHistoryCapacity) {}

template <class T>
LazySequence<T>::LazySequence(T (*rule)(const Sequence<T> &source),
                              const Sequence<T> &initial_values, int history_capacity)
    : cache(history_capacity), generator(nullptr), length(OrdinalLength::omega()) {
    if (initial_values.get_count() == 0) {
        throw std::invalid_argument("Initial values cannot be empty");
    }
    for (int index = 0; index < initial_values.get_count(); index++) {
        append_materialized(initial_values.get(index));
    }
    generator = new RuleGenerator<T>(rule, &cache);
}

template <class T>
LazySequence<T>::LazySequence(Generator<T> *generator)
    : LazySequence(generator, kDefaultHistoryCapacity) {}

template <class T>
LazySequence<T>::LazySequence(Generator<T> *generator, int history_capacity)
    : cache(history_capacity), generator(generator),
      length(generator == nullptr ? OrdinalLength::finite(0) : generator->get_length()) {
    if (generator == nullptr) {
        throw std::invalid_argument("Generator cannot be nullptr");
    }
    generator->bind_source(&cache);
}

template <class T>
LazySequence<T>::LazySequence(const LazySequence<T> &other)
    : cache(other.cache), generator(nullptr), length(other.length) {
    if (other.generator != nullptr) {
        generator = other.generator->clone();
        generator->bind_source(&cache);
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
    cache = other.cache;
    generator = new_generator;
    length = other.length;
    if (generator != nullptr) {
        generator->bind_source(&cache);
    }
    return *this;
}

template <class T> LazySequence<T>::~LazySequence() { delete generator; }

template <class T> void LazySequence<T>::append_materialized(const T &item) const {
    cache.push(item);
}

template <class T> void LazySequence<T>::materialize_to(int index) const {
    if (index < 0) {
        throw std::out_of_range("Index cannot be negative");
    }
    if (length.is_finite() && index >= length.get_finite_count()) {
        throw std::out_of_range("Index is out of range");
    }
    if (cache.contains(index)) {
        return;
    }
    if (!cache.is_empty() && index < cache.get_first_index()) {
        throw std::out_of_range("Index is outside LazySequence history cache");
    }
    while (cache.get_count() <= index) {
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
    return cache.get(index);
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

template <class T> Option<T> LazySequence<T>::try_get_first() const {
    try {
        return Option<T>::Some(get_first());
    } catch (const std::out_of_range &) {
        return Option<T>::None();
    } catch (const std::logic_error &) {
        return Option<T>::None();
    }
}

template <class T> Option<T> LazySequence<T>::try_get_last() const {
    try {
        return Option<T>::Some(get_last());
    } catch (const std::out_of_range &) {
        return Option<T>::None();
    } catch (const std::logic_error &) {
        return Option<T>::None();
    }
}

template <class T> Option<T> LazySequence<T>::try_get(int index) const {
    try {
        return Option<T>::Some(get(index));
    } catch (const std::out_of_range &) {
        return Option<T>::None();
    } catch (const std::logic_error &) {
        return Option<T>::None();
    }
}

template <class T> int LazySequence<T>::get_count() const {
    if (length.is_infinite()) {
        throw std::logic_error("Infinite LazySequence has no finite count");
    }
    return length.get_finite_count();
}

template <class T> OrdinalLength LazySequence<T>::get_length() const { return length; }

template <class T> int LazySequence<T>::get_materialized_count() const {
    return cache.get_cache_count();
}

template <class T> int LazySequence<T>::get_history_capacity() const {
    return cache.get_capacity();
}

template <class T> int LazySequence<T>::get_materialized_start() const {
    return cache.is_empty() ? 0 : cache.get_first_index();
}

template <class T> bool LazySequence<T>::is_infinite() const { return length.is_infinite(); }

template <class T> Sequence<T> *LazySequence<T>::get_sub_sequence(int, int) {
    throw std::logic_error("get_sub_sequence() not supported on LazySequence");
}

template <class T> LazySequence<T> *LazySequence<T>::append(const T &item) {
    return new LazySequence<T>(new AppendGenerator<T>(*this, item), cache.get_capacity());
}

template <class T> LazySequence<T> *LazySequence<T>::prepend(const T &item) {
    return new LazySequence<T>(new PrependGenerator<T>(*this, item), cache.get_capacity());
}

template <class T> LazySequence<T> *LazySequence<T>::insert_at(const T &item, int index) {
    return new LazySequence<T>(new InsertItemGenerator<T>(*this, item, OrdinalIndex::finite(index)),
                               cache.get_capacity());
}

template <class T> Sequence<T> *LazySequence<T>::concat(const Sequence<T> *) {
    throw std::logic_error("concat(Sequence*) not supported on LazySequence");
}

template <class T> LazySequence<T> *LazySequence<T>::map(T (*func)(const T &item)) {
    return new LazySequence<T>(new MapGenerator<T>(*this, func), cache.get_capacity());
}

template <class T> LazySequence<T> *LazySequence<T>::where(bool (*predicate)(const T &item)) {
    return new LazySequence<T>(new WhereGenerator<T>(*this, predicate), cache.get_capacity());
}

template <class T>
T LazySequence<T>::reduce(T (*)(const T &first_elem, const T &second_elem), const T &) {
    throw std::logic_error("reduce() not supported on LazySequence");
}

template <class T>
Sequence<T> *LazySequence<T>::slice(int, int, const Sequence<T> *) {
    throw std::logic_error("slice() not supported on LazySequence");
}

template <class T> IEnumerator<T> *LazySequence<T>::get_enumerator() const {
    throw std::logic_error("get_enumerator() not supported on LazySequence");
}

template <class T>
LazySequence<T> *LazySequence<T>::insert_sequence_at(const LazySequence<T> &items,
                                                     const OrdinalIndex &index) const {
    return new LazySequence<T>(new InsertSequenceGenerator<T>(*this, items, index),
                               cache.get_capacity());
}

template <class T> LazySequence<T> *LazySequence<T>::concat(const LazySequence<T> &other) const {
    return new LazySequence<T>(new ConcatGenerator<T>(*this, other), cache.get_capacity());
}

template <class T> LazySequence<T> *LazySequence<T>::take(int count) const {
    return new LazySequence<T>(new TakeGenerator<T>(*this, count), cache.get_capacity());
}

template <class T>
LazySequence<T> *LazySequence<T>::interleave(const LazySequence<T> **sources, int source_count) {
    return new LazySequence<T>(new InterleaveGenerator<T>(sources, source_count));
}
