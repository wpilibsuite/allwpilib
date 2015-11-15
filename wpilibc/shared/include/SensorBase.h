/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.
 */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

#include "ErrorBase.h"
#include <stdio.h>
#include "Base.h"

/**
 * Base class for all sensors.
 * Stores most recent status information as well as containing utility functions
 * for checking
 * channels and error processing.
 */
class SensorBase : public ErrorBase {
 public:
  SensorBase();
  virtual ~SensorBase() = default;

  SensorBase(const SensorBase&) = delete;
  SensorBase& operator=(const SensorBase&) = delete;

  static void DeleteSingletons();

  static uint32_t GetDefaultSolenoidModule() { return 0; }

  static bool CheckSolenoidModule(uint8_t moduleNumber);
  static bool CheckDigitalChannel(uint32_t channel);
  static bool CheckRelayChannel(uint32_t channel);
  static bool CheckPWMChannel(uint32_t channel);
  static bool CheckAnalogInput(uint32_t channel);
  static bool CheckAnalogOutput(uint32_t channel);
  static bool CheckSolenoidChannel(uint32_t channel);
  static bool CheckPDPChannel(uint32_t channel);

  static const uint32_t kDigitalChannels = 26;
  static const uint32_t kAnalogInputs = 8;
  static const uint32_t kAnalogOutputs = 2;
  static const uint32_t kSolenoidChannels = 8;
  static const uint32_t kSolenoidModules = 2;
  static const uint32_t kPwmChannels = 20;
  static const uint32_t kRelayChannels = 8;
  static const uint32_t kPDPChannels = 16;
  static const uint32_t kChassisSlots = 8;

 protected:
  void AddToSingletonList();

  static void* m_digital_ports[kDigitalChannels];
  static void* m_relay_ports[kRelayChannels];
  static void* m_pwm_ports[kPwmChannels];

 private:
  static SensorBase* m_singletonList;
  SensorBase* m_nextSingleton = nullptr;
};
