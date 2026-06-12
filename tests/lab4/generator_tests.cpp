#include "lazy/lazy_sequence.h"

#include <gtest/gtest.h>

int generator_increment_rule(const Sequence<int> &source) {
    return source.get_last() + 1;
}

int generator_plus_one(const int &item) {
    return item + 1;
}

int generator_double_value(const int &item) {
    return item * 2;
}

bool generator_is_even(const int &item) {
    return item % 2 == 0;
}

bool generator_is_greater_than_three(const int &item) {
    return item > 3;
}

TEST(GeneratorScenarioTest, AppendGeneratorAddsFiniteTailToOmegaLength) {
    int data[] = {0};
    MutableArraySequence<int> initial(data, 1);
    LazySequence<int> sequence(generator_increment_rule, initial);

    LazySequence<int> *result = sequence.append(5);

    EXPECT_EQ(result->get_length().get_finite_count(), 1);
    delete result;
}

TEST(GeneratorScenarioTest, AppendGeneratorCanBeAppliedTwice) {
    int items[] = {1};
    LazySequence<int> sequence(items, 1);
    LazySequence<int> *first = sequence.append(2);
    LazySequence<int> *second = first->append(3);
    delete first;

    EXPECT_EQ(second->get(2), 3);
    delete second;
}

TEST(GeneratorScenarioTest, PrependGeneratorCanBeAppliedTwice) {
    int items[] = {3};
    LazySequence<int> sequence(items, 1);
    LazySequence<int> *first = sequence.prepend(2);
    LazySequence<int> *second = first->prepend(1);
    delete first;

    EXPECT_EQ(second->get(0), 1);
    delete second;
}

TEST(GeneratorScenarioTest, ConcatGeneratorAddsOmegaLengths) {
    int first_data[] = {0};
    int second_data[] = {10};
    MutableArraySequence<int> first_initial(first_data, 1);
    MutableArraySequence<int> second_initial(second_data, 1);
    LazySequence<int> first(generator_increment_rule, first_initial);
    LazySequence<int> second(generator_increment_rule, second_initial);

    LazySequence<int> *result = first.concat(second);

    EXPECT_EQ(result->get_length().get_omega_count(), 2);
    delete result;
}

TEST(GeneratorScenarioTest, ConcatGeneratorCanBeAppliedTwice) {
    int first_items[] = {1};
    int second_items[] = {2};
    int third_items[] = {3};
    LazySequence<int> first(first_items, 1);
    LazySequence<int> second(second_items, 1);
    LazySequence<int> third(third_items, 1);
    LazySequence<int> *joined = first.concat(second);
    LazySequence<int> *result = joined->concat(third);
    delete joined;

    EXPECT_EQ(result->get(2), 3);
    delete result;
}

TEST(GeneratorScenarioTest, InsertSequenceGeneratorInsertsFiniteSequence) {
    int source_items[] = {1, 4};
    int inserted_items[] = {2, 3};
    LazySequence<int> source(source_items, 2);
    LazySequence<int> inserted(inserted_items, 2);

    LazySequence<int> *result = source.insert_sequence_at(inserted, OrdinalIndex::finite(1));

    EXPECT_EQ(result->get(2), 3);
    delete result;
}

TEST(GeneratorScenarioTest, InsertSequenceGeneratorCanBeAppliedTwice) {
    int source_items[] = {1, 6};
    int first_insert_items[] = {2, 3};
    int second_insert_items[] = {4, 5};
    LazySequence<int> source(source_items, 2);
    LazySequence<int> first_insert(first_insert_items, 2);
    LazySequence<int> second_insert(second_insert_items, 2);
    LazySequence<int> *first = source.insert_sequence_at(first_insert, OrdinalIndex::finite(1));
    LazySequence<int> *second = first->insert_sequence_at(second_insert, OrdinalIndex::finite(3));
    delete first;

    EXPECT_EQ(second->get(4), 5);
    delete second;
}

