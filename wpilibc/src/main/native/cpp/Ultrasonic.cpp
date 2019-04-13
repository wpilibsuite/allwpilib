/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/Ultrasonic.h"

#include <hal/HAL.h>

#include "frc/Counter.h"
#include "frc/DigitalInput.h"
#include "frc/DigitalOutput.h"
#include "frc/Timer.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

// Automatic round robin mode
std::atomic<bool> Ultrasonic::m_automaticEnabled{false};

std::vector<Ultrasonic*> Ultrasonic::m_sensors;
std::thread Ultrasonic::m_thread;

Ultrasonic::Ultrasonic(int pingChannel, int echoChannel, DistanceUnit units)
    : m_pingChannel(std::make_shared<DigitalOutput>(pingChannel)),
      m_echoChannel(std::make_shared<DigitalInput>(echoChannel)),
      m_counter(m_echoChannel) {
  m_units = units;
  Initialize();
  AddChild(m_pingChannel);
  AddChild(m_echoChannel);
}

Ultrasonic::Ultrasonic(DigitalOutput* pingChannel, DigitalInput* echoChannel,
                       DistanceUnit units)
    : m_pingChannel(pingChannel, NullDeleter<DigitalOutput>()),
      m_echoChannel(echoChannel, NullDeleter<DigitalInput>()),
      m_counter(m_echoChannel) {
  if (pingChannel == nullptr || echoChannel == nullptr) {
    wpi_setWPIError(NullParameter);
    m_units = units;
    return;
  }
  m_units = units;
  Initialize();
}

Ultrasonic::Ultrasonic(DigitalOutput& pingChannel, DigitalInput& echoChannel,
                       DistanceUnit units)
    : m_pingChannel(&pingChannel, NullDeleter<DigitalOutput>()),
      m_echoChannel(&echoChannel, NullDeleter<DigitalInput>()),
      m_counter(m_echoChannel) {
  m_units = units;
  Initialize();
}

Ultrasonic::Ultrasonic(std::shared_ptr<DigitalOutput> pingChannel,
                       std::shared_ptr<DigitalInput> echoChannel,
                       DistanceUnit units)
    : m_pingChannel(pingChannel),
      m_echoChannel(echoChannel),
      m_counter(m_echoChannel) {
  m_units = units;
  Initialize();
}

Ultrasonic::~Ultrasonic() {
  // Delete the instance of the ultrasonic sensor by freeing the allocated
  // digital channels. If the system was in automatic mode (round robin), then
  // it is stopped, then started again after this sensor is removed (provided
  // this wasn't the last sensor).

  bool wasAutomaticMode = m_automaticEnabled;
  SetAutomaticMode(false);

  // No synchronization needed because the background task is stopped.
  m_sensors.erase(std::remove(m_sensors.begin(), m_sensors.end(), this),
                  m_sensors.end());

  if (!m_sensors.empty() && wasAutomaticMode) {
    SetAutomaticMode(true);
  }
}

void Ultrasonic::Ping() {
  wpi_assert(!m_automaticEnabled);

  // Reset the counter to zero (invalid data now)
  m_counter.Reset();

  // Do the ping to start getting a single range
  m_pingChannel->Pulse(kPingTime);
}

bool Ultrasonic::IsRangeValid() const { return m_counter.Get() > 1; }

void Ultrasonic::SetAutomaticMode(bool enabling) {
  if (enabling == m_automaticEnabled) return;  // ignore the case of no change

  m_automaticEnabled = enabling;

  if (enabling) {
    /* Clear all the counters so no data is valid. No synchronization is needed
     * because the background task is stopped.
     */
    for (auto& sensor : m_sensors) {
      sensor->m_counter.Reset();
    }

    m_thread = std::thread(&Ultrasonic::UltrasonicChecker);

    // TODO: Currently, lvuser does not have permissions to set task priorities.
    // Until that is the case, uncommenting this will break user code that calls
    // Ultrasonic::SetAutomicMode().
    // m_task.SetPriority(kPriority);
  } else {
    // Wait for background task to stop running
    if (m_thread.joinable()) {
      m_thread.join();
    }

    // Clear all the counters (data now invalid) since automatic mode is
    // disabled. No synchronization is needed because the background task is
    // stopped.
    for (auto& sensor : m_sensors) {
      sensor->m_counter.Reset();
    }
  }
}

double Ultrasonic::GetRangeInches() const {
  if (IsRangeValid())
    return m_counter.GetPeriod() * kSpeedOfSoundInchesPerSec / 2.0;
  else
    return 0;
}

double Ultrasonic::GetRangeMM() const { return GetRangeInches() * 25.4; }

bool Ultrasonic::IsEnabled() const { return m_enabled; }

void Ultrasonic::SetEnabled(bool enable) { m_enabled = enable; }

void Ultrasonic::SetDistanceUnits(DistanceUnit units) { m_units = units; }

Ultrasonic::DistanceUnit Ultrasonic::GetDistanceUnits() const {
  return m_units;
}

double Ultrasonic::PIDGet() {
  switch (m_units) {
    case Ultrasonic::kInches:
      return GetRangeInches();
    case Ultrasonic::kMilliMeters:
      return GetRangeMM();
    default:
      return 0.0;
  }
}

void Ultrasonic::SetPIDSourceType(PIDSourceType pidSource) {
  if (wpi_assert(pidSource == PIDSourceType::kDisplacement)) {
    m_pidSource = pidSource;
  }
}

void Ultrasonic::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Ultrasonic");
  builder.AddDoubleProperty("Value", [=]() { return GetRangeInches(); },
                            nullptr);
}

void Ultrasonic::Initialize() {
  bool originalMode = m_automaticEnabled;
  SetAutomaticMode(false);  // Kill task when adding a new sensor
  // Link this instance on the list
  m_sensors.emplace_back(this);

  m_counter.SetMaxPeriod(1.0);
  m_counter.SetSemiPeriodMode(true);
  m_counter.Reset();
  m_enabled = true;  // Make it available for round robin scheduling
  SetAutomaticMode(originalMode);

  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_Ultrasonic, instances);
  SetName("Ultrasonic", m_echoChannel->GetChannel());
}

void Ultrasonic::UltrasonicChecker() {
  while (m_automaticEnabled) {
    for (auto& sensor : m_sensors) {
      if (!m_automaticEnabled) break;

      if (sensor->IsEnabled()) {
        sensor->m_pingChannel->Pulse(kPingTime);  // do the ping
      }

      Wait(0.1);  // wait for ping to return
    }
  }
}
