// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//
// This file is based on the goBILDA FTC driver, which was made available under
// the MIT License. Copyright (c) 2025 Base 10 Assets, LLC.

#include "wpi/drivers/odometry/GoBildaPinpoint.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <format>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/util/UsageReporting.hpp"

using namespace wpi;

GoBildaPinpoint::GoBildaPinpoint(I2C::Port port, int deviceAddress)
    : m_i2c{port, ValidateAddress(deviceAddress)},
      m_bulkReadScope{DEFAULT_BULK_READ_SCOPE.begin(),
                      DEFAULT_BULK_READ_SCOPE.end()} {
  wpi::util::ReportUsage(std::format("I2C[{}]", static_cast<int>(port)),
                         deviceAddress, "GoBildaPinpoint");
}

I2C::Port GoBildaPinpoint::GetPort() const {
  return m_i2c.GetPort();
}

int GoBildaPinpoint::GetDeviceAddress() const {
  return m_i2c.GetDeviceAddress();
}

void GoBildaPinpoint::Update() {
  if (m_deviceVersion == 0 && !ReadRegister(Register::DEVICE_VERSION)) {
    return;
  }

  if (m_deviceVersion == 1 || m_deviceVersion == 2) {
    FixedBulkRead();
  } else if (m_deviceVersion >= 3) {
    if (!SynchronizeBulkReadScope()) {
      return;
    }
    FlexibleBulkRead();
  }
}

void GoBildaPinpoint::UpdateHeading() {
  ReadRegister(Register::H_ORIENTATION, true);
}

void GoBildaPinpoint::SetBulkReadScope(const std::vector<Register>& registers) {
  if (!RequireFirmwareVersion3("Flexible bulk reads")) {
    return;
  }

  std::vector<Register> scope;
  scope.reserve(registers.size());
  for (Register reg : registers) {
    if (!IsIndividuallyReadable(reg)) {
      throw std::invalid_argument(
          "Bulk-read scope contains an unreadable register");
    }
    if (std::find(scope.begin(), scope.end(), reg) == scope.end()) {
      scope.push_back(reg);
    }
  }
  if (scope.empty()) {
    throw std::invalid_argument("At least one bulk-read register is required");
  }

  if (WriteBytes(Register::SET_BULK_READ, EncodeBulkReadScope(scope))) {
    m_bulkReadScope = std::move(scope);
    m_bulkReadScopeSynchronized = true;
  }
}

void GoBildaPinpoint::SetErrorDetectionType(
    ErrorDetectionType errorDetectionType) {
  switch (errorDetectionType) {
    case ErrorDetectionType::NONE:
    case ErrorDetectionType::LOCAL_TEST:
      break;
    case ErrorDetectionType::CRC:
      if (!RequireFirmwareVersion3("CRC error detection")) {
        return;
      }
      break;
    default:
      throw std::invalid_argument("Invalid error detection type");
  }
  if (errorDetectionType == ErrorDetectionType::LOCAL_TEST &&
      m_errorDetectionType != ErrorDetectionType::LOCAL_TEST) {
    m_haveXPosition = false;
    m_haveYPosition = false;
    m_haveHeading = false;
  }
  m_errorDetectionType = errorDetectionType;
}

void GoBildaPinpoint::SetOffsets(wpi::units::meters<> xOffset,
                                 wpi::units::meters<> yOffset) {
  float xOffsetMillimeters = MetersToMillimeters(xOffset, "xOffset");
  float yOffsetMillimeters = MetersToMillimeters(yOffset, "yOffset");
  WriteFloat(Register::X_POD_OFFSET, xOffsetMillimeters);
  WriteFloat(Register::Y_POD_OFFSET, yOffsetMillimeters);
}

void GoBildaPinpoint::RecalibrateIMU() {
  WriteInt(Register::DEVICE_CONTROL,
           static_cast<int32_t>(DeviceControl::RECALIBRATE_IMU));
}

void GoBildaPinpoint::ResetPositionAndIMU() {
  if (WriteInt(Register::DEVICE_CONTROL,
               static_cast<int32_t>(DeviceControl::RESET_POSITION_AND_IMU))) {
    m_haveXPosition = false;
    m_haveYPosition = false;
    m_haveHeading = false;
  }
}

void GoBildaPinpoint::SetEncoderDirections(EncoderDirection xEncoder,
                                           EncoderDirection yEncoder) {
  DeviceControl xControl;
  switch (xEncoder) {
    case EncoderDirection::FORWARD:
      xControl = DeviceControl::SET_X_ENCODER_FORWARD;
      break;
    case EncoderDirection::REVERSED:
      xControl = DeviceControl::SET_X_ENCODER_REVERSED;
      break;
    default:
      throw std::invalid_argument("Invalid X encoder direction");
  }

  DeviceControl yControl;
  switch (yEncoder) {
    case EncoderDirection::FORWARD:
      yControl = DeviceControl::SET_Y_ENCODER_FORWARD;
      break;
    case EncoderDirection::REVERSED:
      yControl = DeviceControl::SET_Y_ENCODER_REVERSED;
      break;
    default:
      throw std::invalid_argument("Invalid Y encoder direction");
  }

  WriteInt(Register::DEVICE_CONTROL, static_cast<int32_t>(xControl));
  WriteInt(Register::DEVICE_CONTROL, static_cast<int32_t>(yControl));
}

