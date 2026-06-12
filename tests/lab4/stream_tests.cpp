#include "array_sequence.h"
#include "lazy/lazy_sequence.h"
#include "streams/file_streams.h"
#include "streams/lazy_streams.h"
#include "streams/live_file_streams.h"
#include "streams/sequence_streams.h"

#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>

int stream_natural_rule(const Sequence<int> &source) {
    return source.get_count();
}

TEST(SequenceReadOnlyStreamTest, ReadReturnsFirstSequenceItem) {
    int items[] = {7, 8};
    MutableArraySequence<int> sequence(items, 2);
    SequenceReadOnlyStream<int> stream(&sequence);
    stream.open();

    EXPECT_EQ(stream.read(), 7);
}

TEST(SequenceWriteOnlyStreamTest, WriteAppendsItem) {
    MutableArraySequence<int> sequence;
    SequenceWriteOnlyStream<int> stream(&sequence);
    stream.open();

    stream.write(42);

    EXPECT_EQ(sequence.get(0), 42);
}

TEST(LazyReadOnlyStreamTest, ReadMaterializesLazyItem) {
    int items[] = {0};
    MutableArraySequence<int> initial(items, 1);
    LazySequence<int> sequence(stream_natural_rule, initial);
    LazyReadOnlyStream<int> stream(&sequence);
    stream.open();

    EXPECT_EQ(stream.read(), 0);
}

TEST(FileLineReadOnlyStreamTest, TryReadReturnsLineFromFile) {
    const char *filename = "stream_test_protocol.txt";
    std::ofstream output(filename);
    output << "START app\n";
    output.close();
    FileLineReadOnlyStream stream(filename);
    stream.open();

    Option<std::string> line = stream.try_read();

    EXPECT_EQ(line.get_value(), "START app");
    std::remove(filename);
}

TEST(FileLineWriteOnlyStreamTest, WriteStoresLineInFile) {
    const char *filename = "stream_write_test_protocol.txt";
    FileLineWriteOnlyStream stream(filename);
    stream.open();
    stream.write("MEASURE 1");
    stream.close();
    std::ifstream input(filename);
    std::string line;
    std::getline(input, line);
    input.close();

    EXPECT_EQ(line, "MEASURE 1");
    std::remove(filename);
}

TEST(LiveFileLineReadOnlyStreamTest, TryReadReturnsNoneBeforeNewLineAppears) {
    const char *filename = "live_stream_empty_test.txt";
    std::ofstream output(filename);
    output.close();
    LiveFileLineReadOnlyStream stream(filename, 1, 0);
    stream.open();

    Option<std::string> line = stream.try_read();

    EXPECT_FALSE(line.has_value());
    std::remove(filename);
}

TEST(LiveFileLineReadOnlyStreamTest, TryReadReturnsAppendedLine) {
    const char *filename = "live_stream_append_test.txt";
    std::ofstream output(filename);
    output.close();
    LiveFileLineReadOnlyStream stream(filename, 3, 0);
    stream.open();
    std::ofstream append(filename, std::ios::app);
    append << "ERROR fail\n";
    append.close();

    Option<std::string> line = stream.try_read();

    EXPECT_EQ(line.get_value(), "ERROR fail");
    std::remove(filename);
}
