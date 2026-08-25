// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//
// This file is based on the goBILDA FTC driver, which was made available under
// the MIT License. Copyright (c) 2025 Base 10 Assets, LLC.

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "wpi/hardware/bus/I2C.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Quaternion.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/frequency.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"

namespace wpi {

/**
 * Driver for the goBILDA Pinpoint Odometry Computer.
 *
 * Aborted I2C transactions and invalid read data are reported through
 * DeviceStatus::FAULT_BAD_READ instead of throwing an exception.
 */
class GoBildaPinpoint {
 public:
  /// Default 7-bit I2C address of the Pinpoint.
  static constexpr int DEFAULT_ADDRESS = 0x31;

  /** Error detection method used for I2C reads. */
  enum class ErrorDetectionType {
    /// Do not validate data read from the device.
    NONE,
    /// Validate the CRC-8 byte returned by v3 or newer firmware.
    CRC,
    /// Reject nonfinite values, invalid quaternions, and implausibly large
    /// changes locally.
    LOCAL_TEST
  };

  /** Reason the driver rejected data or an I2C transaction. */
  enum class FailureReason {
    /// The I2C controller aborted a read transaction.
    I2C_READ_ABORTED,
    /// The I2C controller aborted a write transaction.
    I2C_WRITE_ABORTED,
    /// Data returned by the device did not have the expected CRC-8 value.
    CRC_MISMATCH,
    /// A bulk read returned a nonpositive device loop time.
    INVALID_LOOP_TIME,
    /// A floating-point register returned a nonfinite value.
    NONFINITE_VALUE,
    /// A quaternion sample had a zero or near-zero norm.
    INVALID_QUATERNION,
    /// A position or orientation changed by more than the validation limit.
    CHANGE_TOO_LARGE,
    /// A velocity magnitude exceeded the local validation limit.
    VELOCITY_TOO_LARGE
  };

  /** Register map for the Pinpoint. */
  enum class Register {
    /// Device identifier.
    DEVICE_ID = 1,
    /// Firmware version.
    DEVICE_VERSION = 2,
    /// Device status bit field.
    DEVICE_STATUS = 3,
    /// Device control command.
    DEVICE_CONTROL = 4,
    /// Device loop time.
    LOOP_TIME = 5,
    /// Raw X encoder value.
    X_ENCODER_VALUE = 6,
    /// Raw Y encoder value.
    Y_ENCODER_VALUE = 7,
    /// X position in millimeters.
    X_POSITION = 8,
    /// Y position in millimeters.
    Y_POSITION = 9,
    /// Heading in radians.
    H_ORIENTATION = 10,
    /// X velocity in millimeters per second.
    X_VELOCITY = 11,
    /// Y velocity in millimeters per second.
    Y_VELOCITY = 12,
    /// Heading velocity in radians per second.
    H_VELOCITY = 13,
    /// Encoder ticks per millimeter traveled.
    MM_PER_TICK = 14,
    /// X pod offset in millimeters.
    X_POD_OFFSET = 15,
    /// Y pod offset in millimeters.
    Y_POD_OFFSET = 16,
    /// IMU yaw scalar.
    YAW_SCALAR = 17,
    /// Bulk-read window.
    BULK_READ = 18,
    /// Quaternion W component.
    QUATERNION_W = 19,
    /// Quaternion X component.
    QUATERNION_X = 20,
    /// Quaternion Y component.
    QUATERNION_Y = 21,
    /// Quaternion Z component.
    QUATERNION_Z = 22,
    /// Pitch in radians.
    PITCH = 23,
    /// Roll in radians.
    ROLL = 24,
    /// Flexible bulk-read configuration.
    SET_BULK_READ = 25
  };