void GoBildaPinpoint::SetEncoderResolution(OdometryPod pod) {
  switch (pod) {
    case OdometryPod::SWINGARM:
      SetEncoderResolution(SWINGARM_TICKS_PER_METER);
      break;
    case OdometryPod::FOUR_BAR:
      SetEncoderResolution(FOUR_BAR_TICKS_PER_METER);
      break;
    default:
      throw std::invalid_argument("Invalid odometry pod type");
  }
}

void GoBildaPinpoint::SetEncoderResolution(double ticksPerMeter) {
  if (!std::isfinite(ticksPerMeter) || ticksPerMeter <= 0.0) {
    throw std::invalid_argument(
        "ticksPerMeter must be finite and greater than zero");
  }
  float ticksPerMillimeter =
      RequireFiniteFloat(ticksPerMeter / 1000.0, "ticks per millimeter");
  if (ticksPerMillimeter <= 0.0f) {
    throw std::invalid_argument(
        "ticksPerMeter must produce a positive 32-bit "
        "ticks-per-millimeter value");
  }
  WriteFloat(Register::MM_PER_TICK, ticksPerMillimeter);
}

void GoBildaPinpoint::SetYawScalar(double yawScalar) {
  WriteFloat(Register::YAW_SCALAR, RequireFiniteFloat(yawScalar, "yawScalar"));
}

void GoBildaPinpoint::SetPose(const wpi::math::Pose2d& pose) {
  float xMillimeters = MetersToMillimeters(pose.X(), "pose X");
  float yMillimeters = MetersToMillimeters(pose.Y(), "pose Y");
  float headingRadians =
      RequireFiniteFloat(pose.Rotation().Radians().value(), "pose heading");

  if (WriteFloat(Register::X_POSITION, xMillimeters)) {
    m_haveXPosition = false;
  }
  if (WriteFloat(Register::Y_POSITION, yMillimeters)) {
    m_haveYPosition = false;
  }
  if (WriteFloat(Register::H_ORIENTATION, headingRadians)) {
    m_haveHeading = false;
  }
}

void GoBildaPinpoint::SetXPosition(wpi::units::meters<> position) {
  if (WriteFloat(Register::X_POSITION,
                 MetersToMillimeters(position, "position"))) {
    m_haveXPosition = false;
  }
}

void GoBildaPinpoint::SetYPosition(wpi::units::meters<> position) {
  if (WriteFloat(Register::Y_POSITION,
                 MetersToMillimeters(position, "position"))) {
    m_haveYPosition = false;
  }
}

void GoBildaPinpoint::SetHeading(wpi::units::radians<> heading) {
  if (WriteFloat(Register::H_ORIENTATION,
                 RequireFiniteFloat(heading.value(), "heading"))) {
    m_haveHeading = false;
  }
}

int32_t GoBildaPinpoint::GetDeviceId() {
  ReadIfNotInBulkScope(Register::DEVICE_ID);
  return m_deviceId;
}

int32_t GoBildaPinpoint::GetDeviceVersion() {
  if (m_deviceVersion == 0) {
    ReadRegister(Register::DEVICE_VERSION);
  }
  return m_deviceVersion;
}

double GoBildaPinpoint::GetYawScalar() {
  ReadIfNotInBulkScope(Register::YAW_SCALAR);
  return m_yawScalar;
}

double GoBildaPinpoint::GetEncoderResolution() {
  ReadIfNotInBulkScope(Register::MM_PER_TICK);
  return 1000.0 * m_ticksPerMillimeter;
}

GoBildaPinpoint::DeviceStatus GoBildaPinpoint::GetDeviceStatus() const {
  return m_badReadDetected ? DeviceStatus::FAULT_BAD_READ
                           : DecodeStatus(m_deviceStatusBits);
}

int32_t GoBildaPinpoint::GetDeviceStatusBits() const {
  return m_deviceStatusBits |
         (m_badReadDetected ? static_cast<int32_t>(DeviceStatus::FAULT_BAD_READ)
                            : 0);
}

std::optional<GoBildaPinpoint::Register>
GoBildaPinpoint::GetLastFailedRegister() const {
  return m_lastFailedRegister;
}

std::optional<GoBildaPinpoint::FailureReason>
GoBildaPinpoint::GetLastFailureReason() const {
  return m_lastFailureReason;
}

uint64_t GoBildaPinpoint::GetFailureCount() const {
  return m_failureCount;
}

uint64_t GoBildaPinpoint::GetFailureCount(Register reg) const {
  return m_failureCounts[RegisterIndex(reg)];
}

int32_t GoBildaPinpoint::GetLoopTimeMicroseconds() {
  ReadIfNotInBulkScope(Register::LOOP_TIME);
  return m_loopTimeMicroseconds;
}

wpi::units::hertz<> GoBildaPinpoint::GetFrequency() {
  int32_t loopTime = GetLoopTimeMicroseconds();
  return wpi::units::hertz<>{loopTime == 0 ? 0.0 : 1000000.0 / loopTime};
}

int32_t GoBildaPinpoint::GetXEncoder() {
  ReadIfNotInBulkScope(Register::X_ENCODER_VALUE);
  return m_xEncoderValue;
}

int32_t GoBildaPinpoint::GetYEncoder() {
  ReadIfNotInBulkScope(Register::Y_ENCODER_VALUE);
  return m_yEncoderValue;
}

