#pragma once

#include <stdexcept>

// Создает генератор, который добавляет один элемент перед источником.
template <class T>
PrependGenerator<T>::PrependGenerator(const LazySequence<T> &source, const T &item)
    : source(new LazySequence<T>(source)), item(item), position(0),
      length(OrdinalLength::add(OrdinalLength::finite(1), source.get_length())) {}

// Копирует генератор prepend вместе с источником.
template <class T>
PrependGenerator<T>::PrependGenerator(const PrependGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), item(other.item), position(other.position),
      length(other.length) {}

// Освобождает копию исходной последовательности.
template <class T> PrependGenerator<T>::~PrependGenerator() { delete source; }

// Проверяет, остался ли следующий элемент после prepend.
template <class T> bool PrependGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

// Возвращает следующий элемент результата prepend.
template <class T> T PrependGenerator<T>::get_next() {
    if (!has_next()) {
        throw std::out_of_range("PrependGenerator has no next item");
    }
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

// Создает глубокую копию генератора prepend.
template <class T> Generator<T> *PrependGenerator<T>::clone() const {
    return new PrependGenerator<T>(*this);
}

// Возвращает ординальную длину результата prepend.
template <class T> OrdinalLength PrependGenerator<T>::get_length() const { return length; }

// Возвращает элемент результата prepend по ординальному индексу.
template <class T> T PrependGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (index.is_finite() && index.get_finite_index() == 0) {
        return item;
    }
    if (index.is_finite()) {
        return source->get(index.get_finite_index() - 1);
    }
    return source->get(index);
}
