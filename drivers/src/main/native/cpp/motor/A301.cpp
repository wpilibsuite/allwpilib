// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drivers/motor/A301.hpp"

#include <format>
#include <string>

#include "wpi/hal/UsageReporting.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/util/StackTrace.hpp"

using namespace wpi;

namespace {

constexpr uint16_t BitMask(int offset) {
  return static_cast<uint16_t>(1u << offset);
}

uint16_t PackFaults(const HAL_A301PeriodicStatus1& frame, bool sticky) {
  if (sticky) {
    return frame.otherStickyFault | (frame.motorTypeStickyFault << 1) |
           (frame.sensorStickyFault << 2) | (frame.canStickyFault << 3) |
           (frame.temperatureStickyFault << 4) | (frame.drvStickyFault << 5) |
           (frame.escEepromStickyFault << 6) |
           (frame.firmwareStickyFault << 7) |
           (frame.motorStartupStickyFault << 8);
  }
  return frame.otherFault | (frame.motorTypeFault << 1) |
         (frame.sensorFault << 2) | (frame.canFault << 3) |
         (frame.temperatureFault << 4) | (frame.drvFault << 5) |
         (frame.escEepromFault << 6) | (frame.firmwareFault << 7) |
         (frame.motorStartupFault << 8);
}

uint16_t PackWarnings(const HAL_A301PeriodicStatus1& frame, bool sticky) {
  if (sticky) {
    return frame.brownoutStickyWarning | (frame.overcurrentStickyWarning << 1) |
           (frame.escEepromStickyWarning << 2) |
           (frame.extEepromStickyWarning << 3) |
           (frame.sensorStickyWarning << 4) | (frame.stallStickyWarning << 5) |
           (frame.hasResetStickyWarning << 6) |
           (frame.otherStickyWarning << 7) |
           (frame.overvoltageStickyWarning << 8) |
           (frame.motorLoopSpeedStickyWarning << 9);
  }
  return frame.brownoutWarning | (frame.overcurrentWarning << 1) |
         (frame.escEepromWarning << 2) | (frame.extEepromWarning << 3) |
         (frame.sensorWarning << 4) | (frame.stallWarning << 5) |
         (frame.hasResetWarning << 6) | (frame.otherWarning << 7) |
         (frame.overvoltageWarning << 8) | (frame.motorLoopSpeedWarning << 9);
}

}  // namespace

A301::Faults::Faults(uint16_t faults) : rawBits{faults} {
  other = (faults & BitMask(0)) != 0;
  motorType = (faults & BitMask(1)) != 0;
  sensor = (faults & BitMask(2)) != 0;
  can = (faults & BitMask(3)) != 0;
  temperature = (faults & BitMask(4)) != 0;
  gateDriver = (faults & BitMask(5)) != 0;
  escEeprom = (faults & BitMask(6)) != 0;
  firmware = (faults & BitMask(7)) != 0;
  motorStartup = (faults & BitMask(8)) != 0;
}

A301::Warnings::Warnings(uint16_t warnings) : rawBits{warnings} {
  brownout = (warnings & BitMask(0)) != 0;
  overcurrent = (warnings & BitMask(1)) != 0;
  escEeprom = (warnings & BitMask(2)) != 0;
  extEeprom = (warnings & BitMask(3)) != 0;
  sensor = (warnings & BitMask(4)) != 0;
  stall = (warnings & BitMask(5)) != 0;
  hasReset = (warnings & BitMask(6)) != 0;
  other = (warnings & BitMask(7)) != 0;
  overvoltage = (warnings & BitMask(8)) != 0;
  motorLoopSpeed = (warnings & BitMask(9)) != 0;
}

A301::A301(CANBusMap bus) : A301{bus, kDefaultDeviceId} {}

A301::A301(CANBusMap bus, int deviceId) {
  auto stack = wpi::util::GetStackTrace(1);
  int32_t status = 0;
  m_handle = HAL_InitializeA301(static_cast<int>(bus), deviceId, stack.c_str(),
                                &status);
  WPILIB_CheckErrorStatus(status, "A301 bus {} device {}",
                          static_cast<int>(bus), deviceId);

  m_busId = HAL_GetA301BusId(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "A301 bus ID");
  m_deviceId = HAL_GetA301DeviceId(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "A301 device ID");

  HAL_ReportUsage(std::format("A301[{}][{}]", m_busId, m_deviceId), "");
}

