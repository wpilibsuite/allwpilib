/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Tracer.h"

#include <wpi/Format.h>
#include <wpi/SmallString.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"

using namespace frc;

Tracer::Tracer() { ResetTimer(); }

void Tracer::ResetTimer() { m_startTime = hal::fpga_clock::now(); }

void Tracer::ClearEpochs() {
  ResetTimer();
  m_epochs.clear();
}

void Tracer::AddEpoch(wpi::StringRef epochName) {
  auto currentTime = hal::fpga_clock::now();
  m_epochs[epochName] = currentTime - m_startTime;
  m_startTime = currentTime;
}

void Tracer::PrintEpochs() {
  wpi::SmallString<128> buf;
  wpi::raw_svector_ostream os(buf);
  PrintEpochs(os);
  if (!buf.empty()) DriverStation::ReportWarning(buf);
}

void Tracer::PrintEpochs(wpi::raw_ostream& os) {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;

  auto now = hal::fpga_clock::now();
  if (now - m_lastEpochsPrintTime > kMinPrintPeriod) {
    m_lastEpochsPrintTime = now;
    for (const auto& epoch : m_epochs) {
      os << '\t' << epoch.getKey() << ": "
         << wpi::format(
                "%.6f",
                duration_cast<microseconds>(epoch.getValue()).count() / 1.0e6)
         << "s\n";
    }
  }
}
