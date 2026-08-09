// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <atomic>
#include <string>

#include "wpi/drivers/motor/A301Error.hpp"
#include "wpi/drivers/motor/A301StatusSignal.hpp"
#include "wpi/hal/A301.h"
#include "wpi/hardware/bus/CANBusMap.hpp"
#include "wpi/hardware/motor/MotorController.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/Handle.hpp"

namespace wpi {

/** High-level interface to a FIRST A301 motor controller. */
class A301 final : public MotorController {
 public:
  /** The factory-default A301 device ID. */
  static constexpr int kDefaultDeviceId = HAL_A301_DEFAULT_DEVICE_ID;

  /** A periodic status frame produced by the A301. */
  enum class PeriodicFrame {
    kStatus0 = HAL_A301_STATUS_0,
    kStatus1 = HAL_A301_STATUS_1,
    kStatus2 = HAL_A301_STATUS_2,
    kStatus3 = HAL_A301_STATUS_3,
  };

  /** A301 gearbox speed variant. */
  enum class GearboxRPM {
    kUnknown = HAL_A301_GEARBOX_RPM_UNKNOWN,
    kRPM215 = HAL_A301_GEARBOX_RPM_215,
    kRPM500 = HAL_A301_GEARBOX_RPM_500,
  };

  /** Neutral behavior when the controller output is zero. */
  enum class IdleMode {
    kCoast = HAL_A301_IDLE_MODE_COAST,
    kBrake = HAL_A301_IDLE_MODE_BRAKE,
  };

  /** A301 firmware version information. */
  struct FirmwareVersion {
    uint8_t major{};
    uint8_t minor{};
    uint16_t patch{};
    uint8_t prerelease{};
    uint8_t hardwareRevision{};
    uint32_t raw{};
  };

  /** Active or sticky A301 motor faults. */
  struct Faults {
    bool other{};
    bool motorType{};
    bool sensor{};
    bool can{};
    bool temperature{};
    bool gateDriver{};
    bool escEeprom{};
    bool firmware{};
    bool motorStartup{};
    uint16_t rawBits{};

    Faults() = default;
    explicit Faults(uint16_t faults);
  };

  /** Active or sticky A301 motor warnings. */
  struct Warnings {
    bool brownout{};
    bool overcurrent{};
    bool escEeprom{};
    bool extEeprom{};
    bool sensor{};
    bool stall{};
    bool hasReset{};
    bool other{};
    bool overvoltage{};
    bool motorLoopSpeed{};
    uint16_t rawBits{};

    Warnings() = default;
    explicit Warnings(uint16_t warnings);
  };

  /** A boolean value read from an A301 periodic status frame. */
  class BooleanStatusSignal : public A301StatusSignal<bool> {
   public:
    using A301StatusSignal<bool>::A301StatusSignal;

    bool Get() const { return A301StatusSignal<bool>::Get(); }
    bool Get(bool defaultValue) const {
      return A301StatusSignal<bool>::Get(defaultValue);
    }
    bool IsValid() const { return A301StatusSignal<bool>::IsValid(); }
    A301Error GetError() const { return A301StatusSignal<bool>::GetError(); }
    int32_t GetStatus() const { return A301StatusSignal<bool>::GetStatus(); }
    uint64_t GetTimestamp() const {
      return A301StatusSignal<bool>::GetTimestamp();
    }
  };

  /** A floating-point value read from an A301 periodic status frame. */
  class DoubleStatusSignal : public A301StatusSignal<double> {
   public:
    using A301StatusSignal<double>::A301StatusSignal;

    double Get() const { return A301StatusSignal<double>::Get(); }
    double Get(double defaultValue) const {
      return A301StatusSignal<double>::Get(defaultValue);
    }
    bool IsValid() const { return A301StatusSignal<double>::IsValid(); }
    A301Error GetError() const { return A301StatusSignal<double>::GetError(); }
    int32_t GetStatus() const { return A301StatusSignal<double>::GetStatus(); }
    uint64_t GetTimestamp() const {
      return A301StatusSignal<double>::GetTimestamp();
    }
  };