int A301::GetBusId() const {
  return m_busId;
}

int A301::GetDeviceId() const {
  return m_deviceId;
}

A301::FirmwareVersion A301::GetFirmwareVersion() const {
  HAL_A301FirmwareVersion version{};
  int32_t status = 0;
  HAL_GetA301FirmwareVersion(m_handle, &version, &status);
  WPILIB_ReportError(status, "A301 firmware version");
  return {.major = version.major,
          .minor = version.minor,
          .patch = version.patch,
          .prerelease = version.prerelease,
          .hardwareRevision = version.hardwareRevision,
          .raw = version.raw};
}

std::string A301::GetFirmwareString() const {
  auto version = GetFirmwareVersion();
  if (version.prerelease != 0) {
    return std::format("v{}.{}.{} {} Debug Build", version.major, version.minor,
                       version.patch, version.prerelease);
  }
  return std::format("v{}.{}.{}", version.major, version.minor, version.patch);
}

A301::BooleanStatusSignal A301::HasActiveFault() const {
  auto signal = GetFaults();
  return {signal.Get().rawBits != 0, signal.GetStatus(), signal.GetTimestamp()};
}

A301::BooleanStatusSignal A301::HasStickyFault() const {
  auto signal = GetStickyFaults();
  return {signal.Get().rawBits != 0, signal.GetStatus(), signal.GetTimestamp()};
}

A301::BooleanStatusSignal A301::HasActiveWarning() const {
  auto signal = GetWarnings();
  return {signal.Get().rawBits != 0, signal.GetStatus(), signal.GetTimestamp()};
}

A301::BooleanStatusSignal A301::HasStickyWarning() const {
  auto signal = GetStickyWarnings();
  return {signal.Get().rawBits != 0, signal.GetStatus(), signal.GetTimestamp()};
}

A301::FaultsStatusSignal A301::GetFaults() const {
  auto signal = GetPeriodicStatus1();
  return {Faults{PackFaults(signal.Get(), false)}, signal.GetStatus(),
          signal.GetTimestamp()};
}

A301::FaultsStatusSignal A301::GetStickyFaults() const {
  auto signal = GetPeriodicStatus1();
  return {Faults{PackFaults(signal.Get(), true)}, signal.GetStatus(),
          signal.GetTimestamp()};
}

A301::WarningsStatusSignal A301::GetWarnings() const {
  auto signal = GetPeriodicStatus1();
  return {Warnings{PackWarnings(signal.Get(), false)}, signal.GetStatus(),
          signal.GetTimestamp()};
}

A301::WarningsStatusSignal A301::GetStickyWarnings() const {
  auto signal = GetPeriodicStatus1();
  return {Warnings{PackWarnings(signal.Get(), true)}, signal.GetStatus(),
          signal.GetTimestamp()};
}

A301Error A301::ClearFaults() {
  int32_t status = 0;
  HAL_ClearA301Faults(m_handle, &status);
  return detail::A301ErrorFromHalStatus(status);
}

A301::DoubleStatusSignal A301::GetBusVoltage() const {
  auto signal = GetPeriodicStatus0();
  return {signal.Get().voltage, signal.GetStatus(), signal.GetTimestamp()};
}

A301::DoubleStatusSignal A301::GetAppliedOutput() const {
  auto signal = GetPeriodicStatus0();
  return {signal.Get().appliedOutput, signal.GetStatus(),
          signal.GetTimestamp()};
}

A301::DoubleStatusSignal A301::GetMotorCurrent() const {
  auto signal = GetPeriodicStatus0();
  return {signal.Get().current, signal.GetStatus(), signal.GetTimestamp()};
}

A301::DoubleStatusSignal A301::GetMotorTemperature() const {
  auto signal = GetPeriodicStatus0();
  return {static_cast<double>(signal.Get().motorTemperature),
          signal.GetStatus(), signal.GetTimestamp()};
}

A301::GearboxRPMStatusSignal A301::GetGearboxRPM() const {
  auto signal = GetPeriodicStatus0();
  GearboxRPM rpm = GearboxRPM::kUnknown;
  if (signal.Get().gearboxRPM == HAL_A301_GEARBOX_RPM_215) {
    rpm = GearboxRPM::kRPM215;
  } else if (signal.Get().gearboxRPM == HAL_A301_GEARBOX_RPM_500) {
    rpm = GearboxRPM::kRPM500;
  }
  return {rpm, signal.GetStatus(), signal.GetTimestamp()};
}

