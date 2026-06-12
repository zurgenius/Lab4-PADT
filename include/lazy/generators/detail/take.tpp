#pragma once

#include <stdexcept>

// Создает генератор конечного префикса заданной длины.
template <class T>
TakeGenerator<T>::TakeGenerator(const LazySequence<T> &source, int count)
    : source(new LazySequence<T>(source)), count(count), position(0) {
    if (count < 0) {
        throw std::invalid_argument("Take count cannot be negative");
    }
}

// Копирует генератор take вместе с источником.
template <class T>
TakeGenerator<T>::TakeGenerator(const TakeGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), count(other.count),
      position(other.position) {}

// Освобождает копию исходной последовательности.
template <class T> TakeGenerator<T>::~TakeGenerator() { delete source; }

// Проверяет, остались ли элементы в конечном префиксе.
template <class T> bool TakeGenerator<T>::has_next() const { return position < count; }

// Возвращает следующий элемент конечного префикса.
template <class T> T TakeGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

// Создает глубокую копию генератора take.
template <class T> Generator<T> *TakeGenerator<T>::clone() const {
    return new TakeGenerator<T>(*this);
}

// Возвращает конечную длину префикса.
template <class T> OrdinalLength TakeGenerator<T>::get_length() const {
    return OrdinalLength::finite(count);
}

// Возвращает элемент префикса по конечному индексу.
template <class T> T TakeGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (!index.is_finite() || index.get_finite_index() >= count) {
        throw std::out_of_range("TakeGenerator index is out of range");
    }
    return source->get(index.get_finite_index());
}
