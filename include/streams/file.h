#pragma once

#include "streams/base.h"

#include <fstream>
#include <string>

// Читает уже существующий файл построчно до обычного EOF.
class FileLineReadOnlyStream : public ReadOnlyStream<std::string> {
  private:
    std::string filename;
    std::ifstream file;
    int position;
    bool opened;
    bool ended;

  public:
    FileLineReadOnlyStream(const std::string &filename);

    bool is_end_of_stream() const override;
    std::string read() override;
    Option<std::string> try_read() override;
    void open() override;
    void close() override;
};

// Записывает строки в файл с немедленным flush после каждой записи.
class FileLineWriteOnlyStream : public WriteOnlyStream<std::string> {
  private:
    std::string filename;
    std::ofstream file;
    int position;
    bool opened;

  public:
    FileLineWriteOnlyStream(const std::string &filename);

    int write(const std::string &item) override;
    void open() override;
    void close() override;
};

#include "streams/detail/file.tpp"