wpi::units::meters<> GoBildaPinpoint::GetXPosition() {
  ReadIfNotInBulkScope(Register::X_POSITION);
  return wpi::units::meters<>{m_xPositionMillimeters / 1000.0};
}

wpi::units::meters<> GoBildaPinpoint::GetYPosition() {
  ReadIfNotInBulkScope(Register::Y_POSITION);
  return wpi::units::meters<>{m_yPositionMillimeters / 1000.0};
}

wpi::units::radians<> GoBildaPinpoint::GetHeading() {
  ReadIfNotInBulkScope(Register::H_ORIENTATION);
  return wpi::units::radians<>{m_headingRadians};
}

wpi::units::meters_per_second<> GoBildaPinpoint::GetXVelocity() {
  ReadIfNotInBulkScope(Register::X_VELOCITY);
  return wpi::units::meters_per_second<>{m_xVelocityMillimetersPerSecond /
                                         1000.0};
}

wpi::units::meters_per_second<> GoBildaPinpoint::GetYVelocity() {
  ReadIfNotInBulkScope(Register::Y_VELOCITY);
  return wpi::units::meters_per_second<>{m_yVelocityMillimetersPerSecond /
                                         1000.0};
}

wpi::units::radians_per_second<> GoBildaPinpoint::GetHeadingVelocity() {
  ReadIfNotInBulkScope(Register::H_VELOCITY);
  return wpi::units::radians_per_second<>{m_headingVelocityRadiansPerSecond};
}

wpi::units::meters<> GoBildaPinpoint::GetXOffset() {
  ReadIfNotInBulkScope(Register::X_POD_OFFSET);
  return wpi::units::meters<>{m_xPodOffsetMillimeters / 1000.0};
}

wpi::units::meters<> GoBildaPinpoint::GetYOffset() {
  ReadIfNotInBulkScope(Register::Y_POD_OFFSET);
  return wpi::units::meters<>{m_yPodOffsetMillimeters / 1000.0};
}

wpi::math::Pose2d GoBildaPinpoint::GetPose() {
  if (!BulkReadScopeContainsPose()) {
    ReadPose();
  }
  return wpi::math::Pose2d{
      wpi::units::meters<>{m_xPositionMillimeters / 1000.0},
      wpi::units::meters<>{m_yPositionMillimeters / 1000.0},
      wpi::math::Rotation2d{wpi::units::radians<>{m_headingRadians}}};
}

wpi::math::Quaternion GoBildaPinpoint::GetQuaternion() {
  if (RequireFirmwareVersion3("Quaternion output") &&
      !BulkReadScopeContainsQuaternion()) {
    ReadQuaternion();
  }
  return wpi::math::Quaternion{m_quaternionW, m_quaternionX, m_quaternionY,
                               m_quaternionZ};
}

wpi::math::Rotation3d GoBildaPinpoint::GetRotation3d() {
  return wpi::math::Rotation3d{GetQuaternion()};
}

wpi::units::radians<> GoBildaPinpoint::GetPitch() {
  if (RequireFirmwareVersion3("Pitch output")) {
    ReadIfNotInBulkScope(Register::PITCH);
  }
  return wpi::units::radians<>{m_pitchRadians};
}

wpi::units::radians<> GoBildaPinpoint::GetRoll() {
  if (RequireFirmwareVersion3("Roll output")) {
    ReadIfNotInBulkScope(Register::ROLL);
  }
  return wpi::units::radians<>{m_rollRadians};
}

int GoBildaPinpoint::ValidateAddress(int deviceAddress) {
  if (deviceAddress < 0 || deviceAddress > 0x7f) {
    throw std::invalid_argument("deviceAddress must be a 7-bit I2C address");
  }
  return deviceAddress;
}

float GoBildaPinpoint::MetersToMillimeters(wpi::units::meters<> meters,
                                           const char* parameterName) {
  return RequireFiniteFloat(meters.value() * 1000.0, parameterName);
}

float GoBildaPinpoint::RequireFiniteFloat(double value,
                                          const char* parameterName) {
  float floatValue = static_cast<float>(value);
  if (!std::isfinite(floatValue)) {
    throw std::invalid_argument(std::string{parameterName} +
                                " must be finite and representable as a "
                                "32-bit float");
  }
  return floatValue;
}

bool GoBildaPinpoint::RequireFirmwareVersion3(const char* feature) {
  if (m_deviceVersion == 0 && !ReadRegister(Register::DEVICE_VERSION)) {
    return false;
  }
  if (m_deviceVersion < 3) {
    throw std::runtime_error(std::string{feature} +
                             " requires firmware version 3 or newer");
  }
  return true;
}

bool GoBildaPinpoint::SynchronizeBulkReadScope() {
  if (m_bulkReadScopeSynchronized) {
    return true;
  }
  if (!WriteBytes(Register::SET_BULK_READ,
                  EncodeBulkReadScope(m_bulkReadScope))) {
    return false;
  }
  m_bulkReadScopeSynchronized = true;
  return true;
}

void GoBildaPinpoint::ReadIfNotInBulkScope(Register reg) {
  if (std::find(m_bulkReadScope.begin(), m_bulkReadScope.end(), reg) ==
      m_bulkReadScope.end()) {
    ReadRegister(reg);
  }
}

