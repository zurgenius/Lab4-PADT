#include "lazy/lazy_sequence.h"

#include <gtest/gtest.h>
#include <type_traits>

int next_natural_rule(const Sequence<int> &source) {
    return source.get_count();
}

int next_increment_rule(const Sequence<int> &source) {
    return source.get_last() + 1;
}

int square_item(const int &item) {
    return item * item;
}

bool is_even_item(const int &item) {
    return item % 2 == 0;
}

int sum_items(const int &first, const int &second) {
    return first + second;
}

TEST(LazySequenceTest, InheritsSequenceInterface) {
    EXPECT_TRUE((std::is_base_of<Sequence<int>, LazySequence<int>>::value));
}

TEST(LazySequenceTest, ItemsConstructorStoresFiniteCount) {
    int items[] = {1, 2, 3};

    LazySequence<int> sequence(items, 3);

    EXPECT_EQ(sequence.get_count(), 3);
}

TEST(LazySequenceTest, GetMaterializesRequestedPrefix) {
    int items[] = {0};
    MutableArraySequence<int> initial(items, 1);
    LazySequence<int> sequence(next_natural_rule, initial);

    EXPECT_EQ(sequence.get(5), 5);
}

TEST(LazySequenceTest, RepeatedGetDoesNotGrowMaterializedCache) {
    int items[] = {0};
    MutableArraySequence<int> initial(items, 1);
    LazySequence<int> sequence(next_natural_rule, initial);
    sequence.get(5);
    int materialized = sequence.get_materialized_count();

    sequence.get(5);

    EXPECT_EQ(sequence.get_materialized_count(), materialized);
}

TEST(LazySequenceTest, GetKeepsOnlyConfiguredHistoryWindow) {
    int items[] = {0};
    MutableArraySequence<int> initial(items, 1);
    LazySequence<int> sequence(next_natural_rule, initial, 50);

    sequence.get(60);

    EXPECT_EQ(sequence.get_materialized_start(), 11);
    EXPECT_EQ(sequence.get_materialized_count(), 50);
}

TEST(LazySequenceTest, GetThrowsWhenIndexFellOutOfHistoryWindow) {
    int items[] = {0};
    MutableArraySequence<int> initial(items, 1);
    LazySequence<int> sequence(next_natural_rule, initial, 50);
    sequence.get(60);

    EXPECT_THROW(sequence.get(10), std::out_of_range);
}

TEST(LazySequenceTest, ConstructorUsesCustomHistoryCapacity) {
    int items[] = {0};
    MutableArraySequence<int> initial(items, 1);
    LazySequence<int> sequence(next_natural_rule, initial, 3);

    sequence.get(5);

    EXPECT_EQ(sequence.get_materialized_count(), 3);
}

TEST(LazySequenceTest, GetLastThrowsForInfiniteSequence) {
    int items[] = {0};
    MutableArraySequence<int> initial(items, 1);
    LazySequence<int> sequence(next_natural_rule, initial);

    EXPECT_THROW(sequence.get_last(), std::logic_error);
}

TEST(LazySequenceTest, AppendUsesLazyGenerator) {
    int items[] = {1, 2};
    LazySequence<int> sequence(items, 2);

    LazySequence<int> *result = sequence.append(3);

    EXPECT_EQ(result->get(2), 3);
    delete result;
}

TEST(LazySequenceTest, PrependUsesLazyGenerator) {
    int items[] = {1, 2};
    LazySequence<int> sequence(items, 2);

    LazySequence<int> *result = sequence.prepend(0);

    EXPECT_EQ(result->get(0), 0);
    delete result;
}

TEST(LazySequenceTest, InsertAtUsesLazyGenerator) {
    int items[] = {1, 3};
    LazySequence<int> sequence(items, 2);

    LazySequence<int> *result = sequence.insert_at(2, 1);

    EXPECT_EQ(result->get(1), 2);
    delete result;
}

