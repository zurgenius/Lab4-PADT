#pragma once

#include "lazy/generators/base.h"

template <class T> class AppendGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    T item;
    int position;
    OrdinalLength length;

  public:
    AppendGenerator(const LazySequence<T> &source, const T &item);
    AppendGenerator(const AppendGenerator<T> &other);
    ~AppendGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
