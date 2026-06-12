#pragma once

#include "streams/base.h"

#include <fstream>
#include <string>

// Читает файл, который может дописываться другим процессом во время чтения.
class LiveFileLineReadOnlyStream : public ReadOnlyStream<std::string> {
  private:
    std::string filename;
    std::ifstream file;
    int position;
    bool opened;
    bool ended;
    int empty_polls;
    int max_empty_polls;
    int wait_ms;

  public:
    LiveFileLineReadOnlyStream(const std::string &filename, int max_empty_polls, int wait_ms);

    bool is_end_of_stream() const override;
    std::string read() override;
    Option<std::string> try_read() override;
    void open() override;
    void close() override;
};

#include "streams/detail/live_file.tpp"