TEST(GeneratorScenarioTest, InsertItemGeneratorKeepsOmegaLengthForFiniteInsertion) {
    int data[] = {0};
    MutableArraySequence<int> initial(data, 1);
    LazySequence<int> sequence(generator_increment_rule, initial);

    LazySequence<int> *result = sequence.insert_at(100, 1);

    EXPECT_EQ(result->get_length().get_finite_count(), 0);
    delete result;
}

TEST(GeneratorScenarioTest, InsertItemGeneratorCanBeAppliedTwice) {
    int items[] = {1, 4};
    LazySequence<int> sequence(items, 2);
    LazySequence<int> *first = sequence.insert_at(2, 1);
    LazySequence<int> *second = first->insert_at(3, 2);
    delete first;

    EXPECT_EQ(second->get(2), 3);
    delete second;
}

TEST(GeneratorScenarioTest, InterleaveGeneratorDoesNotMaterializeSourceBeyondRequestedRound) {
    int first_data[] = {0};
    int second_data[] = {10};
    int third_data[] = {20};
    MutableArraySequence<int> first_initial(first_data, 1);
    MutableArraySequence<int> second_initial(second_data, 1);
    MutableArraySequence<int> third_initial(third_data, 1);
    LazySequence<int> first(generator_increment_rule, first_initial);
    LazySequence<int> second(generator_increment_rule, second_initial);
    LazySequence<int> third(generator_increment_rule, third_initial);
    const LazySequence<int> *sources[] = {&first, &second, &third};
    LazySequence<int> *result = LazySequence<int>::interleave(sources, 3);

    result->get(5);

    EXPECT_EQ(result->get_materialized_count(), 6);
    delete result;
}

TEST(GeneratorScenarioTest, InterleaveGeneratorCanBeAppliedTwice) {
    int first_items[] = {1, 3};
    int second_items[] = {2, 4};
    int third_items[] = {10, 20};
    LazySequence<int> first(first_items, 2);
    LazySequence<int> second(second_items, 2);
    LazySequence<int> third(third_items, 2);
    const LazySequence<int> *first_sources[] = {&first, &second};
    LazySequence<int> *first_result = LazySequence<int>::interleave(first_sources, 2);
    const LazySequence<int> *second_sources[] = {first_result, &third};
    LazySequence<int> *second_result = LazySequence<int>::interleave(second_sources, 2);
    delete first_result;

    EXPECT_EQ(second_result->get(2), 2);
    delete second_result;
}

TEST(GeneratorScenarioTest, MapGeneratorCanBeAppliedTwice) {
    int items[] = {2};
    LazySequence<int> sequence(items, 1);
    LazySequence<int> *first = sequence.map(generator_double_value);
    LazySequence<int> *second = first->map(generator_plus_one);
    delete first;

    EXPECT_EQ(second->get(0), 5);
    delete second;
}

TEST(GeneratorScenarioTest, WhereGeneratorCanBeAppliedTwice) {
    int items[] = {1, 2, 3, 4, 6};
    LazySequence<int> sequence(items, 5);
    LazySequence<int> *first = sequence.where(generator_is_even);
    LazySequence<int> *second = first->where(generator_is_greater_than_three);
    delete first;

    EXPECT_EQ(second->get(0), 4);
    delete second;
}

TEST(GeneratorScenarioTest, TakeGeneratorCanBeAppliedTwice) {
    int data[] = {0};
    MutableArraySequence<int> initial(data, 1);
    LazySequence<int> sequence(generator_increment_rule, initial);
    LazySequence<int> *first = sequence.take(5);
    LazySequence<int> *second = first->take(3);
    delete first;

    EXPECT_EQ(second->get_count(), 3);
    delete second;
}

TEST(GeneratorScenarioTest, ChainedOperationsComputeAfterDeferredMaterialization) {
    int items[] = {1, 2};
    LazySequence<int> sequence(items, 2);
    LazySequence<int> *appended = sequence.append(3);
    LazySequence<int> *mapped = appended->map(generator_plus_one);
    LazySequence<int> *prepended = mapped->prepend(0);
    delete appended;
    delete mapped;

    EXPECT_EQ(prepended->get(3), 4);
    delete prepended;
}