  /** Pinpoint status, including device-reported faults and local read failures.
   */
  enum class DeviceStatus {
    /// The device is starting up and has not initialized.
    NOT_READY = 0,
    /// The device is operating normally.
    READY = 1,
    /// The device is calibrating its IMU.
    CALIBRATING = 1 << 1,
    /// The X odometry pod was not detected.
    FAULT_X_POD_NOT_DETECTED = 1 << 2,
    /// The Y odometry pod was not detected.
    FAULT_Y_POD_NOT_DETECTED = 1 << 3,
    /// Neither odometry pod was detected.
    FAULT_NO_PODS_DETECTED = (1 << 2) | (1 << 3),
    /// The IMU reported a runaway condition.
    FAULT_IMU_RUNAWAY = 1 << 4,
    /// A read failed validation or the I2C transaction was aborted.
    FAULT_BAD_READ = 1 << 5
  };

  /** Encoder direction relative to the Pinpoint coordinate system. */
  enum class EncoderDirection {
    /// Encoder count increases in the positive axis direction.
    FORWARD,
    /// Encoder count decreases in the positive axis direction.
    REVERSED
  };

  /** goBILDA odometry pod with a predefined encoder resolution. */
  enum class OdometryPod {
    /// goBILDA Swingarm Odometry Pod.
    SWINGARM,
    /// goBILDA 4-Bar Odometry Pod.
    FOUR_BAR
  };

  /**
   * Constructs a Pinpoint.
   *
   * @param port I2C port to which the Pinpoint is connected.
   * @param deviceAddress 7-bit I2C address.
   * @throws std::invalid_argument if deviceAddress is outside the 7-bit range.
   */
  explicit GoBildaPinpoint(I2C::Port port, int deviceAddress = DEFAULT_ADDRESS);

  GoBildaPinpoint(GoBildaPinpoint&&) = default;
  GoBildaPinpoint& operator=(GoBildaPinpoint&&) = default;

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
   * Reads all registers in the configured bulk-read scope and updates the
   * cached measurements. Call this once per robot loop.
   *
   * An aborted or invalid I2C read does not throw; it sets the device status
   * to DeviceStatus::FAULT_BAD_READ and preserves the last valid measurements.
   *
   * @throws std::runtime_error if CRC error detection is active with firmware
   *     older than v3.
   */
  void Update();

  /**
   * Reads and caches only the heading register.
   *
   * An aborted or invalid I2C read does not throw; it sets the device status
   * to DeviceStatus::FAULT_BAD_READ and preserves the last valid heading.
   */
  void UpdateHeading();

  /**
   * Configures the registers returned by a bulk read on v3 or newer firmware.
   * Duplicate registers are ignored while preserving their first occurrence.
   *
   * @param registers Readable registers to include.
   * @throws std::runtime_error if the firmware is older than v3.
   * @throws std::invalid_argument if no registers are provided or a register
   *     is not readable.
   */
  void SetBulkReadScope(const std::vector<Register>& registers);

  /**
   * Selects the read error detection method. Selecting CRC reads the firmware
   * version and requires v3 or newer firmware.
   *
   * @param errorDetectionType Error detection method.
   * @throws std::runtime_error if CRC is selected with firmware older than v3.
   * @throws std::invalid_argument if errorDetectionType is invalid.
   */
  void SetErrorDetectionType(ErrorDetectionType errorDetectionType);

  /**
   * Sets the odometry pod offsets.
   *
   * The X pod offset is positive to the left of the tracking point. The Y pod
   * offset is positive in front of the tracking point.
   *
   * @param xOffset X pod offset.
   * @param yOffset Y pod offset.
   * @throws std::invalid_argument if either offset is nonfinite or cannot be
   *     represented by the device's 32-bit floating-point register.
   */
  void SetOffsets(wpi::units::meters<> xOffset, wpi::units::meters<> yOffset);

  /** Recalibrates the IMU. The robot must remain stationary for 0.25 seconds.
   */
  void RecalibrateIMU();

  /** Resets the pose to the origin and recalibrates the stationary IMU. */
  void ResetPositionAndIMU();

  /**
   * Sets the directions of both odometry pod encoders.
   *
   * @param xEncoder X (forward) pod direction.
   * @param yEncoder Y (left) pod direction.
   * @throws std::invalid_argument if either direction is invalid.
   */
  void SetEncoderDirections(EncoderDirection xEncoder,
                            EncoderDirection yEncoder);

