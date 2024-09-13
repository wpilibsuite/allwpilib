// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 Analog Devices Inc. All Rights Reserved.           */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*                                                                            */
/* Modified by Juan Chong - frcsupport@analog.com                             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <atomic>
#include <thread>

#include <hal/SimDevice.h>
#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/magnetic_field_strength.h>
#include <units/pressure.h>
#include <units/temperature.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/DigitalInput.h"
#include "frc/DigitalOutput.h"
#include "frc/SPI.h"

namespace frc {
/**
 * Use DMA SPI to read rate, acceleration, and magnetometer data from the
 * ADIS16448 IMU and return the robots heading relative to a starting position,
 * AHRS, and instant measurements
 *
 * The ADIS16448 gyro angle outputs track the robot's heading based on the
 * starting position. As the robot rotates the new heading is computed by
 * integrating the rate of rotation returned by the IMU. When the class is
 * instantiated, a short calibration routine is performed where the IMU samples
 * the gyros while at rest to determine the initial offset. This is subtracted
 * from each sample to determine the heading.
 *
 * This class is for the ADIS16448 IMU connected via the SPI port available on
 * the RoboRIO MXP port.
 */

class ADIS16448_IMU : public wpi::Sendable,
                      public wpi::SendableHelper<ADIS16448_IMU> {
 public:
  /**
   * ADIS16448 calibration times.
   */
  enum class CalibrationTime {
    /// 32 ms calibration time.
    _32ms = 0,
    /// 64 ms calibration time.
    _64ms = 1,
    /// 128 ms calibration time.
    _128ms = 2,
    /// 256 ms calibration time.
    _256ms = 3,
    /// 512 ms calibration time.
    _512ms = 4,
    /// 1 s calibration time.
    _1s = 5,
    /// 2 s calibration time.
    _2s = 6,
    /// 4 s calibration time.
    _4s = 7,
    /// 8 s calibration time.
    _8s = 8,
    /// 16 s calibration time.
    _16s = 9,
    /// 32 s calibration time.
    _32s = 10,
    /// 64 s calibration time.
    _64s = 11
  };

  /**
   * IMU axes.
   */
  enum IMUAxis {
    /// The IMU's X axis.
    kX,
    /// The IMU's Y axis.
    kY,
    /// The IMU's Z axis.
    kZ
  };

  /**
   * IMU constructor on onboard MXP CS0, Z-up orientation, and complementary
   * AHRS computation.
   */
  ADIS16448_IMU();

  /**
   * IMU constructor on the specified MXP port and orientation.
   *
   * @param yaw_axis The axis where gravity is present. Valid options are kX,
   * kY, and kZ
   * @param port The SPI port where the IMU is connected.
   * @param cal_time The calibration time that should be used on start-up.
   */
  explicit ADIS16448_IMU(IMUAxis yaw_axis, SPI::Port port,
                         CalibrationTime cal_time);

  ~ADIS16448_IMU() override;

  ADIS16448_IMU(ADIS16448_IMU&&);
  ADIS16448_IMU& operator=(ADIS16448_IMU&&);

  /**
   * Initialize the IMU.
   *
   * Perform gyro offset calibration by collecting data for a number of seconds
   * and computing the center value. The center value is subtracted from
   * subsequent measurements.
   *
   * It's important to make sure that the robot is not moving while the
   * centering calculations are in progress, this is typically done when the
   * robot is first turned on while it's sitting at rest before the match
   * starts.
   *
   * The calibration routine can be triggered by the user during runtime.
   */
  void Calibrate();

  /**
   * Configures the calibration time used for the next calibrate.
   *
   * @param new_cal_time The calibration time that should be used
   */
  int ConfigCalTime(CalibrationTime new_cal_time);

  /**
   * Reset the gyro.
   *
   * Resets the gyro accumulations to a heading of zero. This can be used if
   * there is significant drift in the gyro and it needs to be recalibrated
   * after running.
   */
  void Reset();

  /**
   * Returns the yaw axis angle in degrees (CCW positive).
   */
  units::degree_t GetAngle() const;

  /**
   * Returns the yaw axis angular rate in degrees per second (CCW positive).
   */
  units::degrees_per_second_t GetRate() const;

  /**
   * Returns the accumulated gyro angle in the X axis.
   */
  units::degree_t GetGyroAngleX() const;

  /**
   * Returns the accumulated gyro angle in the Y axis.
   */
  units::degree_t GetGyroAngleY() const;

  /**
   * Returns the accumulated gyro angle in the Z axis.
   */
  units::degree_t GetGyroAngleZ() const;

  /**
   * Returns the angular rate in the X axis.
   */
  units::degrees_per_second_t GetGyroRateX() const;

  /**
   * Returns the angular rate in the Y axis.
   */
  units::degrees_per_second_t GetGyroRateY() const;

  /**
   * Returns the angular rate in the Z axis.
   */
  units::degrees_per_second_t GetGyroRateZ() const;

  /**
   * Returns the acceleration in the X axis.
   */
  units::meters_per_second_squared_t GetAccelX() const;

  /**
   * Returns the acceleration in the Y axis.
   */
  units::meters_per_second_squared_t GetAccelY() const;

  /**
   * Returns the acceleration in the Z axis.
   */
  units::meters_per_second_squared_t GetAccelZ() const;

  /**
   * Returns the complementary angle around the X axis computed from
   * accelerometer and gyro rate measurements.
   */
  units::degree_t GetXComplementaryAngle() const;

  /**
   * Returns the complementary angle around the Y axis computed from
   * accelerometer and gyro rate measurements.
   */
  units::degree_t GetYComplementaryAngle() const;

  /**
   * Returns the X-axis filtered acceleration angle.
   */
  units::degree_t GetXFilteredAccelAngle() const;

  /**
   * Returns the Y-axis filtered acceleration angle.
   */
  units::degree_t GetYFilteredAccelAngle() const;

  /**
   * Returns the magnetic field strength in the X axis.
   */
  units::tesla_t GetMagneticFieldX() const;

  /**
   * Returns the magnetic field strength in the Y axis.
   */
  units::tesla_t GetMagneticFieldY() const;

  /**
   * Returns the magnetic field strength in the Z axis.
   */
  units::tesla_t GetMagneticFieldZ() const;

  /**
   * Returns the barometric pressure.
   */
  units::pounds_per_square_inch_t GetBarometricPressure() const;

  /**
   * Returns the temperature.
   */
  units::celsius_t GetTemperature() const;

  IMUAxis GetYawAxis() const;

  int SetYawAxis(IMUAxis yaw_axis);

  /**
   * Checks the connection status of the IMU.
   *
   * @return True if the IMU is connected, false otherwise.
   */
  bool IsConnected() const;

  /**
   * Configures the decimation rate of the IMU.
   *
   * @param decimationRate The new decimation value.
   * @return 0 if success, 1 if no change, 2 if error.
   */
  int ConfigDecRate(uint16_t decimationRate);

  /**
   * Get the SPI port number.
   *
   * @return The SPI port number.
   */
  int GetPort() const;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  /** @brief ADIS16448 Register Map Declaration */
  static constexpr uint8_t FLASH_CNT = 0x00;  // Flash memory write count
  static constexpr uint8_t XGYRO_OUT = 0x04;  // X-axis gyroscope output
  static constexpr uint8_t YGYRO_OUT = 0x06;  // Y-axis gyroscope output
  static constexpr uint8_t ZGYRO_OUT = 0x08;  // Z-axis gyroscope output
  static constexpr uint8_t XACCL_OUT = 0x0A;  // X-axis accelerometer output
  static constexpr uint8_t YACCL_OUT = 0x0C;  // Y-axis accelerometer output
  static constexpr uint8_t ZACCL_OUT = 0x0E;  // Z-axis accelerometer output
  static constexpr uint8_t XMAGN_OUT = 0x10;  // X-axis magnetometer output
  static constexpr uint8_t YMAGN_OUT = 0x12;  // Y-axis magnetometer output
  static constexpr uint8_t ZMAGN_OUT = 0x14;  // Z-axis magnetometer output
  static constexpr uint8_t BARO_OUT =
      0x16;  // Barometer pressure measurement, high word
  static constexpr uint8_t TEMP_OUT = 0x18;  // Temperature output
  static constexpr uint8_t XGYRO_OFF =
      0x1A;  // X-axis gyroscope bias offset factor
  static constexpr uint8_t YGYRO_OFF =
      0x1C;  // Y-axis gyroscope bias offset factor
  static constexpr uint8_t ZGYRO_OFF =
      0x1E;  // Z-axis gyroscope bias offset factor
  static constexpr uint8_t XACCL_OFF =
      0x20;  // X-axis acceleration bias offset factor
  static constexpr uint8_t YACCL_OFF =
      0x22;  // Y-axis acceleration bias offset factor
  static constexpr uint8_t ZACCL_OFF =
      0x24;  // Z-axis acceleration bias offset factor
  static constexpr uint8_t XMAGN_HIC =
      0x26;  // X-axis magnetometer, hard iron factor
  static constexpr uint8_t YMAGN_HIC =
      0x28;  // Y-axis magnetometer, hard iron factor
  static constexpr uint8_t ZMAGN_HIC =
      0x2A;  // Z-axis magnetometer, hard iron factor
  static constexpr uint8_t XMAGN_SIC =
      0x2C;  // X-axis magnetometer, soft iron factor
  static constexpr uint8_t YMAGN_SIC =
      0x2E;  // Y-axis magnetometer, soft iron factor
  static constexpr uint8_t ZMAGN_SIC =
      0x30;  // Z-axis magnetometer, soft iron factor
  static constexpr uint8_t GPIO_CTRL = 0x32;  // GPIO control
  static constexpr uint8_t MSC_CTRL = 0x34;   // MISC control
  static constexpr uint8_t SMPL_PRD =
      0x36;  // Sample clock/Decimation filter control
  static constexpr uint8_t SENS_AVG = 0x38;    // Digital filter control
  static constexpr uint8_t SEQ_CNT = 0x3A;     // MAGN_OUT and BARO_OUT counter
  static constexpr uint8_t DIAG_STAT = 0x3C;   // System status
  static constexpr uint8_t GLOB_CMD = 0x3E;    // System command
  static constexpr uint8_t ALM_MAG1 = 0x40;    // Alarm 1 amplitude threshold
  static constexpr uint8_t ALM_MAG2 = 0x42;    // Alarm 2 amplitude threshold
  static constexpr uint8_t ALM_SMPL1 = 0x44;   // Alarm 1 sample size
  static constexpr uint8_t ALM_SMPL2 = 0x46;   // Alarm 2 sample size
  static constexpr uint8_t ALM_CTRL = 0x48;    // Alarm control
  static constexpr uint8_t LOT_ID1 = 0x52;     // Lot identification number
  static constexpr uint8_t LOT_ID2 = 0x54;     // Lot identification number
  static constexpr uint8_t PROD_ID = 0x56;     // Product identifier
  static constexpr uint8_t SERIAL_NUM = 0x58;  // Lot-specific serial number

  /** @brief ADIS16448 Static Constants */
  static constexpr double rad_to_deg = 57.2957795;
  static constexpr double deg_to_rad = 0.0174532;
  static constexpr double grav = 9.81;

  /** @brief struct to store offset data */
  struct OffsetData {
    double gyro_rate_x = 0.0;
    double gyro_rate_y = 0.0;
    double gyro_rate_z = 0.0;
  };

  /** @brief Internal Resources **/
  DigitalInput* m_reset_in = nullptr;
  DigitalOutput* m_status_led = nullptr;

  bool SwitchToStandardSPI();

  bool SwitchToAutoSPI();

  uint16_t ReadRegister(uint8_t reg);

  void WriteRegister(uint8_t reg, uint16_t val);

  void Acquire();

  void Close();

  // User-specified yaw axis
  IMUAxis m_yaw_axis;

  // Last read values (post-scaling)
  double m_gyro_rate_x = 0.0;
  double m_gyro_rate_y = 0.0;
  double m_gyro_rate_z = 0.0;
  double m_accel_x = 0.0;
  double m_accel_y = 0.0;
  double m_accel_z = 0.0;
  double m_mag_x = 0.0;
  double m_mag_y = 0.0;
  double m_mag_z = 0.0;
  double m_baro = 0.0;
  double m_temp = 0.0;

  // Complementary filter variables
  double m_tau = 0.5;
  double m_dt, m_alpha = 0.0;
  double m_compAngleX, m_compAngleY, m_accelAngleX, m_accelAngleY = 0.0;

  // vector for storing most recent imu values
  OffsetData* m_offset_buffer = nullptr;

  double m_gyro_rate_offset_x = 0.0;
  double m_gyro_rate_offset_y = 0.0;
  double m_gyro_rate_offset_z = 0.0;

  // function to re-init offset buffer
  void InitOffsetBuffer(int size);

  // Accumulated gyro values (for offset calculation)
  int m_avg_size = 0;
  int m_accum_count = 0;

  // Integrated gyro values
  double m_integ_gyro_angle_x = 0.0;
  double m_integ_gyro_angle_y = 0.0;
  double m_integ_gyro_angle_z = 0.0;

  // Complementary filter functions
  double FormatFastConverge(double compAngle, double accAngle);

  double FormatRange0to2PI(double compAngle);

  double FormatAccelRange(double accelAngle, double accelZ);

  double CompFilterProcess(double compAngle, double accelAngle, double omega);

  // State and resource variables
  std::atomic<bool> m_thread_active = false;
  std::atomic<bool> m_first_run = true;
  std::atomic<bool> m_thread_idle = false;
  std::atomic<bool> m_start_up_mode = true;

  bool m_auto_configured = false;
  SPI::Port m_spi_port;
  CalibrationTime m_calibration_time{0};
  SPI* m_spi = nullptr;
  DigitalInput* m_auto_interrupt = nullptr;
  bool m_connected{false};

  std::thread m_acquire_task;

  hal::SimDevice m_simDevice;
  hal::SimBoolean m_simConnected;
  hal::SimDouble m_simGyroAngleX;
  hal::SimDouble m_simGyroAngleY;
  hal::SimDouble m_simGyroAngleZ;
  hal::SimDouble m_simGyroRateX;
  hal::SimDouble m_simGyroRateY;
  hal::SimDouble m_simGyroRateZ;
  hal::SimDouble m_simAccelX;
  hal::SimDouble m_simAccelY;
  hal::SimDouble m_simAccelZ;

  struct NonMovableMutexWrapper {
    wpi::mutex mutex;
    NonMovableMutexWrapper() = default;

    NonMovableMutexWrapper(const NonMovableMutexWrapper&) = delete;
    NonMovableMutexWrapper& operator=(const NonMovableMutexWrapper&) = delete;

    NonMovableMutexWrapper(NonMovableMutexWrapper&&) {}
    NonMovableMutexWrapper& operator=(NonMovableMutexWrapper&&) {
      return *this;
    }

    void lock() { mutex.lock(); }

    void unlock() { mutex.unlock(); }

    bool try_lock() noexcept { return mutex.try_lock(); }
  };

  mutable NonMovableMutexWrapper m_mutex;

  // CRC-16 Look-Up Table
  static constexpr uint16_t adiscrc[256] = {
      0x0000, 0x17CE, 0x0FDF, 0x1811, 0x1FBE, 0x0870, 0x1061, 0x07AF, 0x1F3F,
      0x08F1, 0x10E0, 0x072E, 0x0081, 0x174F, 0x0F5E, 0x1890, 0x1E3D, 0x09F3,
      0x11E2, 0x062C, 0x0183, 0x164D, 0x0E5C, 0x1992, 0x0102, 0x16CC, 0x0EDD,
      0x1913, 0x1EBC, 0x0972, 0x1163, 0x06AD, 0x1C39, 0x0BF7, 0x13E6, 0x0428,
      0x0387, 0x1449, 0x0C58, 0x1B96, 0x0306, 0x14C8, 0x0CD9, 0x1B17, 0x1CB8,
      0x0B76, 0x1367, 0x04A9, 0x0204, 0x15CA, 0x0DDB, 0x1A15, 0x1DBA, 0x0A74,
      0x1265, 0x05AB, 0x1D3B, 0x0AF5, 0x12E4, 0x052A, 0x0285, 0x154B, 0x0D5A,
      0x1A94, 0x1831, 0x0FFF, 0x17EE, 0x0020, 0x078F, 0x1041, 0x0850, 0x1F9E,
      0x070E, 0x10C0, 0x08D1, 0x1F1F, 0x18B0, 0x0F7E, 0x176F, 0x00A1, 0x060C,
      0x11C2, 0x09D3, 0x1E1D, 0x19B2, 0x0E7C, 0x166D, 0x01A3, 0x1933, 0x0EFD,
      0x16EC, 0x0122, 0x068D, 0x1143, 0x0952, 0x1E9C, 0x0408, 0x13C6, 0x0BD7,
      0x1C19, 0x1BB6, 0x0C78, 0x1469, 0x03A7, 0x1B37, 0x0CF9, 0x14E8, 0x0326,
      0x0489, 0x1347, 0x0B56, 0x1C98, 0x1A35, 0x0DFB, 0x15EA, 0x0224, 0x058B,
      0x1245, 0x0A54, 0x1D9A, 0x050A, 0x12C4, 0x0AD5, 0x1D1B, 0x1AB4, 0x0D7A,
      0x156B, 0x02A5, 0x1021, 0x07EF, 0x1FFE, 0x0830, 0x0F9F, 0x1851, 0x0040,
      0x178E, 0x0F1E, 0x18D0, 0x00C1, 0x170F, 0x10A0, 0x076E, 0x1F7F, 0x08B1,
      0x0E1C, 0x19D2, 0x01C3, 0x160D, 0x11A2, 0x066C, 0x1E7D, 0x09B3, 0x1123,
      0x06ED, 0x1EFC, 0x0932, 0x0E9D, 0x1953, 0x0142, 0x168C, 0x0C18, 0x1BD6,
      0x03C7, 0x1409, 0x13A6, 0x0468, 0x1C79, 0x0BB7, 0x1327, 0x04E9, 0x1CF8,
      0x0B36, 0x0C99, 0x1B57, 0x0346, 0x1488, 0x1225, 0x05EB, 0x1DFA, 0x0A34,
      0x0D9B, 0x1A55, 0x0244, 0x158A, 0x0D1A, 0x1AD4, 0x02C5, 0x150B, 0x12A4,
      0x056A, 0x1D7B, 0x0AB5, 0x0810, 0x1FDE, 0x07CF, 0x1001, 0x17AE, 0x0060,
      0x1871, 0x0FBF, 0x172F, 0x00E1, 0x18F0, 0x0F3E, 0x0891, 0x1F5F, 0x074E,
      0x1080, 0x162D, 0x01E3, 0x19F2, 0x0E3C, 0x0993, 0x1E5D, 0x064C, 0x1182,
      0x0912, 0x1EDC, 0x06CD, 0x1103, 0x16AC, 0x0162, 0x1973, 0x0EBD, 0x1429,
      0x03E7, 0x1BF6, 0x0C38, 0x0B97, 0x1C59, 0x0448, 0x1386, 0x0B16, 0x1CD8,
      0x04C9, 0x1307, 0x14A8, 0x0366, 0x1B77, 0x0CB9, 0x0A14, 0x1DDA, 0x05CB,
      0x1205, 0x15AA, 0x0264, 0x1A75, 0x0DBB, 0x152B, 0x02E5, 0x1AF4, 0x0D3A,
      0x0A95, 0x1D5B, 0x054A, 0x1284};
};

}  // namespace frc
