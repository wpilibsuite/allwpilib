#include "frc/Tracer.h"

#include <wpi/raw_ostream.h>
#include <wpi/Format.h>

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
  using std::chrono::duration_cast;
  using std::chrono::microseconds;

  auto now = hal::fpga_clock::now();
  if (now - m_lastEpochsPrintTime > kMinPrintPeriod) {
    m_lastEpochsPrintTime = now;
    for (const auto& epoch : m_epochs) {
      wpi::outs() << '\t' << epoch.getKey() << ": "
                  << wpi::format(
                         "%.6f",
                         duration_cast<microseconds>(epoch.getValue()).count() /
                             1.0e6)
                  << "s\n";
    }
  }
}