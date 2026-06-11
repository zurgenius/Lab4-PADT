#include "ordinal.h"

#include <gtest/gtest.h>

TEST(OrdinalIndexTest, FiniteFactoryCreatesFiniteIndex) {
    OrdinalIndex index = OrdinalIndex::finite(3);

    EXPECT_TRUE(index.is_finite());
    EXPECT_EQ(index.get_finite_index(), 3);
}

TEST(OrdinalIndexTest, OmegaTimesRejectsZeroOmegaCount) {
    EXPECT_THROW(OrdinalIndex::omega_times(0, 1), std::out_of_range);
}

TEST(OrdinalLengthTest, FiniteLengthContainsFiniteIndexInside) {
    OrdinalLength length = OrdinalLength::finite(4);

    EXPECT_TRUE(length.contains(OrdinalIndex::finite(3)));
}

TEST(OrdinalLengthTest, FiniteLengthRejectsFiniteIndexOutside) {
    OrdinalLength length = OrdinalLength::finite(4);

    EXPECT_FALSE(length.contains(OrdinalIndex::finite(4)));
}

TEST(OrdinalLengthTest, OmegaLengthContainsFiniteIndex) {
    OrdinalLength length = OrdinalLength::omega();

    EXPECT_TRUE(length.contains(OrdinalIndex::finite(1000)));
}

TEST(OrdinalLengthTest, OmegaLengthRejectsOmegaIndexWithoutTail) {
    OrdinalLength length = OrdinalLength::omega();

    EXPECT_FALSE(length.contains(OrdinalIndex::omega_plus(0)));
}

TEST(OrdinalLengthTest, AddFiniteToOmegaPreservesOmegaTail) {
    OrdinalLength result = OrdinalLength::add(OrdinalLength::omega(), OrdinalLength::finite(5));

    EXPECT_EQ(result.get_omega_count(), 1);
    EXPECT_EQ(result.get_finite_count(), 5);
}

TEST(OrdinalLengthTest, AddOmegaToOmegaProducesTwoOmegas) {
    OrdinalLength result = OrdinalLength::add(OrdinalLength::omega(), OrdinalLength::omega());

    EXPECT_EQ(result.get_omega_count(), 2);
}

TEST(OrdinalLengthTest, SubtractPrefixAfterOmegaReturnsFiniteIndex) {
    OrdinalLength prefix = OrdinalLength::omega();

    OrdinalIndex result = prefix.subtract_prefix(OrdinalIndex::omega_plus(7));

    EXPECT_TRUE(result.is_finite());
    EXPECT_EQ(result.get_finite_index(), 7);
}
