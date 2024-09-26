// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Ultrasonic.h"

#include <memory>
#include <utility>
#include <vector>

#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Counter.h"
#include "frc/DigitalInput.h"
#include "frc/DigitalOutput.h"
#include "frc/Errors.h"
#include "frc/Timer.h"

using namespace frc;

// Automatic round robin mode
std::atomic<bool> Ultrasonic::m_automaticEnabled{false};

std::vector<Ultrasonic*> Ultrasonic::m_sensors;
std::thread Ultrasonic::m_thread;

Ultrasonic::Ultrasonic(int pingChannel, int echoChannel)
    : m_pingChannel(std::make_shared<DigitalOutput>(pingChannel)),
      m_echoChannel(std::make_shared<DigitalInput>(echoChannel)),
      m_counter(m_echoChannel) {
  Initialize();
  wpi::SendableRegistry::AddChild(this, m_pingChannel.get());
  wpi::SendableRegistry::AddChild(this, m_echoChannel.get());
}

Ultrasonic::Ultrasonic(DigitalOutput* pingChannel, DigitalInput* echoChannel)
    : m_pingChannel(pingChannel, wpi::NullDeleter<DigitalOutput>()),
      m_echoChannel(echoChannel, wpi::NullDeleter<DigitalInput>()),
      m_counter(m_echoChannel) {
  if (!pingChannel) {
    throw FRC_MakeError(err::NullParameter, "pingChannel");
  }
  if (!echoChannel) {
    throw FRC_MakeError(err::NullParameter, "echoChannel");
  }
  Initialize();
}

Ultrasonic::Ultrasonic(DigitalOutput& pingChannel, DigitalInput& echoChannel)
    : m_pingChannel(&pingChannel, wpi::NullDeleter<DigitalOutput>()),
      m_echoChannel(&echoChannel, wpi::NullDeleter<DigitalInput>()),
      m_counter(m_echoChannel) {
  Initialize();
}

Ultrasonic::Ultrasonic(std::shared_ptr<DigitalOutput> pingChannel,
                       std::shared_ptr<DigitalInput> echoChannel)
    : m_pingChannel(std::move(pingChannel)),
      m_echoChannel(std::move(echoChannel)),
      m_counter(m_echoChannel) {
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

int Ultrasonic::GetEchoChannel() const {
  return m_echoChannel->GetChannel();
}

void Ultrasonic::Ping() {
  SetAutomaticMode(false);  // turn off automatic round-robin if pinging

  // Reset the counter to zero (invalid data now)
  m_counter.Reset();

  // Do the ping to start getting a single range
  m_pingChannel->Pulse(kPingTime);
}

bool Ultrasonic::IsRangeValid() const {
  if (m_simRangeValid) {
    return m_simRangeValid.Get();
  }
  return m_counter.Get() > 1;
}

void Ultrasonic::SetAutomaticMode(bool enabling) {
  if (enabling == m_automaticEnabled) {
    return;  // ignore the case of no change
  }

  m_automaticEnabled = enabling;

  if (enabling) {
    /* Clear all the counters so no data is valid. No synchronization is needed
     * because the background task is stopped.
     */
    for (auto& sensor : m_sensors) {
      sensor->m_counter.Reset();
    }

    m_thread = std::thread(&Ultrasonic::UltrasonicChecker);
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

units::meter_t Ultrasonic::GetRange() const {
  if (IsRangeValid()) {
    if (m_simRange) {
      return units::inch_t{m_simRange.Get()};
    }
    return m_counter.GetPeriod() * kSpeedOfSound / 2.0;
  } else {
    return 0_m;
  }
}

bool Ultrasonic::IsEnabled() const {
  return m_enabled;
}

void Ultrasonic::SetEnabled(bool enable) {
  m_enabled = enable;
}

void Ultrasonic::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Ultrasonic");
  builder.AddDoubleProperty(
      "Value", [=, this] { return units::inch_t{GetRange()}.value(); },
      nullptr);
}

void Ultrasonic::Initialize() {
  m_simDevice = hal::SimDevice("Ultrasonic", m_echoChannel->GetChannel());
  if (m_simDevice) {
    m_simRangeValid = m_simDevice.CreateBoolean("Range Valid", false, true);
    m_simRange = m_simDevice.CreateDouble("Range (in)", false, 0.0);
    m_pingChannel->SetSimDevice(m_simDevice);
    m_echoChannel->SetSimDevice(m_simDevice);
  }

  bool originalMode = m_automaticEnabled;
  SetAutomaticMode(false);  // Kill task when adding a new sensor
  // Link this instance on the list
  m_sensors.emplace_back(this);

  m_counter.SetMaxPeriod(1_s);
  m_counter.SetSemiPeriodMode(true);
  m_counter.Reset();
  m_enabled = true;  // Make it available for round robin scheduling
  SetAutomaticMode(originalMode);

  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_Ultrasonic, instances);
  wpi::SendableRegistry::AddLW(this, "Ultrasonic", m_echoChannel->GetChannel());
}

void Ultrasonic::UltrasonicChecker() {
  while (m_automaticEnabled) {
    for (auto& sensor : m_sensors) {
      if (!m_automaticEnabled) {
        break;
      }

      if (sensor->IsEnabled()) {
        sensor->m_pingChannel->Pulse(kPingTime);  // do the ping
      }

      Wait(100_ms);  // wait for ping to return
    }
  }
}
