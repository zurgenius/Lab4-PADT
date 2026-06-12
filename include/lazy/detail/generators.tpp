#pragma once

#include "lazy/lazy_sequence.h"

#include <stdexcept>

template <class T> Option<T> Generator<T>::try_get_next() {
    if (!has_next()) {
        return Option<T>::None();
    }
    return Option<T>::Some(get_next());
}

template <class T> void Generator<T>::bind_source(const Sequence<T> *) {}

template <class T> Generator<T>::~Generator() {}

template <class T>
SequenceGenerator<T>::SequenceGenerator(const Sequence<T> &source) : data(), position(0) {
    for (int index = 0; index < source.get_count(); index++) {
        data.append(source.get(index));
    }
}

template <class T>
SequenceGenerator<T>::SequenceGenerator(const SequenceGenerator<T> &other)
    : data(other.data), position(other.position) {}

template <class T> bool SequenceGenerator<T>::has_next() const {
    return position < data.get_count();
}

template <class T> T SequenceGenerator<T>::get_next() {
    if (!has_next()) {
        throw std::out_of_range("SequenceGenerator has no next item");
    }
    return data.get(position++);
}

template <class T> void SequenceGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *SequenceGenerator<T>::clone() const {
    return new SequenceGenerator<T>(*this);
}

template <class T> OrdinalLength SequenceGenerator<T>::get_length() const {
    return OrdinalLength::finite(data.get_count());
}

template <class T> T SequenceGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (!index.is_finite()) {
        throw std::out_of_range("SequenceGenerator index is transfinite");
    }
    return data.get(index.get_finite_index());
}

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

template <class T> void RuleGenerator<T>::reset() {
    position = source == nullptr ? 0 : source->get_count();
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

template <class T>
AppendGenerator<T>::AppendGenerator(const LazySequence<T> &source, const T &item)
    : source(new LazySequence<T>(source)), item(item), position(0),
      length(OrdinalLength::add(source.get_length(), OrdinalLength::finite(1))) {}

template <class T>
AppendGenerator<T>::AppendGenerator(const AppendGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), item(other.item), position(other.position),
      length(other.length) {}

template <class T> AppendGenerator<T>::~AppendGenerator() { delete source; }

template <class T> bool AppendGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T AppendGenerator<T>::get_next() {
    if (!has_next()) {
        throw std::out_of_range("AppendGenerator has no next item");
    }
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void AppendGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *AppendGenerator<T>::clone() const {
    return new AppendGenerator<T>(*this);
}

template <class T> OrdinalLength AppendGenerator<T>::get_length() const { return length; }

template <class T> T AppendGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (source->get_length().contains(index)) {
        return source->get(index);
    }
    OrdinalIndex local = source->get_length().subtract_prefix(index);
    if (local.is_finite() && local.get_finite_index() == 0) {
        return item;
    }
    throw std::out_of_range("AppendGenerator index is out of range");
}

template <class T>
PrependGenerator<T>::PrependGenerator(const LazySequence<T> &source, const T &item)
    : source(new LazySequence<T>(source)), item(item), position(0),
      length(OrdinalLength::add(OrdinalLength::finite(1), source.get_length())) {}

template <class T>
PrependGenerator<T>::PrependGenerator(const PrependGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), item(other.item), position(other.position),
      length(other.length) {}

template <class T> PrependGenerator<T>::~PrependGenerator() { delete source; }

template <class T> bool PrependGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T PrependGenerator<T>::get_next() {
    if (!has_next()) {
        throw std::out_of_range("PrependGenerator has no next item");
    }
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void PrependGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *PrependGenerator<T>::clone() const {
    return new PrependGenerator<T>(*this);
}

template <class T> OrdinalLength PrependGenerator<T>::get_length() const { return length; }

template <class T> T PrependGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (index.is_finite() && index.get_finite_index() == 0) {
        return item;
    }
    if (index.is_finite()) {
        return source->get(index.get_finite_index() - 1);
    }
    return source->get(index);
}

template <class T>
OrdinalLength InsertItemGenerator<T>::calculate_length(const LazySequence<T> &source,
                                                       const OrdinalIndex &index) {
    if (index.is_finite() && source.get_length().is_infinite()) {
        return source.get_length();
    }
    return OrdinalLength::add(source.get_length(), OrdinalLength::finite(1));
}

template <class T>
InsertItemGenerator<T>::InsertItemGenerator(const LazySequence<T> &source, const T &item,
                                            const OrdinalIndex &index)
    : source(new LazySequence<T>(source)), item(item), insert_index(index), position(0),
      length(calculate_length(source, index)) {
    if (!source.get_length().contains(index) &&
        !(source.get_length().is_finite() && index.is_finite() &&
          index.get_finite_index() == source.get_length().get_finite_count())) {
        throw std::out_of_range("Insert index is out of range");
    }
}

template <class T>
InsertItemGenerator<T>::InsertItemGenerator(const InsertItemGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), item(other.item),
      insert_index(other.insert_index), position(other.position), length(other.length) {}

template <class T> InsertItemGenerator<T>::~InsertItemGenerator() { delete source; }

template <class T> bool InsertItemGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T InsertItemGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void InsertItemGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *InsertItemGenerator<T>::clone() const {
    return new InsertItemGenerator<T>(*this);
}

template <class T> OrdinalLength InsertItemGenerator<T>::get_length() const { return length; }

template <class T> T InsertItemGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (index == insert_index) {
        return item;
    }
    if (insert_index.is_finite() && index.is_finite()) {
        int insert_at = insert_index.get_finite_index();
        int current = index.get_finite_index();
        return current < insert_at ? source->get(current) : source->get(current - 1);
    }
    return source->get(index);
}

