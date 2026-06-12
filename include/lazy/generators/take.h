#pragma once

#include "lazy/generators/base.h"

template <class T> class TakeGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    int count;
    int position;

  public:
    TakeGenerator(const LazySequence<T> &source, int count);
    TakeGenerator(const TakeGenerator<T> &other);
    ~TakeGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