bool GoBildaPinpoint::BulkReadScopeContainsPose() const {
  auto contains = [this](Register reg) {
    return std::find(m_bulkReadScope.begin(), m_bulkReadScope.end(), reg) !=
           m_bulkReadScope.end();
  };
  return contains(Register::X_POSITION) && contains(Register::Y_POSITION) &&
         contains(Register::H_ORIENTATION);
}

bool GoBildaPinpoint::BulkReadScopeContainsQuaternion() const {
  auto contains = [this](Register reg) {
    return std::find(m_bulkReadScope.begin(), m_bulkReadScope.end(), reg) !=
           m_bulkReadScope.end();
  };
  return contains(Register::QUATERNION_W) && contains(Register::QUATERNION_X) &&
         contains(Register::QUATERNION_Y) && contains(Register::QUATERNION_Z);
}

bool GoBildaPinpoint::IsIndividuallyReadable(Register reg) {
  RegisterIndex(reg);
  return reg != Register::DEVICE_CONTROL && reg != Register::BULK_READ &&
         reg != Register::SET_BULK_READ;
}

GoBildaPinpoint::RegisterType GoBildaPinpoint::GetRegisterType(Register reg) {
  switch (reg) {
    case Register::DEVICE_ID:
    case Register::DEVICE_VERSION:
    case Register::DEVICE_STATUS:
    case Register::DEVICE_CONTROL:
    case Register::LOOP_TIME:
    case Register::X_ENCODER_VALUE:
    case Register::Y_ENCODER_VALUE:
    case Register::SET_BULK_READ:
      return RegisterType::INT32;
    case Register::X_POSITION:
    case Register::Y_POSITION:
    case Register::H_ORIENTATION:
    case Register::X_VELOCITY:
    case Register::Y_VELOCITY:
    case Register::H_VELOCITY:
    case Register::MM_PER_TICK:
    case Register::X_POD_OFFSET:
    case Register::Y_POD_OFFSET:
    case Register::YAW_SCALAR:
    case Register::QUATERNION_W:
    case Register::QUATERNION_X:
    case Register::QUATERNION_Y:
    case Register::QUATERNION_Z:
    case Register::PITCH:
    case Register::ROLL:
      return RegisterType::FLOAT;
    case Register::BULK_READ:
      return RegisterType::BULK;
    default:
      throw std::invalid_argument("Invalid Pinpoint register");
  }
}

std::size_t GoBildaPinpoint::RegisterIndex(Register reg) {
  int address = static_cast<int>(reg);
  if (address < static_cast<int>(Register::DEVICE_ID) ||
      address > static_cast<int>(Register::SET_BULK_READ)) {
    throw std::invalid_argument("Invalid Pinpoint register");
  }
  return static_cast<std::size_t>(address - 1);
}

bool GoBildaPinpoint::WriteInt(Register reg, int32_t value) {
  return WriteBytes(reg, EncodeInt(value));
}

bool GoBildaPinpoint::WriteFloat(Register reg, float value) {
  return WriteBytes(reg, EncodeFloat(value));
}

bool GoBildaPinpoint::WriteBytes(Register reg,
                                 const std::vector<uint8_t>& data) {
  std::vector<uint8_t> output(data.size() + 1);
  output[0] = static_cast<uint8_t>(reg);
  std::copy(data.begin(), data.end(), output.begin() + 1);
  if (m_i2c.WriteBulk(output.data(), static_cast<int>(output.size()))) {
    RecordFailure(reg, FailureReason::I2C_WRITE_ABORTED);
    return false;
  }
  return true;
}

std::vector<uint8_t> GoBildaPinpoint::ReadBytes(Register reg, int count) {
  uint8_t address = static_cast<uint8_t>(reg);
  if (m_i2c.WriteBulk(&address, 1)) {
    RecordFailure(reg, FailureReason::I2C_WRITE_ABORTED);
    return {};
  }

  std::vector<uint8_t> data(count);
  if (m_i2c.ReadOnly(count, data.data())) {
    RecordFailure(reg, FailureReason::I2C_READ_ABORTED);
    return {};
  }
  return data;
}

bool GoBildaPinpoint::ReadRegister(Register reg, bool clearPreviousBadRead) {
  if (!IsIndividuallyReadable(reg)) {
    throw std::invalid_argument("Register cannot be read individually");
  }

  bool previousBadRead = m_badReadDetected;
  m_badReadDetected = false;
  int readLength = REGISTER_LENGTH;
  if (m_errorDetectionType == ErrorDetectionType::CRC) {
    readLength += CRC_LENGTH;
  }
  std::vector<uint8_t> data = ReadBytes(reg, readLength);
  if (data.empty()) {
    return false;
  }
  if (m_errorDetectionType == ErrorDetectionType::CRC &&
      !CheckCrc(data, REGISTER_LENGTH)) {
    RecordFailure(reg, FailureReason::CRC_MISMATCH);
    return false;
  }

  switch (GetRegisterType(reg)) {
    case RegisterType::INT32:
      SaveInt(reg, DecodeInt(data, 0));
      break;
    case RegisterType::FLOAT:
      SaveFloat(reg, DecodeFloat(data, 0), false);
      break;
    case RegisterType::BULK:
      throw std::invalid_argument("Register cannot be read individually");
  }
  FinishRead(previousBadRead, clearPreviousBadRead);
  return true;
}

