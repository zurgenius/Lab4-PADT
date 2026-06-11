#pragma once

#include "streams/base_streams.h"

#include <fstream>
#include <string>

class FileLineReadOnlyStream : public ReadOnlyStream<std::string> {
  private:
    std::string filename;
    std::ifstream file;
    int position;
    bool opened;
    bool ended;

  public:
    explicit FileLineReadOnlyStream(const std::string &filename);

    bool is_end_of_stream() const override;
    std::string read() override;
    Option<std::string> try_read() override;
    int get_position() const override;
    bool can_seek() const override;
    int seek(int index) override;
    bool can_go_back() const override;
    void open() override;
    void close() override;
};

class FileLineWriteOnlyStream : public WriteOnlyStream<std::string> {
  private:
    std::string filename;
    std::ofstream file;
    int position;
    bool opened;

  public:
    explicit FileLineWriteOnlyStream(const std::string &filename);

    int write(const std::string &item) override;
    int get_position() const override;
    void open() override;
    void close() override;
};

#include "streams/detail/file_streams.tpp"
