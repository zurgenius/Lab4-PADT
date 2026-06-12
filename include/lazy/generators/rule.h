#pragma once

#include "lazy/generators/base.h"

template <class T> class RuleGenerator : public Generator<T> {
  private:
    T (*rule)(const Sequence<T> &source);

  public:
    RuleGenerator(T (*rule)(const Sequence<T> &source));
    RuleGenerator(const RuleGenerator<T> &other);

    bool has_next() const override;
    T get_next(const Sequence<T> &history) override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
