#pragma once

#include "lazy/generators/base.h"

template <class T> class InterleaveGenerator : public Generator<T> {
  private:
    LazySequence<T> **sources;
    int source_count;
    int position;
    OrdinalLength length;

  public:
    InterleaveGenerator(const LazySequence<T> **items, int count);
    InterleaveGenerator(const InterleaveGenerator<T> &other);
    ~InterleaveGenerator() override;

    bool has_next() const override;
    T get_next(const Sequence<T> &history) override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
