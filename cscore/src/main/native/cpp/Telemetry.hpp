// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_TELEMETRY_H_
#define CSCORE_TELEMETRY_H_

#include <wpi/SafeThread.h>

#include "cscore_cpp.h"

namespace cs {

class Notifier;
class SourceImpl;

class Telemetry {
  friend class TelemetryTest;

 public:
  explicit Telemetry(Notifier& notifier) : m_notifier(notifier) {}
  ~Telemetry();

  void Start();
  void Stop();

  // User interface
  void SetPeriod(double seconds);
  double GetElapsedTime();
  int64_t GetValue(CS_Handle handle, CS_TelemetryKind kind, CS_Status* status);
  double GetAverageValue(CS_Handle handle, CS_TelemetryKind kind,
                         CS_Status* status);

  // Telemetry events
  void RecordSourceBytes(const SourceImpl& source, int quantity);
  void RecordSourceFrames(const SourceImpl& source, int quantity);

 private:
  Notifier& m_notifier;

  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;
};

}  // namespace cs

#endif  // CSCORE_TELEMETRY_H_
