#pragma once

#include <stdexcept>

// Создает бесконечный генератор по рекуррентному правилу и истории.
template <class T>
RuleGenerator<T>::RuleGenerator(T (*rule)(const Sequence<T> &source),
                                const Sequence<T> *source)
    : rule(rule), source(source), position(source == nullptr ? 0 : source->get_count()) {
    if (rule == nullptr || source == nullptr || source->get_count() == 0) {
        throw std::invalid_argument("RuleGenerator needs rule and non-empty source");
    }
}

// Копирует правило и позицию без привязки к старой истории.
template <class T>
RuleGenerator<T>::RuleGenerator(const RuleGenerator<T> &other)
    : rule(other.rule), source(nullptr), position(other.position) {}

// Правило всегда может вычислять следующий элемент.
template <class T> bool RuleGenerator<T>::has_next() const { return true; }

// Вычисляет следующий элемент по текущей истории.
template <class T> T RuleGenerator<T>::get_next() {
    if (source == nullptr) {
        throw std::logic_error("RuleGenerator source is not bound");
    }
    position++;
    return rule(*source);
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

// Привязывает генератор правила к новой истории кэша.
template <class T>
void RuleGenerator<T>::bind_source(const Sequence<T> *new_source) {
    source = new_source;
    position = source == nullptr ? 0 : source->get_count();
}