template <class T>
ConcatGenerator<T>::ConcatGenerator(const LazySequence<T> &first,
                                    const LazySequence<T> &second)
    : first(new LazySequence<T>(first)), second(new LazySequence<T>(second)), position(0),
      length(OrdinalLength::add(first.get_length(), second.get_length())) {}

template <class T>
ConcatGenerator<T>::ConcatGenerator(const ConcatGenerator<T> &other)
    : first(new LazySequence<T>(*other.first)), second(new LazySequence<T>(*other.second)),
      position(other.position), length(other.length) {}

template <class T> ConcatGenerator<T>::~ConcatGenerator() {
    delete first;
    delete second;
}

template <class T> bool ConcatGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T ConcatGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void ConcatGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *ConcatGenerator<T>::clone() const {
    return new ConcatGenerator<T>(*this);
}

template <class T> OrdinalLength ConcatGenerator<T>::get_length() const { return length; }

template <class T> T ConcatGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (first->get_length().contains(index)) {
        return first->get(index);
    }
    OrdinalIndex second_index = first->get_length().subtract_prefix(index);
    return second->get(second_index);
}

template <class T>
OrdinalLength InsertSequenceGenerator<T>::calculate_length(const LazySequence<T> &source,
                                                           const LazySequence<T> &inserted,
                                                           const OrdinalIndex &index) {
    if (index.is_finite()) {
        OrdinalLength prefix = OrdinalLength::finite(index.get_finite_index());
        if (source.get_length().is_finite()) {
            int suffix_count = source.get_length().get_finite_count() - index.get_finite_index();
            return OrdinalLength::add(OrdinalLength::add(prefix, inserted.get_length()),
                                      OrdinalLength::finite(suffix_count));
        }
        return OrdinalLength::add(OrdinalLength::add(prefix, inserted.get_length()),
                                  OrdinalLength::omega());
    }
    return OrdinalLength::add(source.get_length(), inserted.get_length());
}

