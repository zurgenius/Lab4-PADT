#pragma once

#include <stdexcept>

template <class T>
RuleGenerator<T>::RuleGenerator(T (*rule)(const Sequence<T> &source),
                                const Sequence<T> *source)
    : rule(rule), source(source), position(source == nullptr ? 0 : source->get_count()) {
    if (rule == nullptr || source == nullptr || source->get_count() == 0) {
        throw std::invalid_argument("RuleGenerator needs rule and non-empty source");
    }
}

template <class T>
RuleGenerator<T>::RuleGenerator(const RuleGenerator<T> &other)
    : rule(other.rule), source(nullptr), position(other.position) {}

template <class T> bool RuleGenerator<T>::has_next() const { return true; }

template <class T> T RuleGenerator<T>::get_next() {
    if (source == nullptr) {
        throw std::logic_error("RuleGenerator source is not bound");
    }
    position++;
    return rule(*source);
}

template <class T> Generator<T> *RuleGenerator<T>::clone() const {
    return new RuleGenerator<T>(*this);
}

template <class T> OrdinalLength RuleGenerator<T>::get_length() const {
    return OrdinalLength::omega();
}

template <class T> T RuleGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (!index.is_finite()) {
        throw std::out_of_range("RuleGenerator supports finite indexes only");
    }
    throw std::logic_error("RuleGenerator get_at requires LazySequence memoization");
}

template <class T>
void RuleGenerator<T>::bind_source(const Sequence<T> *new_source) {
    source = new_source;
    position = source == nullptr ? 0 : source->get_count();
}
