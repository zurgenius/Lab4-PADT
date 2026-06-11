#include "array_sequence.h"
#include "streams/sequence_streams.h"
#include "tasks/event_statistics.h"

#include <gtest/gtest.h>

TEST(EventParserTest, ParseStartReturnsStartEvent) {
    Event<double> event = EventParser<double>::parse_line("START app");

    EXPECT_EQ(event.type, EventType::Start);
}

TEST(EventParserTest, ParseEndReturnsEndEvent) {
    Event<double> event = EventParser<double>::parse_line("END app");

    EXPECT_EQ(event.type, EventType::End);
}

TEST(EventParserTest, ParseMeasureReturnsMeasureEvent) {
    Event<double> event = EventParser<double>::parse_line("MEASURE 12.5");

    EXPECT_EQ(event.type, EventType::Measure);
}

TEST(EventParserTest, ParseMeasureStoresValue) {
    Event<double> event = EventParser<double>::parse_line("MEASURE 12.5");

    EXPECT_DOUBLE_EQ(event.value, 12.5);
}

TEST(EventParserTest, ParseErrorReturnsErrorEvent) {
    Event<double> event = EventParser<double>::parse_line("ERROR failed");

    EXPECT_EQ(event.type, EventType::Error);
}

TEST(EventParserTest, ParseUnknownReturnsUnknownEvent) {
    Event<double> event = EventParser<double>::parse_line("NOISE data");

    EXPECT_EQ(event.type, EventType::Unknown);
}

TEST(OnlineEventStatisticsTest, AddEventCountsTotal) {
    OnlineEventStatistics<double> statistics;

    statistics.add_event(Event<double>(EventType::Start, 0.0, ""));

    EXPECT_EQ(statistics.get_total_events(), 1);
}

TEST(OnlineEventStatisticsTest, AddEventCountsMeasure) {
    OnlineEventStatistics<double> statistics;

    statistics.add_event(Event<double>(EventType::Measure, 3.0, ""));

    EXPECT_EQ(statistics.get_measure_events(), 1);
}

TEST(OnlineEventStatisticsTest, GetMinMeasureReturnsMinimum) {
    OnlineEventStatistics<double> statistics;
    statistics.add_event(Event<double>(EventType::Measure, 3.0, ""));
    statistics.add_event(Event<double>(EventType::Measure, 2.0, ""));

    EXPECT_DOUBLE_EQ(statistics.get_min_measure(), 2.0);
}

TEST(OnlineEventStatisticsTest, GetMaxMeasureReturnsMaximum) {
    OnlineEventStatistics<double> statistics;
    statistics.add_event(Event<double>(EventType::Measure, 3.0, ""));
    statistics.add_event(Event<double>(EventType::Measure, 5.0, ""));

    EXPECT_DOUBLE_EQ(statistics.get_max_measure(), 5.0);
}

TEST(OnlineEventStatisticsTest, GetAverageMeasureReturnsAverage) {
    OnlineEventStatistics<double> statistics;
    statistics.add_event(Event<double>(EventType::Measure, 2.0, ""));
    statistics.add_event(Event<double>(EventType::Measure, 4.0, ""));

    EXPECT_DOUBLE_EQ(statistics.get_average_measure(), 3.0);
}

TEST(OnlineEventStatisticsTest, GetVarianceMeasureReturnsPopulationVariance) {
    OnlineEventStatistics<double> statistics;
    statistics.add_event(Event<double>(EventType::Measure, 2.0, ""));
    statistics.add_event(Event<double>(EventType::Measure, 4.0, ""));

    EXPECT_DOUBLE_EQ(statistics.get_variance_measure(), 1.0);
}

TEST(ProtocolStatisticsTaskTest, ProcessCountsStringProtocolEvents) {
    std::string lines[] = {"START app", "MEASURE 2", "ERROR fail", "END app"};
    MutableArraySequence<std::string> sequence(lines, 4);
    SequenceReadOnlyStream<std::string> stream(&sequence);

    OnlineEventStatistics<double> statistics = ProtocolStatisticsTask<double>::process(stream);

    EXPECT_EQ(statistics.get_total_events(), 4);
}

TEST(EventBatchProcessingTaskTest, ProcessRejectsZeroBatchSize) {
    std::string lines[] = {"START app"};
    MutableArraySequence<std::string> sequence(lines, 1);
    SequenceReadOnlyStream<std::string> stream(&sequence);

    EXPECT_THROW(EventBatchProcessingTask<double>::process(stream, 0), std::invalid_argument);
}

TEST(EventBatchProcessingTaskTest, ProcessCountsEventsAcrossBatches) {
    std::string lines[] = {"START app", "MEASURE 2", "MEASURE 4", "END app"};
    MutableArraySequence<std::string> sequence(lines, 4);
    SequenceReadOnlyStream<std::string> stream(&sequence);

    OnlineEventStatistics<double> statistics = EventBatchProcessingTask<double>::process(stream, 2);

    EXPECT_EQ(statistics.get_measure_events(), 2);
}
