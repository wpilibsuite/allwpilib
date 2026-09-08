// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//
// This file is based on the FTC SDK drivers for the AMS TMD3782 family of color
// sensors, which were made available under the BSD 3-Clause License.
// Copyright (c) 2016-2017 Robert Atkinson and Steve Geffner.

#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "wpi/hardware/bus/I2C.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/Color.hpp"

namespace wpi {

/**
 * Driver for the REV Robotics Color Sensor V2.
 *
 * The sensor is an AMS TMD37821, which combines an RGBC color sensor with an
 * infrared proximity sensor. The proximity channel is exposed both as a
 * calibrated distance and as a raw reflected light level.
 *
 * Call Update() once per robot loop. All accessors return the values cached by
 * the most recent update; they do not access the device.
 *
 * Aborted I2C transactions, an unexpected device ID, and data the sensor
 * reports as not yet valid are reported through DeviceStatus instead of
 * throwing an exception, and the previously cached measurements are preserved.
 */
class RevColorSensorV2 {
 public:
  /// Default 7-bit I2C address of the Color Sensor V2.
  static constexpr int DEFAULT_ADDRESS = 0x39;

  /** Register map for the TMD37821. */
  enum class Register {
    /// Enables states and interrupts.
    ENABLE = 0x00,
    /// RGBC integration time.
    ATIME = 0x01,
    /// Wait time.
    WTIME = 0x03,
    /// Clear channel interrupt low threshold.
    AILT = 0x04,
    /// Clear channel interrupt high threshold.
    AIHT = 0x06,
    /// Interrupt persistence filter.
    PERS = 0x0C,
    /// Configuration.
    CONFIGURATION = 0x0D,
    /// Proximity LED pulse count.
    PPULSE = 0x0E,
    /// Gain and proximity LED drive control.
    CONTROL = 0x0F,
    /// Device identifier.
    DEVICE_ID = 0x12,
    /// Device status.
    STATUS = 0x13,
    /// Clear channel data.
    CLEAR = 0x14,
    /// Red channel data.
    RED = 0x16,
    /// Green channel data.
    GREEN = 0x18,
    /// Blue channel data.
    BLUE = 0x1A,
    /// Proximity data.
    PROXIMITY = 0x1C
  };

  /** Analog gain applied to the color channels by the sensor. */
  enum class Gain {
    /// 1x gain.
    GAIN_1 = 0x00,
    /// 4x gain.
    GAIN_4 = 0x01,
    /// 16x gain.
    GAIN_16 = 0x02,
    /// 64x gain.
    GAIN_64 = 0x03
  };

  /** Nominal drive current of the proximity LED. */
  enum class LedDrive {
    /// 100% drive current.
    PERCENT_100 = 0x00 << 6,
    /// 50% drive current.
    PERCENT_50 = 0x01 << 6,
    /// 25% drive current.
    PERCENT_25 = 0x02 << 6,
    /// 12.5% drive current.
    PERCENT_12_5 = 0x03 << 6
  };

  /** Sensor status, including device-reported faults and local read failures.
   */
  enum class DeviceStatus {
    /// The sensor has not been configured successfully yet.
    NOT_INITIALIZED,
    /// The sensor is operating normally.
    READY,
    /// The device ID register did not report a TMD3782 family part.
    FAULT_UNEXPECTED_DEVICE_ID,
    /// An I2C transaction was aborted, or the sensor reported that its data was
    /// not valid.
    FAULT_BAD_READ
  };

  /** Reason the driver rejected data or an I2C transaction. */
  enum class FailureReason {
    /// The I2C controller aborted a read transaction.
    I2C_READ_ABORTED,
    /// The I2C controller aborted a write transaction.
    I2C_WRITE_ABORTED,
    /// The device ID register did not report a TMD3782 family part.
    UNEXPECTED_DEVICE_ID,
    /// The sensor had not completed an RGBC cycle since the color channels were
    /// enabled.
    COLOR_DATA_NOT_VALID,
    /// The sensor had not completed a proximity cycle since the proximity
    /// channel was enabled.
    PROXIMITY_DATA_NOT_VALID
  };

  /**
   * Constructs a Color Sensor V2 and configures it.
   *
   * Configuration failures are reported through GetDeviceStatus(); Update()
   * retries configuration until it succeeds.
   *
   * @param port I2C port to which the sensor is connected.
   * @param deviceAddress 7-bit I2C address.
   * @throws std::invalid_argument if deviceAddress is outside the 7-bit range.
   */
  explicit RevColorSensorV2(I2C::Port port,
                            int deviceAddress = DEFAULT_ADDRESS);