void GoBildaPinpoint::ReadPose() {
  bool previousBadRead = m_badReadDetected;
  m_badReadDetected = false;
  std::vector<uint8_t> data = ReadBulkSnapshot(
      {Register::X_POSITION, Register::Y_POSITION, Register::H_ORIENTATION});
  if (data.empty()) {
    return;
  }

  SavePose(DecodeFloat(data, 0), DecodeFloat(data, REGISTER_LENGTH),
           DecodeFloat(data, 2 * REGISTER_LENGTH), false);
  FinishRead(previousBadRead, false);
}

void GoBildaPinpoint::ReadQuaternion() {
  bool previousBadRead = m_badReadDetected;
  m_badReadDetected = false;
  std::vector<uint8_t> data =
      ReadBulkSnapshot({Register::QUATERNION_W, Register::QUATERNION_X,
                        Register::QUATERNION_Y, Register::QUATERNION_Z});
  if (data.empty()) {
    return;
  }

  SaveQuaternion(DecodeFloat(data, 0), DecodeFloat(data, REGISTER_LENGTH),
                 DecodeFloat(data, 2 * REGISTER_LENGTH),
                 DecodeFloat(data, 3 * REGISTER_LENGTH));
  FinishRead(previousBadRead, false);
}

std::vector<uint8_t> GoBildaPinpoint::ReadBulkSnapshot(
    const std::vector<Register>& registers) {
  if (!WriteBytes(Register::SET_BULK_READ, EncodeBulkReadScope(registers))) {
    return {};
  }
  m_bulkReadScopeSynchronized = false;

  std::size_t dataLength = registers.size() * REGISTER_LENGTH;
  int readLength = static_cast<int>(
      dataLength +
      (m_errorDetectionType == ErrorDetectionType::CRC ? CRC_LENGTH : 0));
  std::vector<uint8_t> data = ReadBytes(Register::BULK_READ, readLength);
  if (!WriteBytes(Register::SET_BULK_READ,
                  EncodeBulkReadScope(m_bulkReadScope))) {
    return {};
  }
  m_bulkReadScopeSynchronized = true;

  if (data.empty()) {
    return data;
  }
  if (m_errorDetectionType == ErrorDetectionType::CRC &&
      !CheckCrc(data, dataLength)) {
    RecordFailure(Register::BULK_READ, FailureReason::CRC_MISMATCH);
    return {};
  }
  return data;
}

float GoBildaPinpoint::DecodeBulkFloat(const std::vector<uint8_t>& data,
                                       Register reg) const {
  auto it = std::find(m_bulkReadScope.begin(), m_bulkReadScope.end(), reg);
  if (it == m_bulkReadScope.end()) {
    throw std::logic_error("Register is not in the bulk-read scope");
  }
  std::size_t offset =
      static_cast<std::size_t>(std::distance(m_bulkReadScope.begin(), it)) *
      REGISTER_LENGTH;
  return DecodeFloat(data, offset);
}

std::vector<uint8_t> GoBildaPinpoint::EncodeBulkReadScope(
    const std::vector<Register>& registers) {
  std::vector<uint8_t> addresses;
  addresses.reserve(registers.size());
  for (Register reg : registers) {
    addresses.push_back(static_cast<uint8_t>(reg));
  }
  return addresses;
}

void GoBildaPinpoint::FixedBulkRead() {
  if (m_errorDetectionType == ErrorDetectionType::CRC) {
    throw std::runtime_error(
        "CRC error detection requires firmware version 3 or newer");
  }

  bool previousBadRead = m_badReadDetected;
  m_badReadDetected = false;
  std::vector<uint8_t> data =
      ReadBytes(Register::BULK_READ, FIXED_BULK_READ_LENGTH);
  if (data.empty()) {
    return;
  }

  int32_t loopTime = DecodeInt(data, 4);
  if (m_errorDetectionType == ErrorDetectionType::LOCAL_TEST && loopTime <= 0) {
    RecordFailure(Register::LOOP_TIME, FailureReason::INVALID_LOOP_TIME);
    return;
  }

  SaveInt(Register::DEVICE_STATUS, DecodeInt(data, 0));
  SaveInt(Register::LOOP_TIME, loopTime);
  SaveInt(Register::X_ENCODER_VALUE, DecodeInt(data, 8));
  SaveInt(Register::Y_ENCODER_VALUE, DecodeInt(data, 12));
  SavePose(DecodeFloat(data, 16), DecodeFloat(data, 20), DecodeFloat(data, 24),
           true);
  SaveFloat(Register::X_VELOCITY, DecodeFloat(data, 28), true);
  SaveFloat(Register::Y_VELOCITY, DecodeFloat(data, 32), true);
  SaveFloat(Register::H_VELOCITY, DecodeFloat(data, 36), true);
  FinishRead(previousBadRead, true);
}

