#pragma once

#include "lazy/generators/rule.h"

#include <stdexcept>

// Создает бесконечный генератор по рекуррентному правилу и истории.
template <class T>
RuleGenerator<T>::RuleGenerator(T (*rule)(const Sequence<T> &source)) : rule(rule) {
    if (rule == nullptr) {
        throw std::invalid_argument("RuleGenerator needs rule");
    }
}

// Копирует правило без привязки к внешнему кэшу.
template <class T>
RuleGenerator<T>::RuleGenerator(const RuleGenerator<T> &other)
    : rule(other.rule) {}

// Правило всегда может вычислять следующий элемент.
template <class T> bool RuleGenerator<T>::has_next() const { return true; }

// Вычисляет следующий элемент по текущей истории.
template <class T> T RuleGenerator<T>::get_next(const Sequence<T> &history) {
    if (history.get_count() == 0) {
        throw std::logic_error("RuleGenerator history is empty");
    }
    return rule(history);
}

// Создает копию генератора правила.
template <class T> Generator<T> *RuleGenerator<T>::clone() const {
    return new RuleGenerator<T>(*this);
}

// Возвращает длину omega для бесконечного правила.
template <class T> OrdinalLength RuleGenerator<T>::get_length() const {
    return OrdinalLength::omega();
}

// Запрещает произвольный доступ без мемоизации LazySequence.
template <class T> T RuleGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (!index.is_finite()) {
        throw std::out_of_range("RuleGenerator supports finite indexes only");
    }
    throw std::logic_error("RuleGenerator get_at requires LazySequence memoization");
}