  /**
   * Sets the encoder resolution for a goBILDA odometry pod.
   *
   * @param pod Odometry pod type.
   * @throws std::invalid_argument if pod is invalid.
   */
  void SetEncoderResolution(OdometryPod pod);

  /**
   * Sets a custom odometry pod encoder resolution.
   *
   * @param ticksPerMeter Encoder ticks per meter of pod travel.
   * @throws std::invalid_argument if ticksPerMeter is nonfinite, not positive,
   *     or does not produce a positive ticks-per-millimeter value representable
   *     by a 32-bit float.
   */
  void SetEncoderResolution(double ticksPerMeter);

  /**
   * Sets the scalar applied to the IMU yaw measurement.
   *
   * @param yawScalar Yaw scalar.
   * @throws std::invalid_argument if yawScalar is nonfinite or cannot be
   *     represented by a 32-bit float.
   */
  void SetYawScalar(double yawScalar);

  /**
   * Overrides the pose tracked by the Pinpoint.
   *
   * @param pose Pose to set.
   * @throws std::invalid_argument if a pose component is nonfinite or cannot
   *     be represented by a 32-bit float.
   */
  void SetPose(const wpi::math::Pose2d& pose);

  /**
   * Overrides the tracked X position.
   *
   * @param position X position.
   * @throws std::invalid_argument if position is nonfinite or cannot be
   *     represented by a 32-bit float.
   */
  void SetXPosition(wpi::units::meters<> position);

  /**
   * Overrides the tracked Y position.
   *
   * @param position Y position.
   * @throws std::invalid_argument if position is nonfinite or cannot be
   *     represented by a 32-bit float.
   */
  void SetYPosition(wpi::units::meters<> position);

  /**
   * Overrides the tracked heading.
   *
   * @param heading Heading.
   * @throws std::invalid_argument if heading is nonfinite or cannot be
   *     represented by a 32-bit float.
   */
  void SetHeading(wpi::units::radians<> heading);

  /** @return Device identifier. */
  int32_t GetDeviceId();

  /** @return Firmware version. */
  int32_t GetDeviceVersion();

  /** @return Configured yaw scalar. */
  double GetYawScalar();

  /**
   * @return Encoder ticks per meter.
   */
  double GetEncoderResolution();

  /** @return Highest-priority current device or read status. */
  DeviceStatus GetDeviceStatus() const;

  /**
   * @return Cached device status bits combined with the local bad-read status
   *     bit, if set.
   */
  int32_t GetDeviceStatusBits() const;

  /**
   * @return Register associated with the most recent driver-detected failure,
   *     or std::nullopt if none has occurred.
   */
  std::optional<Register> GetLastFailedRegister() const;

  /**
   * @return Reason for the most recent driver-detected failure, or std::nullopt
   *     if none has occurred.
   */
  std::optional<FailureReason> GetLastFailureReason() const;

  /** @return Total number of failures detected by this driver instance. */
  uint64_t GetFailureCount() const;

  /**
   * Returns the number of failures associated with a register.
   *
   * @param reg Register whose failure count should be returned.
   * @return Failure count for the register.
   * @throws std::invalid_argument if reg is invalid.
   */
  uint64_t GetFailureCount(Register reg) const;

  /** @return Most recently reported device loop time in microseconds. */
  int32_t GetLoopTimeMicroseconds();

  /** @return Device loop frequency, or zero if no loop time has been read. */
  wpi::units::hertz<> GetFrequency();

  /** @return Raw X encoder count in ticks. */
  int32_t GetXEncoder();

  /** @return Raw Y encoder count in ticks. */
  int32_t GetYEncoder();

  /** @return Tracked X position. */
  wpi::units::meters<> GetXPosition();

  /** @return Tracked Y position. */
  wpi::units::meters<> GetYPosition();

  /** @return Continuous tracked heading, not constrained to one rotation. */
  wpi::units::radians<> GetHeading();

  /** @return Tracked X velocity. */
  wpi::units::meters_per_second<> GetXVelocity();

