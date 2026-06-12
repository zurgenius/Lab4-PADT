#pragma once

#include "lazy/generators/base.h"

template <class T> class InsertSequenceGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    LazySequence<T> *inserted;
    OrdinalIndex insert_index;
    int position;
    OrdinalLength length;

    static OrdinalLength calculate_length(const LazySequence<T> &source,
                                          const LazySequence<T> &inserted,
                                          const OrdinalIndex &index);

  public:
    InsertSequenceGenerator(const LazySequence<T> &source, const LazySequence<T> &inserted,
                            const OrdinalIndex &index);
    InsertSequenceGenerator(const InsertSequenceGenerator<T> &other);
    ~InsertSequenceGenerator() override;

    bool has_next() const override;
    T get_next() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
