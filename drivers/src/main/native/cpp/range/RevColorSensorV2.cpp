// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//
// This file is based on the FTC SDK drivers for the AMS TMD3782 family of color
// sensors, which were made available under the BSD 3-Clause License.
// Copyright (c) 2016-2017 Robert Atkinson and Steve Geffner.

#include "wpi/drivers/range/RevColorSensorV2.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <format>
#include <limits>
#include <stdexcept>
#include <thread>
#include <vector>

#include "wpi/util/UsageReporting.hpp"

using namespace wpi;

namespace {

void Delay(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int DecodeUnsignedShort(const std::vector<uint8_t>& data, std::size_t offset) {
  return data[offset] | (data[offset + 1] << 8);
}

}  // namespace

RevColorSensorV2::RevColorSensorV2(I2C::Port port, int deviceAddress)
    : m_i2c{port, ValidateAddress(deviceAddress)} {
  wpi::util::ReportUsage(std::format("I2C[{}]", static_cast<int>(port)),
                         deviceAddress, "RevColorSensorV2");
  Initialize();
}

I2C::Port RevColorSensorV2::GetPort() const {
  return m_i2c.GetPort();
}

int RevColorSensorV2::GetDeviceAddress() const {
  return m_i2c.GetDeviceAddress();
}

void RevColorSensorV2::Update() {
  if (!m_initialized && !Initialize()) {
    return;
  }

  m_deviceStatus = DeviceStatus::READY;
  std::vector<uint8_t> data = ReadRegister(Register::STATUS, BULK_READ_LENGTH);
  if (data.empty()) {
    return;
  }

  int status = data[0];
  if ((status & STATUS_COLOR_VALID) == 0) {
    RecordFailure(FailureReason::COLOR_DATA_NOT_VALID);
  } else {
    m_rawClear = DecodeUnsignedShort(data, CLEAR_OFFSET);
    m_rawRed = DecodeUnsignedShort(data, RED_OFFSET);
    m_rawGreen = DecodeUnsignedShort(data, GREEN_OFFSET);
    m_rawBlue = DecodeUnsignedShort(data, BLUE_OFFSET);
  }

  if ((status & STATUS_PROXIMITY_VALID) == 0) {
    RecordFailure(FailureReason::PROXIMITY_DATA_NOT_VALID);
  } else {
    m_rawProximity = DecodeUnsignedShort(data, PROXIMITY_OFFSET);
  }
}

void RevColorSensorV2::SetGain(Gain gain) {
  switch (gain) {
    case Gain::GAIN_1:
    case Gain::GAIN_4:
    case Gain::GAIN_16:
    case Gain::GAIN_60:
      break;
    default:
      throw std::invalid_argument("Invalid gain");
  }
  m_gain = gain;
  Reconfigure();
}

RevColorSensorV2::Gain RevColorSensorV2::GetGain() const {
  return m_gain;
}

void RevColorSensorV2::SetLedDrive(LedDrive ledDrive) {
  switch (ledDrive) {
    case LedDrive::PERCENT_100:
    case LedDrive::PERCENT_50:
    case LedDrive::PERCENT_25:
    case LedDrive::PERCENT_12_5:
      break;
    default:
      throw std::invalid_argument("Invalid LED drive");
  }
  m_ledDrive = ledDrive;
  Reconfigure();
}

RevColorSensorV2::LedDrive RevColorSensorV2::GetLedDrive() const {
  return m_ledDrive;
}

void RevColorSensorV2::SetIntegrationTime(
    wpi::units::millisecond_t integrationTime) {
  m_integrationTimeRegister = IntegrationTimeRegister(integrationTime);
  Reconfigure();
}

wpi::units::millisecond_t RevColorSensorV2::GetIntegrationTime() const {
  return wpi::units::millisecond_t{IntegrationCycles() *
                                   INTEGRATION_CYCLE_MILLISECONDS};
}

void RevColorSensorV2::SetProximityPulseCount(int proximityPulseCount) {
  if (proximityPulseCount < 1 || proximityPulseCount > 255) {
    throw std::invalid_argument(
        "proximityPulseCount must be between 1 and 255");
  }
  m_proximityPulseCount = proximityPulseCount;
  Reconfigure();
}

int RevColorSensorV2::GetProximityPulseCount() const {
  return m_proximityPulseCount;
}

void RevColorSensorV2::SetSoftwareGain(double softwareGain) {
  if (!std::isfinite(softwareGain) || softwareGain <= 0.0) {
    throw std::invalid_argument(
        "softwareGain must be finite and greater than zero");
  }
  m_softwareGain = softwareGain;
}

double RevColorSensorV2::GetSoftwareGain() const {
  return m_softwareGain;
}

void RevColorSensorV2::SetDistanceCalibration(double a, double b, double c) {
  if (!std::isfinite(a) || !std::isfinite(b) || !std::isfinite(c)) {
    throw std::invalid_argument(
        "Distance calibration parameters must be finite");
  }
  m_aParam = a;
  m_bParam = b;
  m_cParam = c;
}

wpi::units::meter_t RevColorSensorV2::GetDistance() const {
  if (m_rawProximity <= m_aParam) {
    return wpi::units::meter_t{std::numeric_limits<double>::quiet_NaN()};
  }
  double centimeters =
      (-m_aParam * m_cParam + m_cParam * m_rawProximity -
       std::sqrt(-m_aParam * m_bParam + m_bParam * m_rawProximity)) /
      (m_aParam - m_rawProximity);
  return wpi::units::centimeter_t{centimeters};
}

int RevColorSensorV2::GetRawProximity() const {
  return m_rawProximity;
}

double RevColorSensorV2::GetProximity() const {
  return std::clamp(m_rawProximity / static_cast<double>(PROXIMITY_SATURATION),
                    0.0, 1.0);
}

int RevColorSensorV2::GetMaximumRawProximityValue() const {
  return PROXIMITY_SATURATION;
}

wpi::util::Color RevColorSensorV2::GetColor() const {
  return wpi::util::Color{GetRed(), GetGreen(), GetBlue()};
}

double RevColorSensorV2::GetRed() const {
  return Normalize(m_rawRed);
}

double RevColorSensorV2::GetGreen() const {
  return Normalize(m_rawGreen);
}

double RevColorSensorV2::GetBlue() const {
  return Normalize(m_rawBlue);
}

double RevColorSensorV2::GetClear() const {
  return Normalize(m_rawClear);
}

int RevColorSensorV2::GetRawRed() const {
  return m_rawRed;
}

int RevColorSensorV2::GetRawGreen() const {
  return m_rawGreen;
}

int RevColorSensorV2::GetRawBlue() const {
  return m_rawBlue;
}

int RevColorSensorV2::GetRawClear() const {
  return m_rawClear;
}

int RevColorSensorV2::GetMaximumRawColorValue() const {
  return std::min(MAX_RAW_COLOR_VALUE,
                  COUNTS_PER_INTEGRATION_CYCLE * IntegrationCycles());
}

uint8_t RevColorSensorV2::GetDeviceId() const {
  return m_deviceId;
}

RevColorSensorV2::DeviceStatus RevColorSensorV2::GetDeviceStatus() const {
  return m_deviceStatus;
}

std::optional<RevColorSensorV2::FailureReason>
RevColorSensorV2::GetLastFailureReason() const {
  return m_lastFailureReason;
}

uint64_t RevColorSensorV2::GetFailureCount() const {
  return m_failureCount;
}

int RevColorSensorV2::ValidateAddress(int deviceAddress) {
  if (deviceAddress < 0 || deviceAddress > 0x7f) {
    throw std::invalid_argument("deviceAddress must be a 7-bit I2C address");
  }
  return deviceAddress;
}

int RevColorSensorV2::IntegrationTimeRegister(
    wpi::units::millisecond_t integrationTime) {
  double milliseconds = integrationTime.value();
  if (!std::isfinite(milliseconds) || milliseconds <= 0.0) {
    throw std::invalid_argument(
        "integrationTime must be finite and greater than zero");
  }
  int cycles = static_cast<int>(
      std::ceil(milliseconds / INTEGRATION_CYCLE_MILLISECONDS));
  return MAX_INTEGRATION_CYCLES - std::min(cycles, MAX_INTEGRATION_CYCLES);
}

bool RevColorSensorV2::Initialize() {
  m_initialized = false;
  m_deviceStatus = DeviceStatus::NOT_INITIALIZED;

  std::vector<uint8_t> data = ReadRegister(Register::DEVICE_ID, 1);
  if (data.empty()) {
    return false;
  }
  m_deviceId = data[0];
  if (m_deviceId != TMD37821_DEVICE_ID && m_deviceId != TMD37823_DEVICE_ID) {
    RecordFailure(FailureReason::UNEXPECTED_DEVICE_ID);
    return false;
  }
  return Configure();
}

void RevColorSensorV2::Reconfigure() {
  if (m_initialized) {
    Configure();
  }
}

bool RevColorSensorV2::Configure() {
  // The integrator must be off while the gain and integration time are
  // written, so the sensor is disabled first and reenabled afterwards.
  m_initialized = false;
  return Disable() &&
         WriteRegister(Register::ATIME, m_integrationTimeRegister) &&
         WriteControl() &&
         WriteRegister(Register::PPULSE, m_proximityPulseCount) && Enable();
}

bool RevColorSensorV2::WriteControl() {
  // Every CONTROL field is written explicitly rather than preserving the
  // register's current contents. Disabling the sensor does not reset CONTROL,
  // so a value retained from an earlier configuration could leave a reserved
  // proximity diode or proximity gain selection in place, which changes every
  // proximity reading and invalidates the distance calibration.
  //
  // This covers all eight bits: the LED drive occupies bits 7:6, the proximity
  // diode select bits 5:4, the proximity gain bits 3:2, and the color gain bits
  // 1:0. The proximity gain is left as zero, selecting the 1x gain the distance
  // calibration was fitted at and the only value this part defines.
  int control = static_cast<int>(m_ledDrive) | CONTROL_PDIODE_IR |
                static_cast<int>(m_gain);
  return WriteRegister(Register::CONTROL, control);
}

bool RevColorSensorV2::Enable() {
  if (!WriteEnable(ENABLE_POWER_ON)) {
    return false;
  }
  Delay(POWER_ON_DELAY_MILLISECONDS);

  if (!WriteEnable(ENABLE_POWER_ON | ENABLE_COLOR | ENABLE_PROXIMITY)) {
    return false;
  }
  Delay(ENABLE_SETTLE_DELAY_MILLISECONDS);

  m_initialized = true;
  m_deviceStatus = DeviceStatus::READY;
  return true;
}

bool RevColorSensorV2::Disable() {
  return WriteEnable(0);
}

bool RevColorSensorV2::WriteEnable(int value) {
  // The interrupt enables are not used, and the reserved high bits must be
  // written as zero.
  return WriteRegister(Register::ENABLE, value & ENABLE_WRITABLE_MASK);
}

std::vector<uint8_t> RevColorSensorV2::ReadRegister(Register reg, int count) {
  std::vector<uint8_t> data(count);
  if (m_i2c.Read(static_cast<int>(reg) | COMMAND_AUTO_INCREMENT, count,
                 data.data())) {
    RecordFailure(FailureReason::I2C_READ_ABORTED);
    return {};
  }
  return data;
}

bool RevColorSensorV2::WriteRegister(Register reg, int value) {
  if (m_i2c.Write(static_cast<int>(reg) | COMMAND_AUTO_INCREMENT,
                  static_cast<uint8_t>(value))) {
    RecordFailure(FailureReason::I2C_WRITE_ABORTED);
    return false;
  }
  return true;
}

void RevColorSensorV2::RecordFailure(FailureReason reason) {
  m_lastFailureReason = reason;
  ++m_failureCount;
  m_deviceStatus = reason == FailureReason::UNEXPECTED_DEVICE_ID
                       ? DeviceStatus::FAULT_UNEXPECTED_DEVICE_ID
                       : DeviceStatus::FAULT_BAD_READ;
}

double RevColorSensorV2::Normalize(int rawValue) const {
  return std::clamp(m_softwareGain * rawValue / GetMaximumRawColorValue(), 0.0,
                    1.0);
}

int RevColorSensorV2::IntegrationCycles() const {
  return MAX_INTEGRATION_CYCLES - m_integrationTimeRegister;
}