  RevColorSensorV2(RevColorSensorV2&&) = default;
  RevColorSensorV2& operator=(RevColorSensorV2&&) = default;

  /**
   * Returns the I2C port.
   *
   * @return I2C port.
   */
  I2C::Port GetPort() const;

  /**
   * Returns the I2C address.
   *
   * @return 7-bit I2C address.
   */
  int GetDeviceAddress() const;

  /**
   * Reads the color and proximity channels and updates the cached
   * measurements. Call this once per robot loop.
   *
   * If the sensor has not been configured successfully, this retries
   * configuration first, which blocks for approximately 8 ms.
   *
   * An aborted I2C transaction or data the sensor reports as not yet valid
   * does not throw; it sets the device status to DeviceStatus::FAULT_BAD_READ
   * and preserves the affected measurements.
   */
  void Update();

  /**
   * Sets the analog gain applied to the color channels.
   *
   * The sensor only accepts gain changes while its integrator is off, so this
   * briefly disables and reenables the sensor, blocking for approximately 8 ms.
   * If the sensor has not been configured successfully, the gain is applied the
   * next time configuration succeeds.
   *
   * @param gain Color channel gain.
   * @throws std::invalid_argument if gain is invalid.
   */
  void SetGain(Gain gain);

  /** @return Configured analog gain. */
  Gain GetGain() const;

  /**
   * Sets the nominal drive current of the proximity LED.
   *
   * The distance calibration returned by GetDistance() was fitted at
   * LedDrive::PERCENT_50, the default. Changing the drive current changes the
   * reflected light level for a given distance, so SetDistanceCalibration()
   * must be used to refit the calibration.
   *
   * This briefly disables and reenables the sensor, blocking for approximately
   * 8 ms. If the sensor has not been configured successfully, the drive current
   * is applied the next time configuration succeeds.
   *
   * @param ledDrive Proximity LED drive current.
   * @throws std::invalid_argument if ledDrive is invalid.
   */
  void SetLedDrive(LedDrive ledDrive);

  /** @return Configured proximity LED drive current. */
  LedDrive GetLedDrive() const;

  /**
   * Sets the color channel integration time.
   *
   * The sensor integrates in 2.4 ms cycles, so the time is rounded up to the
   * next whole cycle, up to a maximum of 614.4 ms. A longer integration time
   * raises GetMaximumRawColorValue() and therefore the resolution of the color
   * channels.
   *
   * The sensor only accepts integration time changes while its integrator is
   * off, so this briefly disables and reenables the sensor, blocking for
   * approximately 8 ms. If the sensor has not been configured successfully, the
   * integration time is applied the next time configuration succeeds.
   *
   * @param integrationTime Color channel integration time.
   * @throws std::invalid_argument if integrationTime is nonfinite or not
   *     positive.
   */
  void SetIntegrationTime(wpi::units::millisecond_t integrationTime);

  /** @return Configured color channel integration time. */
  wpi::units::millisecond_t GetIntegrationTime() const;

  /**
   * Sets the number of times the proximity LED is pulsed during each proximity
   * cycle. More pulses raise the reflected light level for a given distance.
   *
   * The distance calibration returned by GetDistance() was fitted at the
   * default of 8 pulses, so changing the pulse count requires refitting the
   * calibration with SetDistanceCalibration().
   *
   * This briefly disables and reenables the sensor, blocking for approximately
   * 8 ms. If the sensor has not been configured successfully, the pulse count
   * is applied the next time configuration succeeds.
   *
   * @param proximityPulseCount Number of LED pulses per proximity cycle, from
   *     1 to 255.
   * @throws std::invalid_argument if proximityPulseCount is outside 1 to 255.
   */
  void SetProximityPulseCount(int proximityPulseCount);

  /** @return Configured number of proximity LED pulses per proximity cycle. */
  int GetProximityPulseCount() const;

  /**
   * Sets a scale factor applied by this driver to the normalized color
   * channels. This is applied in software after the sensor's own gain; it does
   * not change the raw channel values.
   *
   * @param softwareGain Scale factor applied to the normalized color channels.
   * @throws std::invalid_argument if softwareGain is nonfinite or not positive.
   */
  void SetSoftwareGain(double softwareGain);

