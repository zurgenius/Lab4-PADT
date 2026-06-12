#pragma once

#include "lazy/generators/base.h"

template <class T> class WhereGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    bool (*predicate)(const T &item);
    int source_position;

  public:
    WhereGenerator(const LazySequence<T> &source, bool (*predicate)(const T &item));
    WhereGenerator(const WhereGenerator<T> &other);
    ~WhereGenerator() override;

    bool has_next() const override;
    T get_next(const Sequence<T> &history) override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