void GoBildaPinpoint::FlexibleBulkRead() {
  bool previousBadRead = m_badReadDetected;
  m_badReadDetected = false;
  std::size_t dataLength = m_bulkReadScope.size() * REGISTER_LENGTH;
  int readLength = static_cast<int>(
      dataLength +
      (m_errorDetectionType == ErrorDetectionType::CRC ? CRC_LENGTH : 0));
  std::vector<uint8_t> data = ReadBytes(Register::BULK_READ, readLength);
  if (data.empty()) {
    return;
  }
  if (m_errorDetectionType == ErrorDetectionType::CRC &&
      !CheckCrc(data, dataLength)) {
    RecordFailure(Register::BULK_READ, FailureReason::CRC_MISMATCH);
    return;
  }

  bool hasLoopTime = false;
  for (std::size_t i = 0; i < m_bulkReadScope.size(); ++i) {
    if (m_bulkReadScope[i] == Register::LOOP_TIME) {
      int32_t loopTime = DecodeInt(data, i * REGISTER_LENGTH);
      if (m_errorDetectionType == ErrorDetectionType::LOCAL_TEST &&
          loopTime <= 0) {
        RecordFailure(Register::LOOP_TIME, FailureReason::INVALID_LOOP_TIME);
        return;
      }
      SaveInt(Register::LOOP_TIME, loopTime);
      hasLoopTime = true;
      break;
    }
  }

  bool containsPose = BulkReadScopeContainsPose();
  if (containsPose) {
    SavePose(DecodeBulkFloat(data, Register::X_POSITION),
             DecodeBulkFloat(data, Register::Y_POSITION),
             DecodeBulkFloat(data, Register::H_ORIENTATION), hasLoopTime);
  }
  bool containsQuaternion = BulkReadScopeContainsQuaternion();
  if (containsQuaternion) {
    SaveQuaternion(DecodeBulkFloat(data, Register::QUATERNION_W),
                   DecodeBulkFloat(data, Register::QUATERNION_X),
                   DecodeBulkFloat(data, Register::QUATERNION_Y),
                   DecodeBulkFloat(data, Register::QUATERNION_Z));
  }

  for (std::size_t i = 0; i < m_bulkReadScope.size(); ++i) {
    Register reg = m_bulkReadScope[i];
    std::size_t offset = i * REGISTER_LENGTH;
    switch (GetRegisterType(reg)) {
      case RegisterType::INT32:
        if (reg != Register::LOOP_TIME && reg != Register::DEVICE_VERSION) {
          SaveInt(reg, DecodeInt(data, offset));
        }
        break;
      case RegisterType::FLOAT:
        if (!(containsPose &&
              (reg == Register::X_POSITION || reg == Register::Y_POSITION ||
               reg == Register::H_ORIENTATION)) &&
            !(containsQuaternion &&
              (reg == Register::QUATERNION_W || reg == Register::QUATERNION_X ||
               reg == Register::QUATERNION_Y ||
               reg == Register::QUATERNION_Z))) {
          SaveFloat(reg, DecodeFloat(data, offset), hasLoopTime);
        }
        break;
      case RegisterType::BULK:
        throw std::logic_error("A bulk-read scope contains BULK_READ");
    }
  }
  if (m_errorDetectionType == ErrorDetectionType::LOCAL_TEST) {
    auto contains = [this](Register reg) {
      return std::find(m_bulkReadScope.begin(), m_bulkReadScope.end(), reg) !=
             m_bulkReadScope.end();
    };
    if (!contains(Register::X_POSITION)) {
      m_haveXPosition = false;
    }
    if (!contains(Register::Y_POSITION)) {
      m_haveYPosition = false;
    }
    if (!contains(Register::H_ORIENTATION)) {
      m_haveHeading = false;
    }
  }
  FinishRead(previousBadRead, true);
}

void GoBildaPinpoint::SaveInt(Register reg, int32_t value) {
  switch (reg) {
    case Register::DEVICE_ID:
      m_deviceId = value;
      break;
    case Register::DEVICE_VERSION:
      m_deviceVersion = value;
      break;
    case Register::DEVICE_STATUS:
      m_deviceStatusBits = value;
      break;
    case Register::LOOP_TIME:
      m_loopTimeMicroseconds = value;
      break;
    case Register::X_ENCODER_VALUE:
      m_xEncoderValue = value;
      break;
    case Register::Y_ENCODER_VALUE:
      m_yEncoderValue = value;
      break;
    default:
      throw std::invalid_argument("Register is not an integer data register");
  }
}

void GoBildaPinpoint::SavePose(float xPosition, float yPosition, float heading,
                               bool bulkUpdate) {
  auto validatedX = ValidatePosition(
      Register::X_POSITION, m_xPositionMillimeters, xPosition,
      POSITION_CHANGE_LIMIT_MILLIMETERS, m_haveXPosition, bulkUpdate);
  auto validatedY = ValidatePosition(
      Register::Y_POSITION, m_yPositionMillimeters, yPosition,
      POSITION_CHANGE_LIMIT_MILLIMETERS, m_haveYPosition, bulkUpdate);
  auto validatedHeading =
      ValidatePosition(Register::H_ORIENTATION, m_headingRadians, heading,
                       HEADING_CHANGE_LIMIT_RADIANS, m_haveHeading, bulkUpdate);
  if (!validatedX || !validatedY || !validatedHeading) {
    return;
  }

  m_xPositionMillimeters = *validatedX;
  m_yPositionMillimeters = *validatedY;
  m_headingRadians = *validatedHeading;
  m_haveXPosition = true;
  m_haveYPosition = true;
  m_haveHeading = true;
}

