#pragma once

#include "lazy/lazy_sequence.h"

#include <stdexcept>

// Создает пустую конечную ленивую последовательность.
template <class T>
LazySequence<T>::LazySequence()
    : cache(kDefaultHistoryCapacity), generator(nullptr), length(OrdinalLength::finite(0)) {}

// Создает конечную ленивую последовательность из массива с размером кэша по умолчанию.
template <class T>
LazySequence<T>::LazySequence(const T *items, int count)
    : LazySequence(items, count, kDefaultHistoryCapacity) {}

// Создает конечную ленивую последовательность из массива с заданным размером истории.
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

// Создает конечную ленивую последовательность из обычной Sequence с размером кэша по умолчанию.
template <class T>
LazySequence<T>::LazySequence(const Sequence<T> &source)
    : LazySequence(source, kDefaultHistoryCapacity) {}

// Создает конечную ленивую последовательность из обычной Sequence с заданным размером истории.
template <class T>
LazySequence<T>::LazySequence(const Sequence<T> &source, int history_capacity)
    : cache(history_capacity), generator(nullptr), length(OrdinalLength::finite(source.get_count())) {
    for (int index = 0; index < source.get_count(); index++) {
        append_materialized(source.get(index));
    }
}

// Создает бесконечную последовательность по правилу и начальным значениям.
template <class T>
LazySequence<T>::LazySequence(T (*rule)(const Sequence<T> &source),
                              const Sequence<T> &initial_values)
    : LazySequence(rule, initial_values, kDefaultHistoryCapacity) {}

// Создает бесконечную последовательность по правилу с заданным размером истории.
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
    generator = new RuleGenerator<T>(rule);
}

// Принимает владение готовым генератором и использует размер кэша по умолчанию.
template <class T>
LazySequence<T>::LazySequence(Generator<T> *generator)
    : LazySequence(generator, kDefaultHistoryCapacity) {}

// Принимает владение готовым генератором и задает размер истории.
template <class T>
LazySequence<T>::LazySequence(Generator<T> *generator, int history_capacity)
    : cache(history_capacity), generator(generator),
      length(generator == nullptr ? OrdinalLength::finite(0) : generator->get_length()) {
    if (generator == nullptr) {
        throw std::invalid_argument("Generator cannot be nullptr");
    }
}

// Копирует lazy-последовательность вместе с кэшем и клоном генератора.
template <class T>
LazySequence<T>::LazySequence(const LazySequence<T> &other)
    : cache(other.cache), generator(nullptr), length(other.length) {
    if (other.generator != nullptr) {
        generator = other.generator->clone();
    }
}

// Присваивает lazy-последовательность вместе с кэшем, длиной и клоном генератора.
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
    return *this;
}

// Освобождает генератор, которым владеет lazy-последовательность.
template <class T> LazySequence<T>::~LazySequence() { delete generator; }

// Добавляет уже вычисленный элемент в окно истории.
template <class T> void LazySequence<T>::append_materialized(const T &item) const {
    cache.push(item);
}

// Догенерирует элементы до нужного конечного индекса.
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
        // передаем кэш чтобы убрать bind_source, все генераторы кроме Rule игнорируют кэщ
        append_materialized(generator->get_next(cache));
    }
}

template <class T> const T &LazySequence<T>::get_first() const { return get(0); }

// Возвращает последний элемент только для конечной ленивой последовательности.
template <class T> const T &LazySequence<T>::get_last() const {
    if (length.is_infinite()) {
        throw std::logic_error("Infinite LazySequence has no last item");
    }
    if (length.get_finite_count() == 0) {
        throw std::out_of_range("LazySequence is empty");
    }
    return get(length.get_finite_count() - 1);
}

// Возвращает элемент по конечному индексу, догенерируя префикс при необходимости.
template <class T> const T &LazySequence<T>::get(int index) const {
    materialize_to(index);
    return cache.get(index);
}

// Возвращает элемент по ординальному индексу после возможного бесконечного префикса.
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

// Безопасно возвращает первый элемент, если он доступен.
template <class T> Option<T> LazySequence<T>::try_get_first() const {
    try {
        return Option<T>::Some(get_first());
    } catch (const std::out_of_range &) {
        return Option<T>::None();
    } catch (const std::logic_error &) {
        return Option<T>::None();
    }
}

// Безопасно возвращает последний элемент, если последовательность конечная и непустая.
template <class T> Option<T> LazySequence<T>::try_get_last() const {
    try {
        return Option<T>::Some(get_last());
    } catch (const std::out_of_range &) {
        return Option<T>::None();
    } catch (const std::logic_error &) {
        return Option<T>::None();
    }
}

// Безопасно возвращает элемент по конечному индексу.
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

// Возвращает ординальную длину последовательности.
template <class T> OrdinalLength LazySequence<T>::get_length() const { return length; }

// Возвращает количество элементов, которые сейчас лежат в окне истории.
template <class T> int LazySequence<T>::get_materialized_count() const {
    return cache.get_cache_count();
}

// Возвращает логический индекс первого элемента в окне истории.
template <class T> int LazySequence<T>::get_materialized_start() const {
    return cache.is_empty() ? 0 : cache.get_first_index();
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

template <class T> LazySequence<T> *LazySequence<T>::map(T (*func)(const T &item)) {
    return new LazySequence<T>(new MapGenerator<T>(*this, func), cache.get_capacity());
}

template <class T> LazySequence<T> *LazySequence<T>::where(bool (*predicate)(const T &item)) {
    return new LazySequence<T>(new WhereGenerator<T>(*this, predicate), cache.get_capacity());
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

// Ниже запрещенные методы Sequence: для LazySequence они архитектурно некорректны.

template <class T> Sequence<T> *LazySequence<T>::get_sub_sequence(int, int) {
    throw std::logic_error("get_sub_sequence() not supported on LazySequence");
}

template <class T> Sequence<T> *LazySequence<T>::concat(const Sequence<T> *) {
    throw std::logic_error("concat(Sequence*) not supported on LazySequence");
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