  /** @return Scale factor applied to the normalized color channels. */
  double GetSoftwareGain() const;

  /**
   * Sets the parameters of the curve that converts the raw proximity reading
   * into a distance.
   *
   * The raw proximity signal follows an inverse square law, and the parameters
   * fit it to a linear measure of distance in centimeters:
   *
   *     rawProximity = a + b * (cm + c)^-2
   *
   * The default parameters were fitted for this sensor at the default LED drive
   * current and proximity pulse count. Because the fit is affected by the
   * infrared reflectivity of the target surface, the linearity it produces is
   * usually preserved on other surfaces even when the scale is not, so a simple
   * multiplicative correction is often enough to retune it.
   *
   * @param a The a parameter of the fitted curve.
   * @param b The b parameter of the fitted curve.
   * @param c The c parameter of the fitted curve.
   * @throws std::invalid_argument if any parameter is nonfinite.
   */
  void SetDistanceCalibration(double a, double b, double c);

  /**
   * Returns the distance to the target measured by the infrared proximity
   * channel, from the plastic housing at the front of the sensor.
   *
   * Readings are most accurate perpendicular to the target surface; a cosine
   * correction is usually appropriate otherwise. The usable range is roughly
   * 1 to 10 cm, and this returns NaN when the reflected light level is too low
   * for the calibration curve, which is the case when the target is beyond that
   * range.
   *
   * @return Distance, or NaN if the target is out of range.
   */
  wpi::units::meter_t GetDistance() const;

  /**
   * Returns the raw reflected light level measured by the infrared proximity
   * channel. The value rises as the target gets closer.
   *
   * @return Raw proximity reading, from 0 to GetMaximumRawProximityValue().
   */
  int GetRawProximity() const;

  /**
   * Returns the reflected light level measured by the infrared proximity
   * channel, normalized against its saturation value. The value rises as the
   * target gets closer.
   *
   * @return Proximity reading, from 0 to 1.
   */
  double GetProximity() const;

  /** @return Raw proximity reading at which the proximity channel saturates. */
  int GetMaximumRawProximityValue() const;

  /**
   * Returns the color measured by the sensor, with each channel normalized
   * against GetMaximumRawColorValue() and scaled by the software gain.
   *
   * @return Measured color.
   */
  wpi::util::Color GetColor() const;

  /** @return Normalized red channel, from 0 to 1. */
  double GetRed() const;

  /** @return Normalized green channel, from 0 to 1. */
  double GetGreen() const;

  /** @return Normalized blue channel, from 0 to 1. */
  double GetBlue() const;

  /**
   * Returns the normalized clear channel, which measures unfiltered light and
   * is a useful measure of overall brightness.
   *
   * @return Clear channel, from 0 to 1.
   */
  double GetClear() const;

  /** @return Raw red channel count, from 0 to GetMaximumRawColorValue(). */
  int GetRawRed() const;

  /** @return Raw green channel count, from 0 to GetMaximumRawColorValue(). */
  int GetRawGreen() const;

  /** @return Raw blue channel count, from 0 to GetMaximumRawColorValue(). */
  int GetRawBlue() const;

  /** @return Raw clear channel count, from 0 to GetMaximumRawColorValue(). */
  int GetRawClear() const;

  /**
   * Returns the highest raw color channel count reachable with the configured
   * integration time.
   *
   * @return Maximum raw color channel count.
   */
  int GetMaximumRawColorValue() const;

  /**
   * Returns the device identifier reported by the sensor.
   *
   * @return Device identifier, or zero if it has not been read successfully.
   */
  uint8_t GetDeviceId() const;

  /** @return Current device or read status. */
  DeviceStatus GetDeviceStatus() const;

  /**
   * @return Reason for the most recent driver-detected failure, or std::nullopt
   *     if none has occurred.
   */
  std::optional<FailureReason> GetLastFailureReason() const;

  /** @return Total number of failures detected by this driver instance. */
  uint64_t GetFailureCount() const;

 private:
  /// Every register access is prefixed with the command bit.
  static constexpr int COMMAND_BIT = 0x80;

  /// Command type selecting the auto-increment protocol. Without it the sensor
  /// uses the repeated-byte protocol, which returns the addressed register once
  /// per byte instead of advancing through consecutive registers, so a
  /// multi-byte read would return the same register repeatedly.
  static constexpr int COMMAND_TYPE_AUTO_INCREMENT = 0x01 << 5;