TEST(LazySequenceTest, ConcatExposesSecondInfiniteSequenceAfterOmega) {
    int first_data[] = {0};
    int second_data[] = {10};
    MutableArraySequence<int> first_initial(first_data, 1);
    MutableArraySequence<int> second_initial(second_data, 1);
    LazySequence<int> first(next_increment_rule, first_initial);
    LazySequence<int> second(next_increment_rule, second_initial);

    LazySequence<int> *result = first.concat(second);

    EXPECT_EQ(result->get(OrdinalIndex::omega_plus(0)), 10);
    delete result;
}

TEST(LazySequenceTest, InterleaveReadsThreeInfiniteSourcesRoundRobin) {
    int first_data[] = {0};
    int second_data[] = {100};
    int third_data[] = {1000};
    MutableArraySequence<int> first_initial(first_data, 1);
    MutableArraySequence<int> second_initial(second_data, 1);
    MutableArraySequence<int> third_initial(third_data, 1);
    LazySequence<int> first(next_increment_rule, first_initial);
    LazySequence<int> second(next_increment_rule, second_initial);
    LazySequence<int> third(next_increment_rule, third_initial);
    const LazySequence<int> *sources[] = {&first, &second, &third};

    LazySequence<int> *result = LazySequence<int>::interleave(sources, 3);

    EXPECT_EQ(result->get(0), 0);
    EXPECT_EQ(result->get(1), 100);
    EXPECT_EQ(result->get(2), 1000);
    EXPECT_EQ(result->get(3), 1);
    EXPECT_EQ(result->get(4), 101);
    EXPECT_EQ(result->get(5), 1001);
    delete result;
}

TEST(LazySequenceTest, MapAppliesFunctionLazily) {
    int items[] = {2};
    LazySequence<int> sequence(items, 1);

    LazySequence<int> *result = sequence.map(square_item);

    EXPECT_EQ(result->get(0), 4);
    delete result;
}

TEST(LazySequenceTest, WhereSkipsItemsUntilPredicateMatches) {
    int items[] = {1, 2, 3};
    LazySequence<int> sequence(items, 3);

    LazySequence<int> *result = sequence.where(is_even_item);

    EXPECT_EQ(result->get(0), 2);
    delete result;
}

TEST(LazySequenceTest, TakeMakesInfiniteSequenceFinite) {
    int items[] = {0};
    MutableArraySequence<int> initial(items, 1);
    LazySequence<int> sequence(next_natural_rule, initial);

    LazySequence<int> *result = sequence.take(3);

    EXPECT_EQ(result->get_count(), 3);
    delete result;
}

TEST(LazySequenceTest, GetSubSequenceIsNotSupported) {
    int items[] = {1, 2, 3};
    LazySequence<int> sequence(items, 3);

    EXPECT_THROW(sequence.get_sub_sequence(0, 1), std::logic_error);
}

TEST(LazySequenceTest, SequenceConcatIsNotSupported) {
    int first_items[] = {1};
    int second_items[] = {2};
    LazySequence<int> first(first_items, 1);
    LazySequence<int> second(second_items, 1);

    EXPECT_THROW(first.concat(static_cast<const Sequence<int> *>(&second)), std::logic_error);
}

TEST(LazySequenceTest, ReduceIsNotSupported) {
    int items[] = {1, 2};
    LazySequence<int> sequence(items, 2);

    EXPECT_THROW(sequence.reduce(sum_items, 0), std::logic_error);
}

TEST(LazySequenceTest, SliceIsNotSupported) {
    int items[] = {1, 2, 3};
    LazySequence<int> sequence(items, 3);

    EXPECT_THROW(sequence.slice(0, 1), std::logic_error);
}

TEST(LazySequenceTest, EnumeratorIsNotSupported) {
    int items[] = {1, 2};
    LazySequence<int> sequence(items, 2);

    EXPECT_THROW(sequence.get_enumerator(), std::logic_error);
}
