#pragma once

#include "lazy/generators/base.h"

template <class T> class ConcatGenerator : public Generator<T> {
  private:
    LazySequence<T> *first;
    LazySequence<T> *second;
    int position;
    OrdinalLength length;

  public:
    ConcatGenerator(const LazySequence<T> &first, const LazySequence<T> &second);
    ConcatGenerator(const ConcatGenerator<T> &other);
    ~ConcatGenerator() override;

    bool has_next() const override;
    T get_next(const Sequence<T> &history) override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
