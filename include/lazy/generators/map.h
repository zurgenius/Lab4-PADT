#pragma once

#include "lazy/generators/base.h"

template <class T> class MapGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    T (*func)(const T &item);
    int position;

  public:
    MapGenerator(const LazySequence<T> &source, T (*func)(const T &item));
    MapGenerator(const MapGenerator<T> &other);
    ~MapGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
