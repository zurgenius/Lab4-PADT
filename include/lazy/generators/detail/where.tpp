#pragma once

#include <stdexcept>

// Создает генератор ленивой фильтрации источника.
template <class T>
WhereGenerator<T>::WhereGenerator(const LazySequence<T> &source,
                                  bool (*predicate)(const T &item))
    : source(new LazySequence<T>(source)), predicate(predicate), source_position(0) {
    if (predicate == nullptr) {
        throw std::invalid_argument("Where predicate is nullptr");
    }
}

// Копирует генератор where вместе с источником и предикатом.
template <class T>
WhereGenerator<T>::WhereGenerator(const WhereGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), predicate(other.predicate),
      source_position(other.source_position) {}

// Освобождает копию исходной последовательности.
template <class T> WhereGenerator<T>::~WhereGenerator() { delete source; }

// Проверяет, можно ли продолжать поиск подходящего элемента.
template <class T> bool WhereGenerator<T>::has_next() const {
    return source->get_length().is_infinite() ||
           source_position < source->get_length().get_finite_count();
}

// Ищет и возвращает следующий элемент, удовлетворяющий предикату.
template <class T> T WhereGenerator<T>::get_next() {
    while (has_next()) {
        T value = source->get(source_position++);
        if (predicate(value)) {
            return value;
        }
    }
    throw std::out_of_range("WhereGenerator has no next item");
}

// Создает глубокую копию генератора where.
template <class T> Generator<T> *WhereGenerator<T>::clone() const {
    return new WhereGenerator<T>(*this);
}

// Возвращает длину исходника как верхнюю оценку фильтра.
template <class T> OrdinalLength WhereGenerator<T>::get_length() const {
    return source->get_length();
}

// Запрещает random access для фильтра без последовательного прохода.
template <class T> T WhereGenerator<T>::get_at(const OrdinalIndex &) const {
    throw std::logic_error("WhereGenerator random access is not supported");
}