void GoBildaPinpoint::SaveQuaternion(float w, float x, float y, float z) {
  bool validW = ValidateFinite(Register::QUATERNION_W, w);
  bool validX = ValidateFinite(Register::QUATERNION_X, x);
  bool validY = ValidateFinite(Register::QUATERNION_Y, y);
  bool validZ = ValidateFinite(Register::QUATERNION_Z, z);
  if (!validW || !validX || !validY || !validZ) {
    return;
  }
  double normSquared = static_cast<double>(w) * w + static_cast<double>(x) * x +
                       static_cast<double>(y) * y + static_cast<double>(z) * z;
  if (m_errorDetectionType == ErrorDetectionType::LOCAL_TEST &&
      normSquared < MIN_QUATERNION_NORM_SQUARED) {
    RecordFailure(Register::QUATERNION_W, FailureReason::INVALID_QUATERNION);
    return;
  }

  m_quaternionW = w;
  m_quaternionX = x;
  m_quaternionY = y;
  m_quaternionZ = z;
}

void GoBildaPinpoint::SaveFloat(Register reg, float value, bool bulkUpdate) {
  if (!ValidateFinite(reg, value)) {
    return;
  }

  switch (reg) {
    case Register::X_POSITION: {
      auto validated = ValidatePosition(
          Register::X_POSITION, m_xPositionMillimeters, value,
          POSITION_CHANGE_LIMIT_MILLIMETERS, m_haveXPosition, bulkUpdate);
      if (validated) {
        m_xPositionMillimeters = *validated;
        m_haveXPosition = true;
      }
      break;
    }
    case Register::Y_POSITION: {
      auto validated = ValidatePosition(
          Register::Y_POSITION, m_yPositionMillimeters, value,
          POSITION_CHANGE_LIMIT_MILLIMETERS, m_haveYPosition, bulkUpdate);
      if (validated) {
        m_yPositionMillimeters = *validated;
        m_haveYPosition = true;
      }
      break;
    }
    case Register::H_ORIENTATION: {
      auto validated = ValidatePosition(
          Register::H_ORIENTATION, m_headingRadians, value,
          HEADING_CHANGE_LIMIT_RADIANS, m_haveHeading, bulkUpdate);
      if (validated) {
        m_headingRadians = *validated;
        m_haveHeading = true;
      }
      break;
    }
    case Register::X_VELOCITY: {
      auto validated =
          ValidateVelocity(Register::X_VELOCITY, value,
                           VELOCITY_LIMIT_MILLIMETERS_PER_SECOND, bulkUpdate);
      if (validated) {
        m_xVelocityMillimetersPerSecond = *validated;
      }
      break;
    }
    case Register::Y_VELOCITY: {
      auto validated =
          ValidateVelocity(Register::Y_VELOCITY, value,
                           VELOCITY_LIMIT_MILLIMETERS_PER_SECOND, bulkUpdate);
      if (validated) {
        m_yVelocityMillimetersPerSecond = *validated;
      }
      break;
    }
    case Register::H_VELOCITY: {
      auto validated = ValidateVelocity(
          Register::H_VELOCITY, value,
          HEADING_VELOCITY_LIMIT_RADIANS_PER_SECOND, bulkUpdate);
      if (validated) {
        m_headingVelocityRadiansPerSecond = *validated;
      }
      break;
    }
    case Register::MM_PER_TICK:
      m_ticksPerMillimeter = value;
      break;
    case Register::X_POD_OFFSET:
      m_xPodOffsetMillimeters = value;
      break;
    case Register::Y_POD_OFFSET:
      m_yPodOffsetMillimeters = value;
      break;
    case Register::YAW_SCALAR:
      m_yawScalar = value;
      break;
    case Register::QUATERNION_W:
      m_quaternionW = value;
      break;
    case Register::QUATERNION_X:
      m_quaternionX = value;
      break;
    case Register::QUATERNION_Y:
      m_quaternionY = value;
      break;
    case Register::QUATERNION_Z:
      m_quaternionZ = value;
      break;
    case Register::PITCH:
      m_pitchRadians = value;
      break;
    case Register::ROLL:
      m_rollRadians = value;
      break;
    default:
      throw std::invalid_argument("Register is not a float data register");
  }
}

bool GoBildaPinpoint::ValidateFinite(Register reg, float value) {
  if (m_errorDetectionType == ErrorDetectionType::LOCAL_TEST &&
      !std::isfinite(value)) {
    RecordFailure(reg, FailureReason::NONFINITE_VALUE);
    return false;
  }
  return true;
}

std::optional<float> GoBildaPinpoint::ValidatePosition(
    Register reg, float oldValue, float newValue, float changeLimit,
    bool haveOldValue, bool bulkUpdate) {
  if (m_errorDetectionType != ErrorDetectionType::LOCAL_TEST) {
    return newValue;
  }
  if (bulkUpdate && m_loopTimeMicroseconds <= 0) {
    RecordFailure(reg, FailureReason::INVALID_LOOP_TIME);
    return std::nullopt;
  }
  if (!std::isfinite(newValue)) {
    RecordFailure(reg, FailureReason::NONFINITE_VALUE);
    return std::nullopt;
  }
  if (haveOldValue && std::abs(newValue - oldValue) > changeLimit) {
    RecordFailure(reg, FailureReason::CHANGE_TOO_LARGE);
    return std::nullopt;
  }
  return newValue;
}

