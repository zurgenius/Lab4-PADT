#pragma once

#include <stdexcept>

template <class T>
Cache<T>::Cache(int capacity)
    : buffer(capacity), capacity(capacity), cached_count(0), first_physical_index(0),
      first_logical_index(0), next_logical_index(0) {
    if (capacity <= 0) {
        throw std::invalid_argument("Cache capacity must be positive");
    }
}

template <class T>
Cache<T>::Cache(const Cache<T> &other)
    : buffer(other.buffer), capacity(other.capacity), cached_count(other.cached_count),
      first_physical_index(other.first_physical_index),
      first_logical_index(other.first_logical_index),
      next_logical_index(other.next_logical_index) {}

template <class T> Cache<T> &Cache<T>::operator=(const Cache<T> &other) {
    if (this == &other) {
        return *this;
    }

    DynamicArray<T> copied_buffer(other.buffer);
    buffer.resize(copied_buffer.get_size());
    for (int index = 0; index < copied_buffer.get_size(); index++) {
        buffer.set(index, copied_buffer.get(index));
    }
    capacity = other.capacity;
    cached_count = other.cached_count;
    first_physical_index = other.first_physical_index;
    first_logical_index = other.first_logical_index;
    next_logical_index = other.next_logical_index;
    return *this;
}

template <class T> int Cache<T>::physical_index(int logical_index) const {
    if (!contains(logical_index)) {
        throw std::out_of_range("Index is outside Cache window");
    }
    return (first_physical_index + logical_index - first_logical_index) % capacity;
}

template <class T> bool Cache<T>::is_empty() const { return cached_count == 0; }

template <class T> bool Cache<T>::contains(int logical_index) const {
    if (cached_count == 0) {
        return false;
    }
    return logical_index >= first_logical_index && logical_index <= get_last_index();
}

template <class T> int Cache<T>::get_cache_count() const { return cached_count; }

template <class T> int Cache<T>::get_capacity() const { return capacity; }

template <class T> int Cache<T>::get_first_index() const {
    if (cached_count == 0) {
        throw std::out_of_range("Cache is empty");
    }
    return first_logical_index;
}

template <class T> int Cache<T>::get_last_index() const {
    if (cached_count == 0) {
        throw std::out_of_range("Cache is empty");
    }
    return next_logical_index - 1;
}

template <class T> void Cache<T>::push(const T &item) {
    if (cached_count < capacity) {
        int write_index = (first_physical_index + cached_count) % capacity;
        buffer.set(write_index, item);
        cached_count++;
    } else {
        buffer.set(first_physical_index, item);
        first_physical_index = (first_physical_index + 1) % capacity;
        first_logical_index++;
    }
    next_logical_index++;
}

template <class T> void Cache<T>::clear() {
    cached_count = 0;
    first_physical_index = 0;
    first_logical_index = 0;
    next_logical_index = 0;
}

template <class T> const T &Cache<T>::get_first() const {
    if (cached_count == 0) {
        throw std::out_of_range("Cache is empty");
    }
    return get(first_logical_index);
}

template <class T> const T &Cache<T>::get_last() const {
    if (cached_count == 0) {
        throw std::out_of_range("Cache is empty");
    }
    return get(next_logical_index - 1);
}

template <class T> const T &Cache<T>::get(int index) const {
    return buffer.get(physical_index(index));
}

template <class T> Option<T> Cache<T>::try_get_first() const {
    return cached_count == 0 ? Option<T>::None() : Option<T>::Some(get_first());
}

template <class T> Option<T> Cache<T>::try_get_last() const {
    return cached_count == 0 ? Option<T>::None() : Option<T>::Some(get_last());
}

template <class T> Option<T> Cache<T>::try_get(int index) const {
    return contains(index) ? Option<T>::Some(get(index)) : Option<T>::None();
}

template <class T> int Cache<T>::get_count() const { return next_logical_index; }

template <class T> Sequence<T> *Cache<T>::get_sub_sequence(int, int) {
    throw std::logic_error("Cache does not support get_sub_sequence");
}

template <class T> Sequence<T> *Cache<T>::append(const T &) {
    throw std::logic_error("Cache is read-only");
}

template <class T> Sequence<T> *Cache<T>::prepend(const T &) {
    throw std::logic_error("Cache is read-only");
}

template <class T> Sequence<T> *Cache<T>::insert_at(const T &, int) {
    throw std::logic_error("Cache is read-only");
}

template <class T> Sequence<T> *Cache<T>::concat(const Sequence<T> *) {
    throw std::logic_error("Cache does not support concat");
}

template <class T> Sequence<T> *Cache<T>::map(T (*)(const T &)) {
    throw std::logic_error("Cache does not support map");
}

template <class T> Sequence<T> *Cache<T>::where(bool (*)(const T &)) {
    throw std::logic_error("Cache does not support where");
}

template <class T>
T Cache<T>::reduce(T (*)(const T &, const T &), const T &) {
    throw std::logic_error("Cache does not support reduce");
}

template <class T>
Sequence<T> *Cache<T>::slice(int, int, const Sequence<T> *) {
    throw std::logic_error("Cache does not support slice");
}

template <class T> IEnumerator<T> *Cache<T>::get_enumerator() const {
    throw std::logic_error("Cache does not support enumerator");
}
