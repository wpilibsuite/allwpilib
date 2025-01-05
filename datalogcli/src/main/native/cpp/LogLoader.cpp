// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LogLoader.h"

#include <memory>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include "fmt/base.h"
#include "wpi/DataLogReader.h"

#include <wpi/DataLogReaderThread.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/fs.h>

using namespace datalogcli;

LogLoader::LogLoader() {}

datalogcli::LogLoader::LogLoader(const LogLoader& original) {
  m_reader = std::make_unique<wpi::DataLogReaderThread>(original.m_reader.get());
  m_filename = original.m_filename;
  m_error = original.m_error;
  m_entryTree = original.m_entryTree;
  m_filter = original.m_filter;
}

LogLoader::~LogLoader() = default;

void LogLoader::Load(std::string_view log_path) {

  // Handle opening the file
  std::error_code ec;
  auto buf = wpi::MemoryBuffer::GetFile(log_path);
  if (ec) {
    m_error = fmt::format("Could not open file: {}", ec.message());
    return;
  }

  wpi::log::DataLogReader reader{*std::move(buf)};
  if (!reader.IsValid()) {
    m_error = "Not a valid datalog file";
    return;
  }
  unload(); // release the actual file, we have the data in the reader now
  m_reader = std::make_unique<wpi::DataLogReaderThread>(std::move(reader));
  m_entryTree.clear();

  // Handle Errors
  fmt::println("{}", m_error);

  if (!m_reader) {
    return;
  }

  // Summary info
  fmt::println("{}", fs::path{m_filename}.stem().string().c_str());
  fmt::println("%u records, %u entries%s", m_reader->GetNumRecords(),
              m_reader->GetNumEntries(),
              m_reader->IsDone() ? "" : " (working)");

  if (!m_reader->IsDone()) {
    return;
  }
}

std::vector<wpi::log::DataLogRecord> LogLoader::GetRecords(std::string_view field_name) {
  std::vector<wpi::log::DataLogRecord> record_list{};

  const wpi::DataLogReaderEntry* entry = m_reader->GetEntry(field_name);
  for (wpi::DataLogReaderRange range : entry->ranges)
  {
    wpi::log::DataLogReader::iterator rangeReader = range.begin();
    while (!rangeReader->IsFinish())
    {
      record_list.push_back(*rangeReader);
    }
  }

  return record_list;
}



