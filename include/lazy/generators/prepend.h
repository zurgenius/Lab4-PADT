#pragma once

#include "lazy/generators/base.h"

template <class T> class PrependGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    T item;
    int position;
    OrdinalLength length;

  public:
    PrependGenerator(const LazySequence<T> &source, const T &item);
    PrependGenerator(const PrependGenerator<T> &other);
    ~PrependGenerator() override;

    bool has_next() const override;
    T get_next(const Sequence<T> &history) override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
