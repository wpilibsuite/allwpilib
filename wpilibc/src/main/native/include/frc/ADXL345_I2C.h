// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <wpi/struct/Struct.h>
#include <wpi/telemetry/TelemetryLoggable.h>
#include <wpi/telemetry/TelemetryTable.h>

#include "frc/I2C.h"

namespace frc {

/**
 * ADXL345 Accelerometer on I2C.
 *
 * This class allows access to a Analog Devices ADXL345 3-axis accelerometer on
 * an I2C bus. This class assumes the default (not alternate) sensor address of
 * 0x1D (7-bit address).
 */
class ADXL345_I2C : public wpi::TelemetryLoggable {
 public:
  /**
   * Accelerometer range.
   */
  enum Range {
    /// 2 Gs max.
    kRange_2G = 0,
    /// 4 Gs max.
    kRange_4G = 1,
    /// 8 Gs max.
    kRange_8G = 2,
    /// 16 Gs max.
    kRange_16G = 3
  };

  /**
   * Accelerometer axes.
   */
  enum Axes {
    /// X axis.
    kAxis_X = 0x00,
    /// Y axis.
    kAxis_Y = 0x02,
    /// Z axis.
    kAxis_Z = 0x04
  };

  /**
   * Container type for accelerations from all axes.
   */
  struct AllAxes {
    /// Acceleration along the X axis in g-forces.
    double x = 0.0;
    /// Acceleration along the Y axis in g-forces.
    double y = 0.0;
    /// Acceleration along the Z axis in g-forces.
    double z = 0.0;
  };

  /// Default I2C device address.
  static constexpr int kAddress = 0x1D;

  /**
   * Constructs the ADXL345 Accelerometer over I2C.
   *
   * @param port          The I2C port the accelerometer is attached to
   * @param range         The range (+ or -) that the accelerometer will measure
   * @param deviceAddress The I2C address of the accelerometer (0x1D or 0x53)
   */
  explicit ADXL345_I2C(I2C::Port port, Range range = kRange_2G,
                       int deviceAddress = kAddress);
  ~ADXL345_I2C() override = default;

  ADXL345_I2C(ADXL345_I2C&&) = default;
  ADXL345_I2C& operator=(ADXL345_I2C&&) = default;

  I2C::Port GetI2CPort() const;
  int GetI2CDeviceAddress() const;

  /**
   * Set the measuring range of the accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the
   *     accelerometer will measure.
   */
  void SetRange(Range range);

  /**
   * Returns the acceleration along the X axis in g-forces.
   *
   * @return The acceleration along the X axis in g-forces.
   */
  double GetX();

  /**
   * Returns the acceleration along the Y axis in g-forces.
   *
   * @return The acceleration along the Y axis in g-forces.
   */
  double GetY();

  /**
   * Returns the acceleration along the Z axis in g-forces.
   *
   * @return The acceleration along the Z axis in g-forces.
   */
  double GetZ();

  /**
   * Get the acceleration of one axis in Gs.
   *
   * @param axis The axis to read from.
   * @return Acceleration of the ADXL345 in Gs.
   */
  virtual double GetAcceleration(Axes axis) const;

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the
   *         ADXL345 in Gs.
   */
  virtual AllAxes GetAccelerations() const;

  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

  std::string_view GetTelemetryType() const override;

 private:
  mutable I2C m_i2c;

  hal::SimDevice m_simDevice;
  hal::SimEnum m_simRange;
  hal::SimDouble m_simX;
  hal::SimDouble m_simY;
  hal::SimDouble m_simZ;

  static constexpr int kPowerCtlRegister = 0x2D;
  static constexpr int kDataFormatRegister = 0x31;
  static constexpr int kDataRegister = 0x32;
  static constexpr double kGsPerLSB = 0.00390625;

  enum PowerCtlFields {
    kPowerCtl_Link = 0x20,
    kPowerCtl_AutoSleep = 0x10,
    kPowerCtl_Measure = 0x08,
    kPowerCtl_Sleep = 0x04
  };

  enum DataFormatFields {
    kDataFormat_SelfTest = 0x80,
    kDataFormat_SPI = 0x40,
    kDataFormat_IntInvert = 0x20,
    kDataFormat_FullRes = 0x08,
    kDataFormat_Justify = 0x04
  };
};

}  // namespace frc

template <>
struct wpi::Struct<frc::ADXL345_I2C::AllAxes> {
  static constexpr std::string_view GetTypeName() {
    return "ADXL345_I2C.AllAxes";
  }
  static constexpr size_t GetSize() { return 8 * 3; }
  static constexpr std::string_view GetSchema() {
    return "double x;double y;double z";
  }

  static frc::ADXL345_I2C::AllAxes Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::ADXL345_I2C::AllAxes& value);
};

static_assert(wpi::StructSerializable<frc::ADXL345_I2C::AllAxes>);