  /** @return Tracked Y velocity. */
  wpi::units::meters_per_second<> GetYVelocity();

  /** @return Heading velocity. */
  wpi::units::radians_per_second<> GetHeadingVelocity();

  /** @return X pod offset. */
  wpi::units::meters<> GetXOffset();

  /** @return Y pod offset. */
  wpi::units::meters<> GetYOffset();

  /**
   * Returns the tracked pose. The heading is normalized by Rotation2d.
   *
   * If the configured bulk-read scope does not contain all three pose
   * registers, this reads X, Y, and heading together so the returned components
   * are from the same device snapshot.
   *
   * @return Tracked pose.
   */
  wpi::math::Pose2d GetPose();

  /**
   * Returns the device orientation quaternion.
   *
   * If the configured bulk-read scope does not contain all four quaternion
   * registers, this reads all four components together so they come from the
   * same device snapshot.
   *
   * @return Orientation quaternion.
   * @throws std::runtime_error if the firmware is older than v3.
   */
  wpi::math::Quaternion GetQuaternion();

  /**
   * Returns the device orientation as a 3D rotation.
   *
   * @return 3D orientation.
   * @throws std::runtime_error if the firmware is older than v3.
   */
  wpi::math::Rotation3d GetRotation3d();

  /**
   * Returns the pitch.
   *
   * @return Pitch.
   * @throws std::runtime_error if the firmware is older than v3.
   */
  wpi::units::radians<> GetPitch();

  /**
   * Returns the roll.
   *
   * @return Roll.
   * @throws std::runtime_error if the firmware is older than v3.
   */
  wpi::units::radians<> GetRoll();

 private:
  enum class RegisterType { INT32, FLOAT, BULK };

  enum class DeviceControl : int32_t {
    RECALIBRATE_IMU = 1 << 0,
    RESET_POSITION_AND_IMU = 1 << 1,
    SET_Y_ENCODER_REVERSED = 1 << 2,
    SET_Y_ENCODER_FORWARD = 1 << 3,
    SET_X_ENCODER_REVERSED = 1 << 4,
    SET_X_ENCODER_FORWARD = 1 << 5
  };

  static constexpr int REGISTER_LENGTH = 4;
  static constexpr int FIXED_BULK_READ_LENGTH = 40;
  static constexpr int CRC_LENGTH = 1;
  static constexpr uint8_t CRC_INITIAL_VALUE = 0x90;
  static constexpr uint8_t CRC_POLYNOMIAL_VALUE = 0x31;
  static constexpr double SWINGARM_TICKS_PER_METER = 13262.91192;
  static constexpr double FOUR_BAR_TICKS_PER_METER = 19894.36789;
  static constexpr float POSITION_CHANGE_LIMIT_MILLIMETERS = 5000.0f;
  static constexpr float HEADING_CHANGE_LIMIT_RADIANS = 120.0f;
  static constexpr float VELOCITY_LIMIT_MILLIMETERS_PER_SECOND = 10000.0f;
  static constexpr float HEADING_VELOCITY_LIMIT_RADIANS_PER_SECOND = 120.0f;
  static constexpr double MIN_QUATERNION_NORM_SQUARED = 1e-12;
  static constexpr std::array<Register, 10> DEFAULT_BULK_READ_SCOPE = {
      Register::DEVICE_STATUS,   Register::LOOP_TIME,
      Register::X_ENCODER_VALUE, Register::Y_ENCODER_VALUE,
      Register::X_POSITION,      Register::Y_POSITION,
      Register::H_ORIENTATION,   Register::X_VELOCITY,
      Register::Y_VELOCITY,      Register::H_VELOCITY};

