#pragma once

#include "lazy/generators/base.h"

template <class T> class InsertItemGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    T item;
    OrdinalIndex insert_index;
    int position;
    OrdinalLength length;

    static OrdinalLength calculate_length(const LazySequence<T> &source,
                                          const OrdinalIndex &index);

  public:
    InsertItemGenerator(const LazySequence<T> &source, const T &item,
                        const OrdinalIndex &index);
    InsertItemGenerator(const InsertItemGenerator<T> &other);
    ~InsertItemGenerator() override;

    bool has_next() const override;
    T get_next() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
