// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Tracer.h"

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/Errors.h"

using namespace frc;

Tracer::Tracer() {
  ResetTimer();
}

void Tracer::PublishToNetworkTables(std::string_view topicName) {
  if (m_publishNT) {
    return;
  }
  m_publishNT = true;
  m_inst = nt::NetworkTableInstance::GetDefault();
  m_ntTopic = topicName;
  m_publisherCache = wpi::StringMap<std::shared_ptr<nt::IntegerPublisher>>(10);
}

void Tracer::StartDataLog(wpi::log::DataLog& dataLog, std::string_view entry) {
  if (m_dataLogEnabled) {
    return;
  }
  m_dataLogEnabled = true;
  m_dataLog = &dataLog;
  m_dataLogEntry = entry;
  m_entryCache = wpi::StringMap<int>(10);
}

void Tracer::ResetTimer() {
  m_startTime = hal::fpga_clock::now();
}

void Tracer::ClearEpochs() {
  ResetTimer();
  m_epochs.clear();
}

void Tracer::AddEpoch(std::string_view epochName) {
  auto currentTime = hal::fpga_clock::now();
  auto epoch = currentTime - m_startTime;
  m_epochs[epochName] = epoch;
  if (m_publishNT) {
    // Topics are cached with the epoch name as the key, and the publisher
    // object as the value
    if (m_publisherCache.count(epochName) == 0) {
      // Create and prep the epoch publisher
      auto topic =
          m_inst.GetIntegerTopic(fmt::format("{}/{}", m_ntTopic, epochName));
      auto pub = std::make_shared<nt::IntegerPublisher>(topic.Publish());
      m_publisherCache.insert(std::pair(epochName, pub));
    }
    m_publisherCache.lookup(epochName)->Set(epoch.count());
  }
  if (m_dataLogEnabled) {
    // Epochs are cached with the epoch name as the key, and the entry index as
    // the value
    if (m_entryCache.count(epochName) == 0) {
      // Start a data log entry
      int entryIndex = m_dataLog->Start(
          fmt::format("{}/{}", m_dataLogEntry, epochName), "int64");
      // Cache the entry index with the epoch name as the key
      m_entryCache.insert(std::pair(epochName, entryIndex));
    }
    m_dataLog->AppendInteger(m_entryCache.lookup(epochName), epoch.count(), 0);
  }
  m_startTime = currentTime;
}

void Tracer::PrintEpochs() {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream os(buf);
  PrintEpochs(os);
  if (!buf.empty()) {
    FRC_ReportError(warn::Warning, "{}", buf.c_str());
  }
}

void Tracer::PrintEpochs(wpi::raw_ostream& os) {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;

  auto now = hal::fpga_clock::now();
  if (now - m_lastEpochsPrintTime > kMinPrintPeriod) {
    m_lastEpochsPrintTime = now;
    for (const auto& epoch : m_epochs) {
      os << fmt::format(
          "\t{}: {:.6f}s\n", epoch.getKey(),
          duration_cast<microseconds>(epoch.getValue()).count() / 1.0e6);
    }
  }
}
