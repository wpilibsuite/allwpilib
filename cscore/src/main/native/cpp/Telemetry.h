/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_TELEMETRY_H_
#define CSCORE_TELEMETRY_H_

#include <stdint.h>

#include <wpi/SafeThread.h>
#include <wpi/Signal.h>

#include "cscore_cpp.h"

namespace cs {

class Telemetry {
  friend class TelemetryTest;

 public:
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
  void Record(CS_Handle handle, CS_TelemetryKind kind, int64_t quantity);

  // Called when telemetry updated
  wpi::sig::Signal<> telemetryUpdated;

 private:
  class Thread;
  wpi::SafeThreadOwner<Thread> m_owner;
};

}  // namespace cs

#endif  // CSCORE_TELEMETRY_H_
