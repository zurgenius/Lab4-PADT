#include "lazy/lazy_sequence.h"

#include <gtest/gtest.h>

int generator_increment_rule(const Sequence<int> &source) {
    return source.get_last() + 1;
}

TEST(GeneratorScenarioTest, AppendGeneratorAddsFiniteTailToOmegaLength) {
    int data[] = {0};
    MutableArraySequence<int> initial(data, 1);
    LazySequence<int> sequence(generator_increment_rule, initial);

    LazySequence<int> *result = sequence.append(5);

    EXPECT_EQ(result->get_length().get_finite_count(), 1);
    delete result;
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

TEST(GeneratorScenarioTest, InsertSequenceGeneratorInsertsFiniteSequence) {
    int source_items[] = {1, 4};
    int inserted_items[] = {2, 3};
    LazySequence<int> source(source_items, 2);
    LazySequence<int> inserted(inserted_items, 2);

    LazySequence<int> *result = source.insert_sequence_at(inserted, OrdinalIndex::finite(1));

    EXPECT_EQ(result->get(2), 3);
    delete result;
}

TEST(GeneratorScenarioTest, InsertItemGeneratorKeepsOmegaLengthForFiniteInsertion) {
    int data[] = {0};
    MutableArraySequence<int> initial(data, 1);
    LazySequence<int> sequence(generator_increment_rule, initial);

    LazySequence<int> *result = sequence.insert_at(100, 1);

    EXPECT_EQ(result->get_length().get_finite_count(), 0);
    delete result;
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
