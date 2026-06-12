#pragma once

#include "array_sequence.h"
#include "lazy/generators.h"
#include "lazy/cache.h"
#include "sequence.h"

template <class T> class LazySequence {
  private:
    mutable Cache<T> cache;
    Generator<T> *generator;
    OrdinalLength length;

    void append_materialized(const T &item) const;
    void materialize_to(int index) const;

  public:
    static const int kDefaultHistoryCapacity = 50;

    LazySequence();
    LazySequence(const T *items, int count);
    LazySequence(const T *items, int count, int history_capacity);
    explicit LazySequence(const Sequence<T> &source);
    LazySequence(const Sequence<T> &source, int history_capacity);
    LazySequence(T (*rule)(const Sequence<T> &source), const Sequence<T> &initial_values);
    LazySequence(T (*rule)(const Sequence<T> &source), const Sequence<T> &initial_values,
                 int history_capacity);
    explicit LazySequence(Generator<T> *generator);
    LazySequence(Generator<T> *generator, int history_capacity);
    LazySequence(const LazySequence<T> &other);
    LazySequence<T> &operator=(const LazySequence<T> &other);
    ~LazySequence();

    const T &get_first() const;
    const T &get_last() const;
    const T &get(int index) const;
    T get(const OrdinalIndex &index) const;
    const T &operator[](int index) const;

    int get_count() const;
    OrdinalLength get_length() const;
    int get_materialized_count() const;
    int get_history_capacity() const;
    int get_materialized_start() const;
    bool is_infinite() const;

    LazySequence<T> *append(const T &item) const;
    LazySequence<T> *prepend(const T &item) const;
    LazySequence<T> *insert_at(const T &item, int index) const;
    LazySequence<T> *insert_sequence_at(const LazySequence<T> &items,
                                        const OrdinalIndex &index) const;
    LazySequence<T> *concat(const LazySequence<T> &other) const;
    LazySequence<T> *map(T (*func)(const T &item)) const;
    LazySequence<T> *where(bool (*predicate)(const T &item)) const;
    LazySequence<T> *take(int count) const;

    static LazySequence<T> *interleave(const LazySequence<T> **sources, int source_count);
};

#include "lazy/generators/detail/base.tpp"
#include "lazy/generators/detail/sequence.tpp"
#include "lazy/generators/detail/rule.tpp"
#include "lazy/generators/detail/append.tpp"
#include "lazy/generators/detail/prepend.tpp"
#include "lazy/generators/detail/insert_item.tpp"
#include "lazy/generators/detail/concat.tpp"
#include "lazy/generators/detail/insert_sequence.tpp"
#include "lazy/generators/detail/interleave.tpp"
#include "lazy/generators/detail/map.tpp"
#include "lazy/generators/detail/where.tpp"
#include "lazy/generators/detail/take.tpp"
#include "lazy/detail/lazy_sequence.tpp"