  /** A fault value read from an A301 periodic status frame. */
  class FaultsStatusSignal : public A301StatusSignal<Faults> {
   public:
    using A301StatusSignal<Faults>::A301StatusSignal;

    Faults Get() const { return A301StatusSignal<Faults>::Get(); }
    Faults Get(Faults defaultValue) const {
      return A301StatusSignal<Faults>::Get(defaultValue);
    }
    bool IsValid() const { return A301StatusSignal<Faults>::IsValid(); }
    A301Error GetError() const { return A301StatusSignal<Faults>::GetError(); }
    int32_t GetStatus() const { return A301StatusSignal<Faults>::GetStatus(); }
    uint64_t GetTimestamp() const {
      return A301StatusSignal<Faults>::GetTimestamp();
    }
  };

  /** A warning value read from an A301 periodic status frame. */
  class WarningsStatusSignal : public A301StatusSignal<Warnings> {
   public:
    using A301StatusSignal<Warnings>::A301StatusSignal;

    Warnings Get() const { return A301StatusSignal<Warnings>::Get(); }
    Warnings Get(Warnings defaultValue) const {
      return A301StatusSignal<Warnings>::Get(defaultValue);
    }
    bool IsValid() const { return A301StatusSignal<Warnings>::IsValid(); }
    A301Error GetError() const {
      return A301StatusSignal<Warnings>::GetError();
    }
    int32_t GetStatus() const { return A301StatusSignal<Warnings>::GetStatus(); }
    uint64_t GetTimestamp() const {
      return A301StatusSignal<Warnings>::GetTimestamp();
    }
  };

  /** A gearbox value read from an A301 periodic status frame. */
  class GearboxRPMStatusSignal : public A301StatusSignal<GearboxRPM> {
   public:
    using A301StatusSignal<GearboxRPM>::A301StatusSignal;

    GearboxRPM Get() const { return A301StatusSignal<GearboxRPM>::Get(); }
    GearboxRPM Get(GearboxRPM defaultValue) const {
      return A301StatusSignal<GearboxRPM>::Get(defaultValue);
    }
    bool IsValid() const { return A301StatusSignal<GearboxRPM>::IsValid(); }
    A301Error GetError() const {
      return A301StatusSignal<GearboxRPM>::GetError();
    }
    int32_t GetStatus() const {
      return A301StatusSignal<GearboxRPM>::GetStatus();
    }
    uint64_t GetTimestamp() const {
      return A301StatusSignal<GearboxRPM>::GetTimestamp();
    }
  };

  /** Constructs an A301 using its factory-default device ID. */
  explicit A301(CANBusMap bus);

  /**
   * Constructs an A301.
   *
   * On a Motioncore CAN bus, the connected device ID is automatically
   * detected when possible and the supplied ID is used as a fallback.
   */
  A301(CANBusMap bus, int deviceId);

  A301(const A301&) = delete;
  A301& operator=(const A301&) = delete;
  A301(A301&&) = delete;
  A301& operator=(A301&&) = delete;
  ~A301() override = default;

  /** Returns the CAN bus ID. */
  int GetBusId() const;

  /** Returns the resolved device ID. */
  int GetDeviceId() const;

  /** Returns the controller firmware version. */
  FirmwareVersion GetFirmwareVersion() const;

  /** Returns the firmware version in human-readable form. */
  std::string GetFirmwareString() const;

  BooleanStatusSignal HasActiveFault() const;
  BooleanStatusSignal HasStickyFault() const;
  BooleanStatusSignal HasActiveWarning() const;
  BooleanStatusSignal HasStickyWarning() const;

  FaultsStatusSignal GetFaults() const;
  FaultsStatusSignal GetStickyFaults() const;
  WarningsStatusSignal GetWarnings() const;
  WarningsStatusSignal GetStickyWarnings() const;