template <class T>
InsertSequenceGenerator<T>::InsertSequenceGenerator(const LazySequence<T> &source,
                                                    const LazySequence<T> &inserted,
                                                    const OrdinalIndex &index)
    : source(new LazySequence<T>(source)), inserted(new LazySequence<T>(inserted)),
      insert_index(index), position(0), length(calculate_length(source, inserted, index)) {
    if (!source.get_length().contains(index) &&
        !(source.get_length().is_finite() && index.is_finite() &&
          index.get_finite_index() == source.get_length().get_finite_count())) {
        throw std::out_of_range("Insert index is out of range");
    }
}

template <class T>
InsertSequenceGenerator<T>::InsertSequenceGenerator(const InsertSequenceGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), inserted(new LazySequence<T>(*other.inserted)),
      insert_index(other.insert_index), position(other.position), length(other.length) {}

template <class T> InsertSequenceGenerator<T>::~InsertSequenceGenerator() {
    delete source;
    delete inserted;
}

template <class T> bool InsertSequenceGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T InsertSequenceGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void InsertSequenceGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *InsertSequenceGenerator<T>::clone() const {
    return new InsertSequenceGenerator<T>(*this);
}

template <class T> OrdinalLength InsertSequenceGenerator<T>::get_length() const {
    return length;
}

template <class T> T InsertSequenceGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (insert_index.is_finite() && index.is_finite()) {
        int insert_at = insert_index.get_finite_index();
        int current = index.get_finite_index();
        if (current < insert_at) {
            return source->get(current);
        }
        OrdinalIndex inserted_index = OrdinalIndex::finite(current - insert_at);
        if (inserted->get_length().contains(inserted_index)) {
            return inserted->get(inserted_index);
        }
        OrdinalIndex after_inserted = inserted->get_length().subtract_prefix(inserted_index);
        return source->get(insert_at + after_inserted.get_finite_index());
    }
    if (source->get_length().contains(index)) {
        return source->get(index);
    }
    return inserted->get(source->get_length().subtract_prefix(index));
}

template <class T>
InterleaveGenerator<T>::InterleaveGenerator(const LazySequence<T> **items, int count)
    : sources(nullptr), source_count(count), position(0), length(OrdinalLength::finite(0)) {
    if (items == nullptr || count <= 0) {
        throw std::invalid_argument("Interleave sources are empty");
    }
    sources = new LazySequence<T> *[count];
    bool has_infinite = false;
    int finite_min =
        items[0]->get_length().is_finite() ? items[0]->get_length().get_finite_count() : 0;
    for (int index = 0; index < count; index++) {
        if (items[index] == nullptr) {
            delete[] sources;
            sources = nullptr;
            throw std::invalid_argument("Interleave source is nullptr");
        }
        sources[index] = new LazySequence<T>(*items[index]);
        if (items[index]->get_length().is_infinite()) {
            has_infinite = true;
        } else if (items[index]->get_length().get_finite_count() < finite_min) {
            finite_min = items[index]->get_length().get_finite_count();
        }
    }
    length = has_infinite ? OrdinalLength::omega() : OrdinalLength::finite(finite_min * count);
}

template <class T>
InterleaveGenerator<T>::InterleaveGenerator(const InterleaveGenerator<T> &other)
    : sources(new LazySequence<T> *[other.source_count]), source_count(other.source_count),
      position(other.position), length(other.length) {
    for (int index = 0; index < source_count; index++) {
        sources[index] = new LazySequence<T>(*other.sources[index]);
    }
}

template <class T> InterleaveGenerator<T>::~InterleaveGenerator() {
    for (int index = 0; index < source_count; index++) {
        delete sources[index];
    }
    delete[] sources;
}

template <class T> bool InterleaveGenerator<T>::has_next() const {
    return length.is_infinite() || position < length.get_finite_count();
}

template <class T> T InterleaveGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void InterleaveGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *InterleaveGenerator<T>::clone() const {
    return new InterleaveGenerator<T>(*this);
}

template <class T> OrdinalLength InterleaveGenerator<T>::get_length() const { return length; }