  static int ValidateAddress(int deviceAddress);
  static float MetersToMillimeters(wpi::units::meters<> meters,
                                   const char* parameterName);
  static float RequireFiniteFloat(double value, const char* parameterName);
  bool RequireFirmwareVersion3(const char* feature);
  bool SynchronizeBulkReadScope();
  void ReadIfNotInBulkScope(Register reg);
  bool BulkReadScopeContainsPose() const;
  bool BulkReadScopeContainsQuaternion() const;
  static bool IsIndividuallyReadable(Register reg);
  static RegisterType GetRegisterType(Register reg);
  static std::size_t RegisterIndex(Register reg);
  bool WriteInt(Register reg, int32_t value);
  bool WriteFloat(Register reg, float value);
  bool WriteBytes(Register reg, const std::vector<uint8_t>& data);
  std::vector<uint8_t> ReadBytes(Register reg, int count);
  bool ReadRegister(Register reg, bool clearPreviousBadRead = false);
  void ReadPose();
  void ReadQuaternion();
  std::vector<uint8_t> ReadBulkSnapshot(const std::vector<Register>& registers);
  float DecodeBulkFloat(const std::vector<uint8_t>& data, Register reg) const;
  static std::vector<uint8_t> EncodeBulkReadScope(
      const std::vector<Register>& registers);
  void FixedBulkRead();
  void FlexibleBulkRead();
  void SaveInt(Register reg, int32_t value);
  void SavePose(float xPosition, float yPosition, float heading,
                bool bulkUpdate);
  void SaveQuaternion(float w, float x, float y, float z);
  void SaveFloat(Register reg, float value, bool bulkUpdate);
  bool ValidateFinite(Register reg, float value);
  std::optional<float> ValidatePosition(Register reg, float oldValue,
                                        float newValue, float changeLimit,
                                        bool haveOldValue, bool bulkUpdate);
  std::optional<float> ValidateVelocity(Register reg, float newValue,
                                        float magnitudeLimit, bool bulkUpdate);
  void RecordFailure(Register reg, FailureReason reason);
  void FinishRead(bool previousBadRead, bool clearPreviousBadRead);
  static DeviceStatus DecodeStatus(int32_t status);
  static std::vector<uint8_t> EncodeInt(int32_t value);
  static std::vector<uint8_t> EncodeFloat(float value);
  static int32_t DecodeInt(const std::vector<uint8_t>& data,
                           std::size_t offset);
  static float DecodeFloat(const std::vector<uint8_t>& data,
                           std::size_t offset);
  static bool CheckCrc(const std::vector<uint8_t>& data,
                       std::size_t dataLength);
  static uint8_t ComputeCrc8(const std::vector<uint8_t>& data,
                             std::size_t length);

  I2C m_i2c;
  std::vector<Register> m_bulkReadScope;
  bool m_bulkReadScopeSynchronized = false;
  ErrorDetectionType m_errorDetectionType = ErrorDetectionType::LOCAL_TEST;

  int32_t m_deviceId = 0;
  int32_t m_deviceVersion = 0;
  int32_t m_deviceStatusBits = 0;
  int32_t m_loopTimeMicroseconds = 0;
  int32_t m_xEncoderValue = 0;
  int32_t m_yEncoderValue = 0;
  float m_xPositionMillimeters = 0.0f;
  float m_yPositionMillimeters = 0.0f;
  float m_headingRadians = 0.0f;
  float m_xVelocityMillimetersPerSecond = 0.0f;
  float m_yVelocityMillimetersPerSecond = 0.0f;
  float m_headingVelocityRadiansPerSecond = 0.0f;
  float m_ticksPerMillimeter = 0.0f;
  float m_xPodOffsetMillimeters = 0.0f;
  float m_yPodOffsetMillimeters = 0.0f;
  float m_yawScalar = 0.0f;
  float m_quaternionW = 0.0f;
  float m_quaternionX = 0.0f;
  float m_quaternionY = 0.0f;
  float m_quaternionZ = 0.0f;
  float m_pitchRadians = 0.0f;
  float m_rollRadians = 0.0f;

  bool m_haveXPosition = false;
  bool m_haveYPosition = false;
  bool m_haveHeading = false;
  bool m_badReadDetected = false;
  std::array<uint64_t, 25> m_failureCounts{};
  uint64_t m_failureCount = 0;
  std::optional<Register> m_lastFailedRegister;
  std::optional<FailureReason> m_lastFailureReason;
};

}  // namespace wpi
