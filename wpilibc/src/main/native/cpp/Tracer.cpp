// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Tracer.h"

#include <fmt/format.h>
#include <networktables/IntegerTopic.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/Errors.h"

using namespace frc;

Tracer::Tracer() {
  ResetTimer();
}

void Tracer::PublishToNetworkTables(std::string_view topicName) {
  m_publishNT = true;
  m_inst = nt::NetworkTableInstance::GetDefault();
  m_ntTopic = topicName;
}

void Tracer::StartDataLog(wpi::log::DataLog dataLog, std::string_view entry) {
  m_dataLog = dataLog;
  m_dataLogEntry = entry;
  m_dataLogEnabled = true;
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
    auto topic =
        m_inst.GetIntegerTopic(m_ntTopic.data() + "/" + epochName.data());
    auto pub = topic.Publish();
    pub.SetDefault(0);
    topic.SetRetained(true);
    pub.Set(epoch.count());
  }
  if (m_dataLogEnabled) {
    m_dataLog.AppendInteger(
        m_dataLog.Start(m_dataLogEntry + "/" + epochName, "int64"),
        epoch.count(), 0);
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
