// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/Tracer.hpp"

#include <fmt/format.h>
#include <wpi/util/SmallString.hpp>
#include <wpi/util/raw_ostream.hpp>

#include "wpi/system/Errors.hpp"

using namespace wpi;

Tracer::Tracer() {
  ResetTimer();
}

void Tracer::ResetTimer() {
  m_startTime = wpi::hal::fpga_clock::now();
}

void Tracer::ClearEpochs() {
  ResetTimer();
  m_epochs.clear();
}

void Tracer::AddEpoch(std::string_view epochName) {
  auto currentTime = wpi::hal::fpga_clock::now();
  m_epochs[epochName] = currentTime - m_startTime;
  m_startTime = currentTime;
}

void Tracer::PrintEpochs() {
  wpi::util::SmallString<128> buf;
  wpi::util::raw_svector_ostream os(buf);
  PrintEpochs(os);
  if (!buf.empty()) {
    WPILIB_ReportWarning("{}", buf.c_str());
  }
}

void Tracer::PrintEpochs(wpi::util::raw_ostream& os) {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;

  auto now = wpi::hal::fpga_clock::now();
  if (now - m_lastEpochsPrintTime > kMinPrintPeriod) {
    m_lastEpochsPrintTime = now;
    for (const auto& epoch : m_epochs) {
      os << fmt::format(
          "\t{}: {:.6f}s\n", epoch.first,
          duration_cast<microseconds>(epoch.second).count() / 1.0e6);
    }
  }
}
