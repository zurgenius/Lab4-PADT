#pragma once

#include "array_sequence.h"
#include "lazy/generators.h"
#include "lazy/cache.h"
#include "sequence.h"

template <class T> class LazySequence : public Sequence<T> {
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
    ~LazySequence() override;

    const T &get_first() const override;
    const T &get_last() const override;
    const T &get(int index) const override;
    T get(const OrdinalIndex &index) const;

    Option<T> try_get_first() const override;
    Option<T> try_get_last() const override;
    Option<T> try_get(int index) const override;
    int get_count() const override;
    OrdinalLength get_length() const;
    int get_materialized_count() const;
    int get_history_capacity() const;
    int get_materialized_start() const;
    bool is_infinite() const;

    Sequence<T> *get_sub_sequence(int start, int end) override;
    LazySequence<T> *append(const T &item) override;
    LazySequence<T> *prepend(const T &item) override;
    LazySequence<T> *insert_at(const T &item, int index) override;
    Sequence<T> *concat(const Sequence<T> *other) override;
    LazySequence<T> *map(T (*func)(const T &item)) override;
    LazySequence<T> *where(bool (*predicate)(const T &item)) override;
    T reduce(T (*func)(const T &first_elem, const T &second_elem),
             const T &initial_elem) override;
    Sequence<T> *slice(int index, int count, const Sequence<T> *replace_seq = nullptr) override;
    IEnumerator<T> *get_enumerator() const override;

    LazySequence<T> *insert_sequence_at(const LazySequence<T> &items,
                                        const OrdinalIndex &index) const;
    LazySequence<T> *concat(const LazySequence<T> &other) const;
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