A301::DoubleStatusSignal A301::GetRelativeEncoderPosition() const {
  auto signal = GetPeriodicStatus2();
  return {signal.Get().relativeEncoderPosition, signal.GetStatus(),
          signal.GetTimestamp()};
}

A301::DoubleStatusSignal A301::GetEncoderVelocity() const {
  auto signal = GetPeriodicStatus2();
  return {signal.Get().encoderVelocity, signal.GetStatus(),
          signal.GetTimestamp()};
}

A301::DoubleStatusSignal A301::GetAbsoluteEncoderPosition() const {
  auto signal = GetPeriodicStatus3();
  return {signal.Get().absoluteEncoderPosition, signal.GetStatus(),
          signal.GetTimestamp()};
}

A301Error A301::SetRelativeEncoderPosition(double position) {
  int32_t status = 0;
  HAL_SetA301RelativeEncoderPosition(m_handle, position, &status);
  return detail::A301ErrorFromHalStatus(status);
}

A301Error A301::SetAbsoluteEncoderPosition(double position) {
  int32_t status = 0;
  HAL_SetA301AbsoluteEncoderPosition(m_handle, position, &status);
  return detail::A301ErrorFromHalStatus(status);
}

A301Error A301::SetVelocity(double velocity) {
  return SetSetpoint(HAL_A301_CONTROL_TYPE_VELOCITY, velocity);
}

A301Error A301::SetRelativePosition(double position) {
  return SetSetpoint(HAL_A301_CONTROL_TYPE_RELATIVE_POSITION, position);
}

A301Error A301::SetRelativePositionWithSpeed(double position, double speed) {
  return SetSetpoint(HAL_A301_CONTROL_TYPE_RELATIVE_POSITION, position, speed);
}

A301Error A301::SetAbsolutePosition(double position) {
  return SetSetpoint(HAL_A301_CONTROL_TYPE_ABSOLUTE_POSITION, position);
}

A301Error A301::SetAbsolutePositionWithSpeed(double position, double speed) {
  return SetSetpoint(HAL_A301_CONTROL_TYPE_ABSOLUTE_POSITION, position, speed);
}

A301Error A301::SetCurrent(double current) {
  return SetSetpoint(HAL_A301_CONTROL_TYPE_CURRENT, current);
}

A301Error A301::SetIdleMode(IdleMode idleMode) {
  int32_t status = 0;
  HAL_SetA301IdleMode(m_handle, static_cast<HAL_A301IdleMode>(idleMode),
                      &status);
  return detail::A301ErrorFromHalStatus(status);
}

A301::IdleMode A301::GetIdleMode() const {
  int32_t status = 0;
  auto mode = HAL_GetA301IdleMode(m_handle, &status);
  WPILIB_ReportError(status, "A301 idle mode");
  return static_cast<IdleMode>(mode);
}

A301Error A301::EnableAbsolutePositionContinuousInput() {
  int32_t status = 0;
  HAL_SetA301AbsolutePositionContinuousInput(m_handle, true, &status);
  return detail::A301ErrorFromHalStatus(status);
}

A301Error A301::DisableAbsolutePositionContinuousInput() {
  int32_t status = 0;
  HAL_SetA301AbsolutePositionContinuousInput(m_handle, false, &status);
  return detail::A301ErrorFromHalStatus(status);
}

bool A301::IsAbsolutePositionContinuousInputEnabled() const {
  int32_t status = 0;
  bool enabled =
      HAL_GetA301AbsolutePositionContinuousInput(m_handle, &status) != 0;
  WPILIB_ReportError(status, "A301 absolute position continuous input");
  return enabled;
}

A301Error A301::SetAbsoluteEncoderRangeOffset(double offset) {
  int32_t status = 0;
  HAL_SetA301AbsoluteEncoderRangeOffset(m_handle, offset, &status);
  return detail::A301ErrorFromHalStatus(status);
}

double A301::GetAbsoluteEncoderRangeOffset() const {
  int32_t status = 0;
  double offset = HAL_GetA301AbsoluteEncoderRangeOffset(m_handle, &status);
  WPILIB_ReportError(status, "A301 absolute encoder range offset");
  return offset;
}

