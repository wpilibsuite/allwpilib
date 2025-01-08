// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/DataLogWriter.h"

#include <memory>
#include <utility>
#include <vector>

#include "wpi/raw_ostream.h"

using namespace wpi::log;

static std::unique_ptr<wpi::raw_ostream> CheckOpen(std::string_view filename,
                                                   std::error_code& ec) {
  auto rv = std::make_unique<wpi::raw_fd_ostream>(filename, ec);
  if (ec) {
    return nullptr;
  }
  return rv;
}

DataLogWriter::DataLogWriter(std::string_view filename, std::error_code& ec,
                             std::string_view extraHeader)
    : DataLogWriter{s_defaultMessageLog, filename, ec, extraHeader} {}

DataLogWriter::DataLogWriter(wpi::Logger& msglog, std::string_view filename,
                             std::error_code& ec, std::string_view extraHeader)
    : DataLogWriter{msglog, CheckOpen(filename, ec), extraHeader} {
  if (ec) {
    Stop();
  }
}

DataLogWriter::DataLogWriter(std::unique_ptr<wpi::raw_ostream> os,
                             std::string_view extraHeader)
    : DataLogWriter{s_defaultMessageLog, std::move(os), extraHeader} {}

DataLogWriter::DataLogWriter(wpi::Logger& msglog,
                             std::unique_ptr<wpi::raw_ostream> os,
                             std::string_view extraHeader)
    : DataLog{msglog, extraHeader}, m_os{std::move(os)} {
  StartFile();
}

DataLogWriter::~DataLogWriter() {
  if (m_os) {
    Flush();
  }
}

void DataLogWriter::Flush() {
  if (!m_os) {
    return;
  }
  std::vector<Buffer> writeBufs;
  FlushBufs(&writeBufs);
  for (auto&& buf : writeBufs) {
    (*m_os) << buf.GetData();
  }
  ReleaseBufs(&writeBufs);
}

void DataLogWriter::Stop() {
  DataLog::Stop();
  Flush();
  m_os.reset();
}

bool DataLogWriter::BufferFull() {
  Flush();
  return false;
}

extern "C" {

struct WPI_DataLog* WPI_DataLog_CreateWriter(
    const struct WPI_String* filename, int* errorCode,
    const struct WPI_String* extraHeader) {
  std::error_code ec;
  auto rv = reinterpret_cast<WPI_DataLog*>(new DataLogWriter{
      wpi::to_string_view(filename), ec, wpi::to_string_view(extraHeader)});
  *errorCode = ec.value();
  return rv;
}

}  // extern "C"