template <class T> T InterleaveGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (!index.is_finite()) {
        throw std::out_of_range("InterleaveGenerator supports finite indexes only");
    }
    int flat = index.get_finite_index();
    int source_index = flat % source_count;
    int item_index = flat / source_count;
    return sources[source_index]->get(item_index);
}

template <class T>
MapGenerator<T>::MapGenerator(const LazySequence<T> &source, T (*func)(const T &item))
    : source(new LazySequence<T>(source)), func(func), position(0) {
    if (func == nullptr) {
        throw std::invalid_argument("Map function is nullptr");
    }
}

template <class T>
MapGenerator<T>::MapGenerator(const MapGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), func(other.func), position(other.position) {}

template <class T> MapGenerator<T>::~MapGenerator() { delete source; }

template <class T> bool MapGenerator<T>::has_next() const {
    return source->get_length().is_infinite() ||
           position < source->get_length().get_finite_count();
}

template <class T> T MapGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void MapGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *MapGenerator<T>::clone() const {
    return new MapGenerator<T>(*this);
}

template <class T> OrdinalLength MapGenerator<T>::get_length() const {
    return source->get_length();
}

template <class T> T MapGenerator<T>::get_at(const OrdinalIndex &index) const {
    return func(source->get(index));
}

template <class T>
WhereGenerator<T>::WhereGenerator(const LazySequence<T> &source,
                                  bool (*predicate)(const T &item))
    : source(new LazySequence<T>(source)), predicate(predicate), source_position(0) {
    if (predicate == nullptr) {
        throw std::invalid_argument("Where predicate is nullptr");
    }
}

template <class T>
WhereGenerator<T>::WhereGenerator(const WhereGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), predicate(other.predicate),
      source_position(other.source_position) {}

template <class T> WhereGenerator<T>::~WhereGenerator() { delete source; }

template <class T> bool WhereGenerator<T>::has_next() const {
    return source->get_length().is_infinite() ||
           source_position < source->get_length().get_finite_count();
}

template <class T> T WhereGenerator<T>::get_next() {
    while (has_next()) {
        T value = source->get(source_position++);
        if (predicate(value)) {
            return value;
        }
    }
    throw std::out_of_range("WhereGenerator has no next item");
}

template <class T> void WhereGenerator<T>::reset() { source_position = 0; }

template <class T> Generator<T> *WhereGenerator<T>::clone() const {
    return new WhereGenerator<T>(*this);
}

template <class T> OrdinalLength WhereGenerator<T>::get_length() const {
    return source->get_length();
}

template <class T> T WhereGenerator<T>::get_at(const OrdinalIndex &) const {
    throw std::logic_error("WhereGenerator random access is not supported");
}

template <class T>
TakeGenerator<T>::TakeGenerator(const LazySequence<T> &source, int count)
    : source(new LazySequence<T>(source)), count(count), position(0) {
    if (count < 0) {
        throw std::invalid_argument("Take count cannot be negative");
    }
}

template <class T>
TakeGenerator<T>::TakeGenerator(const TakeGenerator<T> &other)
    : source(new LazySequence<T>(*other.source)), count(other.count),
      position(other.position) {}

template <class T> TakeGenerator<T>::~TakeGenerator() { delete source; }

template <class T> bool TakeGenerator<T>::has_next() const { return position < count; }

template <class T> T TakeGenerator<T>::get_next() {
    T value = get_at(OrdinalIndex::finite(position));
    position++;
    return value;
}

template <class T> void TakeGenerator<T>::reset() { position = 0; }

template <class T> Generator<T> *TakeGenerator<T>::clone() const {
    return new TakeGenerator<T>(*this);
}

template <class T> OrdinalLength TakeGenerator<T>::get_length() const {
    return OrdinalLength::finite(count);
}

template <class T> T TakeGenerator<T>::get_at(const OrdinalIndex &index) const {
    if (!index.is_finite() || index.get_finite_index() >= count) {
        throw std::out_of_range("TakeGenerator index is out of range");
    }
    return source->get(index.get_finite_index());
}
