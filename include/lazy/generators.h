#pragma once

#include "array_sequence.h"
#include "option.h"
#include "ordinal.h"
#include "sequence.h"

template <class T> class LazySequence;

template <class T> class Generator {
  public:
    virtual bool has_next() const = 0;
    virtual T get_next() = 0;

    virtual Option<T> try_get_next();
    virtual void reset() = 0;
    virtual Generator<T> *clone() const = 0;
    virtual OrdinalLength get_length() const = 0;
    virtual T get_at(const OrdinalIndex &index) const = 0;

    virtual void bind_materialized(MutableArraySequence<T> *source);
    virtual ~Generator();
};

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

template <class T> class RuleGenerator : public Generator<T> {
  private:
    T (*rule)(const Sequence<T> &source);
    MutableArraySequence<T> *source;
    int position;

  public:
    RuleGenerator(T (*rule)(const Sequence<T> &source), MutableArraySequence<T> *source);
    RuleGenerator(const RuleGenerator<T> &other);

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
    void bind_materialized(MutableArraySequence<T> *new_source) override;
};

template <class T> class AppendGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    T item;
    int position;
    OrdinalLength length;

  public:
    AppendGenerator(const LazySequence<T> &source, const T &item);
    AppendGenerator(const AppendGenerator<T> &other);
    ~AppendGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};

template <class T> class PrependGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    T item;
    int position;
    OrdinalLength length;

  public:
    PrependGenerator(const LazySequence<T> &source, const T &item);
    PrependGenerator(const PrependGenerator<T> &other);
    ~PrependGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};

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
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};

template <class T> class ConcatGenerator : public Generator<T> {
  private:
    LazySequence<T> *first;
    LazySequence<T> *second;
    int position;
    OrdinalLength length;

  public:
    ConcatGenerator(const LazySequence<T> &first, const LazySequence<T> &second);
    ConcatGenerator(const ConcatGenerator<T> &other);
    ~ConcatGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};

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
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};

template <class T> class InterleaveGenerator : public Generator<T> {
  private:
    LazySequence<T> **sources;
    int source_count;
    int position;
    OrdinalLength length;

  public:
    InterleaveGenerator(const LazySequence<T> **items, int count);
    InterleaveGenerator(const InterleaveGenerator<T> &other);
    ~InterleaveGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};

template <class T> class MapGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    T (*func)(const T &item);
    int position;

  public:
    MapGenerator(const LazySequence<T> &source, T (*func)(const T &item));
    MapGenerator(const MapGenerator<T> &other);
    ~MapGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};

template <class T> class WhereGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    bool (*predicate)(const T &item);
    int source_position;

  public:
    WhereGenerator(const LazySequence<T> &source, bool (*predicate)(const T &item));
    WhereGenerator(const WhereGenerator<T> &other);
    ~WhereGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};

template <class T> class TakeGenerator : public Generator<T> {
  private:
    LazySequence<T> *source;
    int count;
    int position;

  public:
    TakeGenerator(const LazySequence<T> &source, int count);
    TakeGenerator(const TakeGenerator<T> &other);
    ~TakeGenerator() override;

    bool has_next() const override;
    T get_next() override;
    void reset() override;
    Generator<T> *clone() const override;
    OrdinalLength get_length() const override;
    T get_at(const OrdinalIndex &index) const override;
};
