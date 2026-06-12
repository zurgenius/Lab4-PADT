#pragma once

#include "array_sequence.h"
#include "lazy/generators/base.h"

template <class T> class SequenceGenerator : public Generator<T> {
  private:
    MutableArraySequence<T> data;
    int position;

  public:
    explicit SequenceGenerator(const Sequence<T> &source);
    SequenceGenerator(const SequenceGenerator<T> &other);

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
