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

    /** Constructs an empty fault set. */
    Faults() = default;

    /**
     * Decodes a packed A301 fault bit field.
     *
     * @param faults packed fault bits
     */
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

    /** Constructs an empty warning set. */
    Warnings() = default;

    /**
     * Decodes a packed A301 warning bit field.
     *
     * @param warnings packed warning bits
     */
    explicit Warnings(uint16_t warnings);
  };

  /** A boolean value read from an A301 periodic status frame. */
  class BooleanStatusSignal : public A301StatusSignal<bool> {
   public:
    /** Inherits the A301 status signal constructors. */
    using A301StatusSignal<bool>::A301StatusSignal;

    /** @return the most recently received value */
    bool Get() const { return A301StatusSignal<bool>::Get(); }

    /**
     * @param defaultValue value to return when this signal is invalid
     * @return the received value or @p defaultValue
     */
    bool Get(bool defaultValue) const {
      return A301StatusSignal<bool>::Get(defaultValue);
    }

    /** @return true when the value is valid */
    bool IsValid() const { return A301StatusSignal<bool>::IsValid(); }

    /** @return the error associated with the read */
    A301Error GetError() const { return A301StatusSignal<bool>::GetError(); }

    /** @return zero on success, or a HAL error code */
    int32_t GetStatus() const { return A301StatusSignal<bool>::GetStatus(); }

    /** @return the CAN frame timestamp in microseconds */
    uint64_t GetTimestamp() const {
      return A301StatusSignal<bool>::GetTimestamp();
    }
  };

  /** A floating-point value read from an A301 periodic status frame. */
  class DoubleStatusSignal : public A301StatusSignal<double> {
   public:
    /** Inherits the A301 status signal constructors. */
    using A301StatusSignal<double>::A301StatusSignal;

    /** @return the most recently received value */
    double Get() const { return A301StatusSignal<double>::Get(); }

    /**
     * @param defaultValue value to return when this signal is invalid
     * @return the received value or @p defaultValue
     */
    double Get(double defaultValue) const {
      return A301StatusSignal<double>::Get(defaultValue);
    }

    /** @return true when the value is valid */
    bool IsValid() const { return A301StatusSignal<double>::IsValid(); }

    /** @return the error associated with the read */
    A301Error GetError() const { return A301StatusSignal<double>::GetError(); }

    /** @return zero on success, or a HAL error code */
    int32_t GetStatus() const { return A301StatusSignal<double>::GetStatus(); }

    /** @return the CAN frame timestamp in microseconds */
    uint64_t GetTimestamp() const {
      return A301StatusSignal<double>::GetTimestamp();
    }
  };

  /** A fault value read from an A301 periodic status frame. */
  class FaultsStatusSignal : public A301StatusSignal<Faults> {
   public:
    /** Inherits the A301 status signal constructors. */
    using A301StatusSignal<Faults>::A301StatusSignal;

    /** @return the most recently received fault set */
    Faults Get() const { return A301StatusSignal<Faults>::Get(); }

    /**
     * @param defaultValue value to return when this signal is invalid
     * @return the received value or @p defaultValue
     */
    Faults Get(Faults defaultValue) const {
      return A301StatusSignal<Faults>::Get(defaultValue);
    }

    /** @return true when the value is valid */
    bool IsValid() const { return A301StatusSignal<Faults>::IsValid(); }

    /** @return the error associated with the read */
    A301Error GetError() const { return A301StatusSignal<Faults>::GetError(); }

    /** @return zero on success, or a HAL error code */
    int32_t GetStatus() const { return A301StatusSignal<Faults>::GetStatus(); }

    /** @return the CAN frame timestamp in microseconds */
    uint64_t GetTimestamp() const {
      return A301StatusSignal<Faults>::GetTimestamp();
    }
  };

  /** A warning value read from an A301 periodic status frame. */
  class WarningsStatusSignal : public A301StatusSignal<Warnings> {
   public:
    /** Inherits the A301 status signal constructors. */
    using A301StatusSignal<Warnings>::A301StatusSignal;

    /** @return the most recently received warning set */
    Warnings Get() const { return A301StatusSignal<Warnings>::Get(); }

    /**
     * @param defaultValue value to return when this signal is invalid
     * @return the received value or @p defaultValue
     */
    Warnings Get(Warnings defaultValue) const {
      return A301StatusSignal<Warnings>::Get(defaultValue);
    }

    /** @return true when the value is valid */
    bool IsValid() const { return A301StatusSignal<Warnings>::IsValid(); }

    /** @return the error associated with the read */
    A301Error GetError() const {
      return A301StatusSignal<Warnings>::GetError();
    }

    /** @return zero on success, or a HAL error code */
    int32_t GetStatus() const {
      return A301StatusSignal<Warnings>::GetStatus();
    }

    /** @return the CAN frame timestamp in microseconds */
    uint64_t GetTimestamp() const {
      return A301StatusSignal<Warnings>::GetTimestamp();
    }
  };

  /** A gearbox value read from an A301 periodic status frame. */
  class GearboxRPMStatusSignal : public A301StatusSignal<GearboxRPM> {
   public:
    /** Inherits the A301 status signal constructors. */
    using A301StatusSignal<GearboxRPM>::A301StatusSignal;

    /** @return the most recently received gearbox speed variant */
    GearboxRPM Get() const { return A301StatusSignal<GearboxRPM>::Get(); }

    /**
     * @param defaultValue value to return when this signal is invalid
     * @return the received value or @p defaultValue
     */
    GearboxRPM Get(GearboxRPM defaultValue) const {
      return A301StatusSignal<GearboxRPM>::Get(defaultValue);
    }

    /** @return true when the value is valid */
    bool IsValid() const { return A301StatusSignal<GearboxRPM>::IsValid(); }

    /** @return the error associated with the read */
    A301Error GetError() const {
      return A301StatusSignal<GearboxRPM>::GetError();
    }

    /** @return zero on success, or a HAL error code */
    int32_t GetStatus() const {
      return A301StatusSignal<GearboxRPM>::GetStatus();
    }

    /** @return the CAN frame timestamp in microseconds */
    uint64_t GetTimestamp() const {
      return A301StatusSignal<GearboxRPM>::GetTimestamp();
    }
  };

  /**
   * Constructs an A301 using its factory-default device ID.
   *
   * @param bus CAN bus containing the controller
   */
  explicit A301(CANBusMap bus);

  /**
   * Constructs an A301.
   *
   * On a Motioncore CAN bus, the connected device ID is automatically
   * detected when possible and the supplied ID is used as a fallback.
   *
   * @param bus CAN bus containing the controller
   * @param deviceId requested device ID
   */
  A301(CANBusMap bus, int deviceId);

  /**
   * Copy construction is disabled.
   *
   * @param other A301 that would be copied
   */
  A301(const A301& other) = delete;

  /**
   * Copy assignment is disabled.
   *
   * @param other A301 that would be copied
   * @return this object
   */
  A301& operator=(const A301& other) = delete;

  /**
   * Move construction is disabled.
   *
   * @param other A301 that would be moved
   */
  A301(A301&& other) = delete;

  /**
   * Move assignment is disabled.
   *
   * @param other A301 that would be moved
   * @return this object
   */
  A301& operator=(A301&& other) = delete;

  /** Releases the native A301 handle. */
  ~A301() override = default;

  /** @return the CAN bus ID */
  int GetBusId() const;

  /** @return the resolved device ID */
  int GetDeviceId() const;

  /** @return the controller firmware version */
  FirmwareVersion GetFirmwareVersion() const;

  /** @return the firmware version in human-readable form */
  std::string GetFirmwareString() const;

  /** @return a signal indicating whether an active fault is present */
  BooleanStatusSignal HasActiveFault() const;

  /** @return a signal indicating whether a sticky fault is present */
  BooleanStatusSignal HasStickyFault() const;

  /** @return a signal indicating whether an active warning is present */
  BooleanStatusSignal HasActiveWarning() const;

  /** @return a signal indicating whether a sticky warning is present */
  BooleanStatusSignal HasStickyWarning() const;

  /** @return active motor faults */
  FaultsStatusSignal GetFaults() const;

  /** @return sticky motor faults recorded since they were last cleared */
  FaultsStatusSignal GetStickyFaults() const;

  /** @return active motor warnings */
  WarningsStatusSignal GetWarnings() const;

  /** @return sticky motor warnings recorded since they were last cleared */
  WarningsStatusSignal GetStickyWarnings() const;

  /**
   * Clears active and sticky faults and warnings.
   *
   * @return command status
   */
  A301Error ClearFaults();

  /** @return the controller input-voltage signal, in volts */
  DoubleStatusSignal GetBusVoltage() const;

  /** @return the applied-output duty-cycle signal */
  DoubleStatusSignal GetAppliedOutput() const;

  /** @return the motor-current signal, in amperes */
  DoubleStatusSignal GetMotorCurrent() const;

  /** @return the motor-temperature signal, in degrees Celsius */
  DoubleStatusSignal GetMotorTemperature() const;

  /** @return the attached gearbox speed-variant signal */
  GearboxRPMStatusSignal GetGearboxRPM() const;

  /** @return the relative encoder-position signal, in motor rotations */
  DoubleStatusSignal GetRelativeEncoderPosition() const;

  /** @return the relative encoder-velocity signal, in RPM */
  DoubleStatusSignal GetEncoderVelocity() const;

  /** @return the absolute encoder-position signal, in rotations */
  DoubleStatusSignal GetAbsoluteEncoderPosition() const;

  /**
   * Sets the relative encoder position.
   *
   * @param position position in motor rotations
   * @return command status
   */
  A301Error SetRelativeEncoderPosition(double position);

  /**
   * Sets the absolute encoder position.
   *
   * @param position position in rotations
   * @return command status
   */
  A301Error SetAbsoluteEncoderPosition(double position);

  /**
   * Sets a velocity setpoint.
   *
   * @param velocity velocity in RPM
   * @return command status
   */
  A301Error SetVelocity(double velocity);

  /**
   * Sets a relative position setpoint using maximum speed.
   *
   * @param position position in motor rotations
   * @return command status
   */
  A301Error SetRelativePosition(double position);

  /**
   * Sets a relative position setpoint with a specific approach speed.
   *
   * A speed less than or equal to zero requests maximum speed.
   *
   * @param position position in motor rotations
   * @param speed approach speed in RPM
   * @return command status
   */
  A301Error SetRelativePositionWithSpeed(double position, double speed);

  /**
   * Sets an absolute position setpoint using maximum speed.
   *
   * @param position absolute position in rotations
   * @return command status
   */
  A301Error SetAbsolutePosition(double position);

  /**
   * Sets an absolute position setpoint with a specific approach speed.
   *
   * A speed less than or equal to zero requests maximum speed.
   *
   * @param position absolute position in rotations
   * @param speed approach speed in RPM
   * @return command status
   */
  A301Error SetAbsolutePositionWithSpeed(double position, double speed);

  /**
   * Sets a motor-current setpoint.
   *
   * @param current current in amperes
   * @return command status
   */
  A301Error SetCurrent(double current);

  /**
   * Sets the idle mode.
   *
   * @param idleMode idle mode
   * @return command status
   */
  A301Error SetIdleMode(IdleMode idleMode);

  /** @return the configured idle mode */
  IdleMode GetIdleMode() const;

  /**
   * Enables continuous input for absolute position control.
   *
   * @return command status
   */
  A301Error EnableAbsolutePositionContinuousInput();

  /**
   * Disables continuous input for absolute position control.
   *
   * @return command status
   */
  A301Error DisableAbsolutePositionContinuousInput();

  /** @return true when absolute position continuous input is enabled */
  bool IsAbsolutePositionContinuousInputEnabled() const;

  /**
   * Sets the absolute encoder range offset.
   *
   * @param offset offset in rotations, from -0.5 to 0.5
   * @return command status
   */
  A301Error SetAbsoluteEncoderRangeOffset(double offset);

  /** @return the absolute encoder range offset in rotations */
  double GetAbsoluteEncoderRangeOffset() const;

  /**
   * Sets the controller duty cycle.
   *
   * @param throttle duty cycle, conventionally from -1.0 to 1.0
   */
  void SetThrottle(double throttle) override;

  /** @return the last commanded duty cycle */
  double GetThrottle() const override;

  /**
   * Sets the controller's internal voltage setpoint.
   *
   * @param outputVolts voltage setpoint
   */
  void SetVoltage(wpi::units::volt_t outputVolts) override;

  /**
   * Sets whether non-position control output is inverted.
   *
   * @param isInverted true to invert the output
   */
  void SetInverted(bool isInverted) override;

  /** @return true when non-position control output is inverted */
  bool GetInverted() const override;

  /** Stops the motor output. */
  void Disable() override;

  /**
   * Sets a periodic status frame period.
   *
   * The controller may normalize the requested period. Use
   * GetStatusFramePeriod() to retrieve the effective value.
   *
   * @param frame periodic status frame
   * @param periodMs requested period in milliseconds, from 0 to 1000
   * @return this object for method chaining
   */
  A301& SetStatusFramePeriod(PeriodicFrame frame, int periodMs);

  /**
   * Returns the effective periodic status frame period.
   *
   * @param frame periodic status frame
   * @return period in milliseconds
   */
  int GetStatusFramePeriod(PeriodicFrame frame) const;

  /**
   * Sets the fault signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& FaultsPeriodMs(int periodMs);

  /** @return the effective fault signal period in milliseconds */
  int GetFaultsPeriodMs() const;

  /**
   * Sets the warning signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& WarningsPeriodMs(int periodMs);

  /** @return the effective warning signal period in milliseconds */
  int GetWarningsPeriodMs() const;

  /**
   * Sets the bus-voltage signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& BusVoltagePeriodMs(int periodMs);

  /** @return the effective bus-voltage signal period in milliseconds */
  int GetBusVoltagePeriodMs() const;

  /**
   * Sets the applied-output signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& AppliedOutputPeriodMs(int periodMs);

  /** @return the effective applied-output signal period in milliseconds */
  int GetAppliedOutputPeriodMs() const;

  /**
   * Sets the motor-current signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& MotorCurrentPeriodMs(int periodMs);

  /** @return the effective motor-current signal period in milliseconds */
  int GetMotorCurrentPeriodMs() const;

  /**
   * Sets the motor-temperature signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& MotorTemperaturePeriodMs(int periodMs);

  /** @return the effective motor-temperature signal period in milliseconds */
  int GetMotorTemperaturePeriodMs() const;

  /**
   * Sets the relative encoder-position signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& RelativeEncoderPositionPeriodMs(int periodMs);

  /**
   * @return the effective relative encoder-position signal period in
   * milliseconds
   */
  int GetRelativeEncoderPositionPeriodMs() const;

  /**
   * Sets the encoder-velocity signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& EncoderVelocityPeriodMs(int periodMs);

  /** @return the effective encoder-velocity signal period in milliseconds */
  int GetEncoderVelocityPeriodMs() const;

  /**
   * Sets the absolute encoder-position signal period.
   *
   * @param periodMs requested period in milliseconds
   * @return this object for method chaining
   */
  A301& AbsoluteEncoderPositionPeriodMs(int periodMs);

  /**
   * @return the effective absolute encoder-position signal period in
   * milliseconds
   */
  int GetAbsoluteEncoderPositionPeriodMs() const;

 private:
  static constexpr double kDefaultPositionSpeed = 0.0;

  /** @return decoded periodic status frame 0 */
  A301StatusSignal<HAL_A301PeriodicStatus0> GetPeriodicStatus0() const;

  /** @return decoded periodic status frame 1 */
  A301StatusSignal<HAL_A301PeriodicStatus1> GetPeriodicStatus1() const;

  /** @return decoded periodic status frame 2 */
  A301StatusSignal<HAL_A301PeriodicStatus2> GetPeriodicStatus2() const;

  /** @return decoded periodic status frame 3 */
  A301StatusSignal<HAL_A301PeriodicStatus3> GetPeriodicStatus3() const;

  /**
   * Sends a repeating control setpoint.
   *
   * @param controlType controller control mode
   * @param setpoint requested control value
   * @param positionSpeed approach speed for position controls, or zero for
   * maximum speed
   * @return command status
   */
  A301Error SetSetpoint(HAL_A301ControlType controlType, double setpoint,
                        double positionSpeed = kDefaultPositionSpeed);

  wpi::util::Handle<HAL_A301Handle, HAL_FreeA301> m_handle;
  int m_busId{};
  int m_deviceId{};
  std::atomic<double> m_setpoint{};
};

}  // namespace wpi