std::optional<float> GoBildaPinpoint::ValidateVelocity(Register reg,
                                                       float newValue,
                                                       float magnitudeLimit,
                                                       bool bulkUpdate) {
  if (m_errorDetectionType != ErrorDetectionType::LOCAL_TEST) {
    return newValue;
  }
  if (bulkUpdate && m_loopTimeMicroseconds <= 0) {
    RecordFailure(reg, FailureReason::INVALID_LOOP_TIME);
    return std::nullopt;
  }
  if (!std::isfinite(newValue)) {
    RecordFailure(reg, FailureReason::NONFINITE_VALUE);
    return std::nullopt;
  }
  if (std::abs(newValue) > magnitudeLimit) {
    RecordFailure(reg, FailureReason::VELOCITY_TOO_LARGE);
    return std::nullopt;
  }
  return newValue;
}

void GoBildaPinpoint::RecordFailure(Register reg, FailureReason reason) {
  m_badReadDetected = true;
  m_lastFailedRegister = reg;
  m_lastFailureReason = reason;
  ++m_failureCount;
  ++m_failureCounts[RegisterIndex(reg)];
}

void GoBildaPinpoint::FinishRead(bool previousBadRead,
                                 bool clearPreviousBadRead) {
  if (!m_badReadDetected && previousBadRead && !clearPreviousBadRead) {
    m_badReadDetected = true;
  }
}

GoBildaPinpoint::DeviceStatus GoBildaPinpoint::DecodeStatus(int32_t status) {
  if ((status & static_cast<int32_t>(DeviceStatus::CALIBRATING)) != 0) {
    return DeviceStatus::CALIBRATING;
  }
  bool xPodDetected =
      (status & static_cast<int32_t>(DeviceStatus::FAULT_X_POD_NOT_DETECTED)) ==
      0;
  bool yPodDetected =
      (status & static_cast<int32_t>(DeviceStatus::FAULT_Y_POD_NOT_DETECTED)) ==
      0;
  if (!xPodDetected && !yPodDetected) {
    return DeviceStatus::FAULT_NO_PODS_DETECTED;
  }
  if (!xPodDetected) {
    return DeviceStatus::FAULT_X_POD_NOT_DETECTED;
  }
  if (!yPodDetected) {
    return DeviceStatus::FAULT_Y_POD_NOT_DETECTED;
  }
  if ((status & static_cast<int32_t>(DeviceStatus::FAULT_IMU_RUNAWAY)) != 0) {
    return DeviceStatus::FAULT_IMU_RUNAWAY;
  }
  if ((status & static_cast<int32_t>(DeviceStatus::FAULT_BAD_READ)) != 0) {
    return DeviceStatus::FAULT_BAD_READ;
  }
  if ((status & static_cast<int32_t>(DeviceStatus::READY)) != 0) {
    return DeviceStatus::READY;
  }
  return DeviceStatus::NOT_READY;
}

std::vector<uint8_t> GoBildaPinpoint::EncodeInt(int32_t value) {
  uint32_t bits = std::bit_cast<uint32_t>(value);
  return {static_cast<uint8_t>(bits), static_cast<uint8_t>(bits >> 8),
          static_cast<uint8_t>(bits >> 16), static_cast<uint8_t>(bits >> 24)};
}

std::vector<uint8_t> GoBildaPinpoint::EncodeFloat(float value) {
  uint32_t bits = std::bit_cast<uint32_t>(value);
  return {static_cast<uint8_t>(bits), static_cast<uint8_t>(bits >> 8),
          static_cast<uint8_t>(bits >> 16), static_cast<uint8_t>(bits >> 24)};
}

int32_t GoBildaPinpoint::DecodeInt(const std::vector<uint8_t>& data,
                                   std::size_t offset) {
  uint32_t bits = static_cast<uint32_t>(data[offset]) |
                  static_cast<uint32_t>(data[offset + 1]) << 8 |
                  static_cast<uint32_t>(data[offset + 2]) << 16 |
                  static_cast<uint32_t>(data[offset + 3]) << 24;
  return std::bit_cast<int32_t>(bits);
}

float GoBildaPinpoint::DecodeFloat(const std::vector<uint8_t>& data,
                                   std::size_t offset) {
  uint32_t bits = static_cast<uint32_t>(data[offset]) |
                  static_cast<uint32_t>(data[offset + 1]) << 8 |
                  static_cast<uint32_t>(data[offset + 2]) << 16 |
                  static_cast<uint32_t>(data[offset + 3]) << 24;
  return std::bit_cast<float>(bits);
}

bool GoBildaPinpoint::CheckCrc(const std::vector<uint8_t>& data,
                               std::size_t dataLength) {
  return data.size() > dataLength &&
         ComputeCrc8(data, dataLength) == data[dataLength];
}

uint8_t GoBildaPinpoint::ComputeCrc8(const std::vector<uint8_t>& data,
                                     std::size_t length) {
  uint8_t crc = CRC_INITIAL_VALUE;
  for (std::size_t index = 0; index < length; ++index) {
    crc ^= data[index];
    for (int bit = 0; bit < 8; ++bit) {
      if ((crc & 0x80) != 0) {
        crc = static_cast<uint8_t>((crc << 1) ^ CRC_POLYNOMIAL_VALUE);
      } else {
        crc = static_cast<uint8_t>(crc << 1);
      }
    }
  }
  return crc;
}