  /** Clears active and sticky faults and warnings. */
  A301Error ClearFaults();

  DoubleStatusSignal GetBusVoltage() const;
  DoubleStatusSignal GetAppliedOutput() const;
  DoubleStatusSignal GetMotorCurrent() const;
  DoubleStatusSignal GetMotorTemperature() const;
  GearboxRPMStatusSignal GetGearboxRPM() const;
  DoubleStatusSignal GetRelativeEncoderPosition() const;
  DoubleStatusSignal GetEncoderVelocity() const;
  DoubleStatusSignal GetAbsoluteEncoderPosition() const;

  A301Error SetRelativeEncoderPosition(double position);
  A301Error SetAbsoluteEncoderPosition(double position);
  A301Error SetVelocity(double velocity);
  A301Error SetRelativePosition(double position);
  A301Error SetRelativePositionWithSpeed(double position, double speed);
  A301Error SetAbsolutePosition(double position);
  A301Error SetAbsolutePositionWithSpeed(double position, double speed);
  A301Error SetCurrent(double current);

  A301Error SetIdleMode(IdleMode idleMode);
  IdleMode GetIdleMode() const;

  A301Error EnableAbsolutePositionContinuousInput();
  A301Error DisableAbsolutePositionContinuousInput();
  bool IsAbsolutePositionContinuousInputEnabled() const;

  A301Error SetAbsoluteEncoderRangeOffset(double offset);
  double GetAbsoluteEncoderRangeOffset() const;

  void SetThrottle(double throttle) override;
  double GetThrottle() const override;
  void SetVoltage(wpi::units::volt_t outputVolts) override;
  void SetInverted(bool isInverted) override;
  bool GetInverted() const override;
  void Disable() override;

  /** Sets a periodic status frame period and returns this object. */
  A301& SetStatusFramePeriod(PeriodicFrame frame, int periodMs);

  /** Returns the effective periodic status frame period. */
  int GetStatusFramePeriod(PeriodicFrame frame) const;

  A301& FaultsPeriodMs(int periodMs);
  int GetFaultsPeriodMs() const;
  A301& WarningsPeriodMs(int periodMs);
  int GetWarningsPeriodMs() const;
  A301& BusVoltagePeriodMs(int periodMs);
  int GetBusVoltagePeriodMs() const;
  A301& AppliedOutputPeriodMs(int periodMs);
  int GetAppliedOutputPeriodMs() const;
  A301& MotorCurrentPeriodMs(int periodMs);
  int GetMotorCurrentPeriodMs() const;
  A301& MotorTemperaturePeriodMs(int periodMs);
  int GetMotorTemperaturePeriodMs() const;
  A301& RelativeEncoderPositionPeriodMs(int periodMs);
  int GetRelativeEncoderPositionPeriodMs() const;
  A301& EncoderVelocityPeriodMs(int periodMs);
  int GetEncoderVelocityPeriodMs() const;
  A301& AbsoluteEncoderPositionPeriodMs(int periodMs);
  int GetAbsoluteEncoderPositionPeriodMs() const;

 private:
  static constexpr double kDefaultPositionSpeed = 0.0;

  A301StatusSignal<HAL_A301PeriodicStatus0> GetPeriodicStatus0() const;
  A301StatusSignal<HAL_A301PeriodicStatus1> GetPeriodicStatus1() const;
  A301StatusSignal<HAL_A301PeriodicStatus2> GetPeriodicStatus2() const;
  A301StatusSignal<HAL_A301PeriodicStatus3> GetPeriodicStatus3() const;

  A301Error SetSetpoint(HAL_A301ControlType controlType, double setpoint,
                        double positionSpeed = kDefaultPositionSpeed);

  wpi::util::Handle<HAL_A301Handle, HAL_FreeA301> m_handle;
  int m_busId{};
  int m_deviceId{};
  std::atomic<double> m_setpoint{};
};

}  // namespace wpi
