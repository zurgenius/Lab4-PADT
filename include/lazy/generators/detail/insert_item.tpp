#pragma once

#include <stdexcept>

// Считает длину после вставки одного элемента.
template <class T>
OrdinalLength InsertItemGenerator<T>::calculate_length(const LazySequence<T> &source,
                                                       const OrdinalIndex &index) {
    if (index.is_finite() && source.get_length().is_infinite()) {
        return source.get_length();
    }
    return OrdinalLength::add(source.get_length(), OrdinalLength::finite(1));
}

// Создает генератор вставки одного элемента в lazy-последовательность.
template <class T>
InsertItemGenerator<T>::InsertItemGenerator(const LazySequence<T> &source, const T &item,
                                            const OrdinalIndex &index)
    : source(new LazySequence<T>(source)), item(item), insert_index(index), position(0),
      length(calculate_length(source, index)) {
    if (!source.get_length().contains(index) &&
        !(source.get_length().is_finite() && index.is_finite() &&
          index.get_finite_index() == source.get_length().get_finite_count())) {
        throw std::out_of_range("Insert index is out of range");
    }
}

// Копирует генератор вставки элемента вместе с источником.
template <class T>
InsertItemGenerator<T>::InsertItemGenerator(const InsertItemGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), item(other.item),
      insert_index(other.insert_index), position(other.position), length(other.length) {}

// Освобождает копию исходной последовательности.
template <class T> InsertItemGenerator<T>::~InsertItemGenerator() { delete source; }

// Проверяет, остался ли следующий элемент после вставки.
template <class T> bool InsertItemGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

// Возвращает следующий элемент результата вставки.
template <class T> T InsertItemGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

// Создает глубокую копию генератора вставки элемента.
template <class T> Generator<T> *InsertItemGenerator<T>::clone() const {
    return new InsertItemGenerator<T>(*this);
}

// Возвращает ординальную длину результата вставки.
template <class T> OrdinalLength InsertItemGenerator<T>::get_length() const { return length; }

// Возвращает элемент результата вставки по ординальному индексу.
template <class T> T InsertItemGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (index == insert_index) {
        return item;
    }
    if (insert_index.is_finite() && index.is_finite()) {
        int insert_at = insert_index.get_finite_index();
        int current = index.get_finite_index();
        return current < insert_at ? source->get(current) : source->get(current - 1);
    }
    return source->get(index);
}