  /// Register address prefix used for every read and write.
  static constexpr int COMMAND_AUTO_INCREMENT =
      COMMAND_BIT | COMMAND_TYPE_AUTO_INCREMENT;

  static constexpr uint8_t TMD37821_DEVICE_ID = 0x60;
  static constexpr uint8_t TMD37823_DEVICE_ID = 0x69;

  static constexpr int ENABLE_POWER_ON = 0x01;
  static constexpr int ENABLE_COLOR = 0x02;
  static constexpr int ENABLE_PROXIMITY = 0x04;
  static constexpr int ENABLE_WAIT = 0x08;
  static constexpr int ENABLE_WRITABLE_MASK =
      ENABLE_POWER_ON | ENABLE_COLOR | ENABLE_PROXIMITY | ENABLE_WAIT;

  static constexpr int STATUS_COLOR_VALID = 0x01;
  static constexpr int STATUS_PROXIMITY_VALID = 0x02;

  /// The TMD3782 family requires this control bit to be set to select the IR
  /// photodiode.
  static constexpr int CONTROL_IR_DIODE = 0x20;

  static constexpr int GAIN_MASK = 0x03;
  static constexpr int LED_DRIVE_MASK = 0xC0;

  /// STATUS through the high byte of the proximity data, read as one block.
  static constexpr int BULK_READ_LENGTH = 11;

  static constexpr std::size_t CLEAR_OFFSET = 1;
  static constexpr std::size_t RED_OFFSET = 3;
  static constexpr std::size_t GREEN_OFFSET = 5;
  static constexpr std::size_t BLUE_OFFSET = 7;
  static constexpr std::size_t PROXIMITY_OFFSET = 9;

  static constexpr double INTEGRATION_CYCLE_MILLISECONDS = 2.4;
  static constexpr int MAX_INTEGRATION_CYCLES = 256;
  static constexpr int COUNTS_PER_INTEGRATION_CYCLE = 1024;
  static constexpr int MAX_RAW_COLOR_VALUE = 65535;
  static constexpr int PROXIMITY_SATURATION = 1023;

  /// The datasheet specifies a 2.4 ms warm-up delay after power on.
  static constexpr int POWER_ON_DELAY_MILLISECONDS = 3;

  static constexpr int ENABLE_SETTLE_DELAY_MILLISECONDS = 5;

  static constexpr wpi::units::millisecond_t DEFAULT_INTEGRATION_TIME{24.0};
  static constexpr int DEFAULT_PROXIMITY_PULSE_COUNT = 8;

  static constexpr double DEFAULT_A_PARAM = 186.347;
  static constexpr double DEFAULT_B_PARAM = 30403.5;
  static constexpr double DEFAULT_C_PARAM = 0.576649;

  static int ValidateAddress(int deviceAddress);
  static int IntegrationTimeRegister(wpi::units::millisecond_t integrationTime);

  bool Initialize();
  void Reconfigure();
  bool Configure();
  bool WriteControl();
  bool Enable();
  bool Disable();
  bool WriteEnable(int value);
  std::vector<uint8_t> ReadRegister(Register reg, int count);
  bool WriteRegister(Register reg, int value);
  void RecordFailure(FailureReason reason);
  double Normalize(int rawValue) const;
  int IntegrationCycles() const;

  I2C m_i2c;

  Gain m_gain = Gain::GAIN_4;
  LedDrive m_ledDrive = LedDrive::PERCENT_50;
  int m_integrationTimeRegister =
      IntegrationTimeRegister(DEFAULT_INTEGRATION_TIME);
  int m_proximityPulseCount = DEFAULT_PROXIMITY_PULSE_COUNT;
  double m_softwareGain = 1.0;

  double m_aParam = DEFAULT_A_PARAM;
  double m_bParam = DEFAULT_B_PARAM;
  double m_cParam = DEFAULT_C_PARAM;

  bool m_initialized = false;
  uint8_t m_deviceId = 0;
  DeviceStatus m_deviceStatus = DeviceStatus::NOT_INITIALIZED;
  std::optional<FailureReason> m_lastFailureReason;
  uint64_t m_failureCount = 0;

  int m_rawClear = 0;
  int m_rawRed = 0;
  int m_rawGreen = 0;
  int m_rawBlue = 0;
  int m_rawProximity = 0;
};

}  // namespace wpi
