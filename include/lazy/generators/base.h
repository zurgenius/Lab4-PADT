#pragma once

#include "option.h"
#include "ordinal.h"
#include "../../sequence.h"

template <class T> class LazySequence;

template <class T> class Generator {
  public:
    virtual bool has_next() const = 0;
    virtual T get_next(const Sequence<T> &history) = 0;

    virtual Generator<T> *clone() const = 0;
    virtual OrdinalLength get_length() const = 0;
    virtual T get_at(const OrdinalIndex &index) const = 0;

    virtual ~Generator();
};
