#pragma once

#include "lazy/generators/base.h"

template <class T> class RuleGenerator : public Generator<T> {
  private:
    T (*rule)(const Sequence<T> &source);
    const Sequence<T> *source;
    int position;

  public:
    RuleGenerator(T (*rule)(const Sequence<T> &source), const Sequence<T> *source);
    RuleGenerator(const RuleGenerator<T> &other);

    bool has_next() const override;
    T get_next() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
    void bind_source(const Sequence<T> *new_source) override;
};