void A301::SetThrottle(double throttle) {
  m_setpoint = throttle;
  SetSetpoint(HAL_A301_CONTROL_TYPE_DUTY_CYCLE, throttle);
}

double A301::GetThrottle() const {
  return m_setpoint;
}

void A301::SetVoltage(wpi::units::volt_t outputVolts) {
  m_setpoint = outputVolts.value() / 12.0;
  SetSetpoint(HAL_A301_CONTROL_TYPE_VOLTAGE, outputVolts.value());
}

void A301::SetInverted(bool isInverted) {
  int32_t status = 0;
  HAL_SetA301Inverted(m_handle, isInverted, &status);
}

bool A301::GetInverted() const {
  int32_t status = 0;
  bool inverted = HAL_GetA301Inverted(m_handle, &status) != 0;
  WPILIB_ReportError(status, "A301 inversion");
  return inverted;
}

void A301::Disable() {
  SetThrottle(0.0);
}

A301& A301::SetStatusFramePeriod(PeriodicFrame frame, int periodMs) {
  int32_t status = 0;
  HAL_SetA301StatusFramePeriod(
      m_handle, static_cast<HAL_A301PeriodicFrame>(frame), periodMs, &status);
  return *this;
}

int A301::GetStatusFramePeriod(PeriodicFrame frame) const {
  int32_t status = 0;
  int period = HAL_GetA301StatusFramePeriod(
      m_handle, static_cast<HAL_A301PeriodicFrame>(frame), &status);
  WPILIB_ReportError(status, "A301 status frame period");
  return period;
}

A301& A301::FaultsPeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus1, periodMs);
}

int A301::GetFaultsPeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus1);
}

A301& A301::WarningsPeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus1, periodMs);
}

int A301::GetWarningsPeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus1);
}

A301& A301::BusVoltagePeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus0, periodMs);
}

int A301::GetBusVoltagePeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus0);
}

A301& A301::AppliedOutputPeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus0, periodMs);
}

int A301::GetAppliedOutputPeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus0);
}

A301& A301::MotorCurrentPeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus0, periodMs);
}

int A301::GetMotorCurrentPeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus0);
}

A301& A301::MotorTemperaturePeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus0, periodMs);
}

int A301::GetMotorTemperaturePeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus0);
}

A301& A301::RelativeEncoderPositionPeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus2, periodMs);
}

int A301::GetRelativeEncoderPositionPeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus2);
}

A301& A301::EncoderVelocityPeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus2, periodMs);
}

int A301::GetEncoderVelocityPeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus2);
}

A301& A301::AbsoluteEncoderPositionPeriodMs(int periodMs) {
  return SetStatusFramePeriod(PeriodicFrame::kStatus3, periodMs);
}

int A301::GetAbsoluteEncoderPositionPeriodMs() const {
  return GetStatusFramePeriod(PeriodicFrame::kStatus3);
}

A301StatusSignal<HAL_A301PeriodicStatus0> A301::GetPeriodicStatus0() const {
  HAL_A301PeriodicStatus0 frame{};
  int32_t status = 0;
  HAL_GetA301PeriodicStatus0(m_handle, &frame, &status);
  return {frame, status, frame.timestamp};
}

A301StatusSignal<HAL_A301PeriodicStatus1> A301::GetPeriodicStatus1() const {
  HAL_A301PeriodicStatus1 frame{};
  int32_t status = 0;
  HAL_GetA301PeriodicStatus1(m_handle, &frame, &status);
  return {frame, status, frame.timestamp};
}

A301StatusSignal<HAL_A301PeriodicStatus2> A301::GetPeriodicStatus2() const {
  HAL_A301PeriodicStatus2 frame{};
  int32_t status = 0;
  HAL_GetA301PeriodicStatus2(m_handle, &frame, &status);
  return {frame, status, frame.timestamp};
}

A301StatusSignal<HAL_A301PeriodicStatus3> A301::GetPeriodicStatus3() const {
  HAL_A301PeriodicStatus3 frame{};
  int32_t status = 0;
  HAL_GetA301PeriodicStatus3(m_handle, &frame, &status);
  return {frame, status, frame.timestamp};
}

A301Error A301::SetSetpoint(HAL_A301ControlType controlType, double setpoint,
                            double positionSpeed) {
  int32_t status = 0;
  HAL_SetA301Setpoint(m_handle, setpoint, controlType, positionSpeed, &status);
  return detail::A301ErrorFromHalStatus(status);
}
