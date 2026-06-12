#pragma once

#include "dynamic_array.h"
#include "sequence.h"

template <class T> class Cache : public Sequence<T> {
  private:
    DynamicArray<T> buffer;
    int capacity;
    int cached_count;
    // Физический индекс указывает ячейку кэша, логический - позицию в последовательности.
    int first_physical_index;
    int first_logical_index;
    int next_logical_index;

    // Переводит индекс последовательности в текущую ячейку кольцевого буфера.
    int physical_index(int logical_index) const;

  public:
    int get_cache_count() const;
    int get_capacity() const;
    int get_first_index() const;
    int get_last_index() const;

    Cache(int capacity = 50);
    Cache(const Cache<T> &other);
    // Нужен для присваивания LazySequence вместе с текущим окном кэша.
    Cache<T> &operator=(const Cache<T> &other);

    bool is_empty() const;
    bool contains(int logical_index) const;
    void push(const T &item);

    const T &get_first() const override;
    const T &get_last() const override;
    const T &get(int index) const override;
    Option<T> try_get_first() const override;
    Option<T> try_get_last() const override;
    Option<T> try_get(int index) const override;
    int get_count() const override;

    // Закрытые методы интерфейса Sequence: Cache их не поддерживает и бросает logic_error.
    Sequence<T> *get_sub_sequence(int start, int end) override;
    Sequence<T> *append(const T &item) override;
    Sequence<T> *prepend(const T &item) override;
    Sequence<T> *insert_at(const T &item, int index) override;
    Sequence<T> *concat(const Sequence<T> *other) override;
    Sequence<T> *map(T (*func)(const T &elem)) override;
    Sequence<T> *where(bool (*predicate)(const T &elem)) override;
    T reduce(T (*func)(const T &first_elem, const T &second_elem),
             const T &initial_elem) override;
    Sequence<T> *slice(int index, int count, const Sequence<T> *replace_seq = nullptr) override;
    IEnumerator<T> *get_enumerator() const override;
};

#include "lazy/detail/cache.tpp"
