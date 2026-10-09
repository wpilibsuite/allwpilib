// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/XRP.hpp"

#include <algorithm>
#include <bit>
#include <chrono>
#include <cmath>
#include <limits>
#include <mutex>
#include <numbers>

#include "wpi/hal/Ports.h"
#include "wpi/hal/SimDevice.h"
#include "wpi/hal/simulation/AnalogInData.h"
#include "wpi/hal/simulation/DIOData.h"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/hal/simulation/EncoderData.h"
#include "wpi/hal/simulation/IMUData.h"
#include "wpi/hal/simulation/SimDeviceData.h"
#include "wpi/util/Endian.hpp"

using namespace wpilibxrp;

namespace {

bool HasField(uint16_t mask, uint16_t field) {
  return (mask & field) != 0;
}

size_t ExpectedStatusPayloadSize(uint16_t mask) {
  size_t size = 0;

  for (int encoder = 0; encoder < 4; encoder++) {
    if (HasField(mask, STATUS_ENCODER_0 << encoder)) {
      size += 8;
    }
  }

  if (HasField(mask, STATUS_DIO)) {
    size += 2;
  }
  if (HasField(mask, STATUS_GYRO)) {
    size += 24;
  }
  if (HasField(mask, STATUS_ACCEL)) {
    size += 12;
  }
  for (int analog = 0; analog < 3; analog++) {
    if (HasField(mask, STATUS_ANALOG_0 << analog)) {
      size += 2;
    }
  }
  if (HasField(mask, STATUS_TIMING)) {
    size += 4;
  }
  if (HasField(mask, STATUS_COMMAND_ACK)) {
    size += 5;
  }

  return size;
}

void WriteUint16(wpi::net::raw_uv_ostream& buf, uint16_t value) {
  uint8_t bytes[2];
  wpi::util::support::endian::write16be(bytes, value);
  buf << bytes[0] << bytes[1];
}

void WriteInt16(wpi::net::raw_uv_ostream& buf, int16_t value) {
  WriteUint16(buf, static_cast<uint16_t>(value));
}

uint16_t ReadUint16(std::span<const uint8_t> packet, size_t offset = 0) {
  return wpi::util::support::endian::read16be(&packet[offset]);
}

float ReadFloat(std::span<const uint8_t> packet, size_t offset = 0) {
  return std::bit_cast<float>(
      wpi::util::support::endian::read32be(&packet[offset]));
}

int16_t EncodeMotorOutput(float value) {
  if (!std::isfinite(value)) {
    return 0;
  }
  return static_cast<int16_t>(std::clamp(value, -1.0f, 1.0f) * MOTOR_MAX_PWM);
}

uint8_t EncodeServoOutput(float value) {
  if (!std::isfinite(value)) {
    return 0;
  }
  return static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) *
                              SERVO_MAX_DEGREES);
}

float GetSimDouble(const char* deviceName, const char* valueName,
                   float defaultValue) {
  auto device = HALSIM_GetSimDeviceHandle(deviceName);
  auto value = HALSIM_GetSimValueHandle(device, valueName);
  return value ? HAL_GetSimValueDouble(value) : defaultValue;
}

}  // namespace

XRP::XRP()
    : m_encoders{std::make_unique<EncoderSimData[]>(HAL_GetNumEncoders())} {
  for (int i = 0; i < HAL_GetNumEncoders(); ++i) {
    auto& encoder = m_encoders[i];
    encoder.index = i;
    encoder.initializedCallback = HALSIM_RegisterEncoderInitializedCallback(
        i,
        [](const char*, void* param, const HAL_Value*) {
          static_cast<EncoderSimData*>(param)->countOffset = 0;
        },
        &encoder, false);
    encoder.resetCallback = HALSIM_RegisterEncoderResetCallback(
        i,
        [](const char*, void* param, const HAL_Value* value) {
          if (value->data.v_boolean) {
            auto& encoder = *static_cast<EncoderSimData*>(param);
            // HAL calls this before zeroing the simulated encoder count.
            encoder.countOffset.fetch_add(
                HALSIM_GetEncoderCount(encoder.index));
          }
        },
        &encoder, false);
  }
}

XRP::~XRP() {
  for (int i = 0; i < HAL_GetNumEncoders(); ++i) {
    HALSIM_CancelEncoderInitializedCallback(i,
                                            m_encoders[i].initializedCallback);
    HALSIM_CancelEncoderResetCallback(i, m_encoders[i].resetCallback);
  }
}

bool XRP::HandleXRPUpdate(std::span<const uint8_t> packet) {
  if (packet.size() < PACKET_HEADER_SIZE) {
    return false;
  }

  uint16_t seq = (packet[0] << 8) + packet[1];
  uint16_t fieldMask = ReadUint16(packet, 3);
  if ((fieldMask & ~STATUS_ALL_FIELDS) != 0 ||
      packet.size() !=
          PACKET_HEADER_SIZE + ExpectedStatusPayloadSize(fieldMask)) {
    return false;
  }

  {
    std::scoped_lock lock(m_data_snapshot_mutex);
    uint16_t distance = static_cast<uint16_t>(seq - m_wpilib_bound_seq);
    // Firmware may coalesce packets across rollover. Half the sequence space
    // or more is ambiguous and must be treated as stale.
    if (m_have_wpilib_bound_seq && (distance == 0 || distance >= 0x8000)) {
      return false;
    }

    m_wpilib_bound_seq = seq;
    m_have_wpilib_bound_seq = true;
    auto& packetInfo = m_data_snapshot.status.packet;
    packetInfo.present = true;
    packetInfo.sequence = seq;
    packetInfo.fieldMask = fieldMask;
    packetInfo.lastUpdate = std::chrono::steady_clock::now();
  }

  packet = packet.subspan(PACKET_HEADER_SIZE);
  for (int encoder = 0; encoder < 4; encoder++) {
    if (HasField(fieldMask, STATUS_ENCODER_0 << encoder)) {
      ReadEncoderData(encoder, packet.subspan(0, 8));
      packet = packet.subspan(8);
    }
  }

  if (HasField(fieldMask, STATUS_DIO)) {
    ReadDIOData(packet[0], packet[1]);
    packet = packet.subspan(2);
  }

  if (HasField(fieldMask, STATUS_GYRO)) {
    ReadGyroData(packet.subspan(0, 24));
    packet = packet.subspan(24);
  }

  if (HasField(fieldMask, STATUS_ACCEL)) {
    ReadAccelData(packet.subspan(0, 12));
    packet = packet.subspan(12);
  }

  for (int analog = 0; analog < 3; analog++) {
    if (HasField(fieldMask, STATUS_ANALOG_0 << analog)) {
      ReadAnalogData(analog, packet.subspan(0, 2));
      packet = packet.subspan(2);
    }
  }

  if (HasField(fieldMask, STATUS_TIMING)) {
    packet = packet.subspan(4);
  }

  if (HasField(fieldMask, STATUS_COMMAND_ACK)) {
    ReadCommandAckData(packet.subspan(0, 5));
  }

  return true;
}

void XRP::SetupXRPSendBuffer(wpi::net::raw_uv_ostream& buf) {
  ReadHALOutputs();
  uint16_t fieldMask = GetControlFieldMask();
  SetupSendHeader(buf, fieldMask);
  SetupMotorFields(buf, fieldMask);
  SetupServoFields(buf, fieldMask);
  SetupDigitalOutFields(buf, fieldMask);
  RecordControlData(fieldMask);
  m_xrp_bound_seq++;
}

uint16_t XRP::SetupRenameDeviceBuffer(wpi::net::raw_uv_ostream& buf,
                                      std::string_view deviceName) {
  uint16_t seq = m_xrp_bound_seq;
  SetupSendHeader(buf, CONTROL_DEVICE_NAME);
  buf << static_cast<uint8_t>(deviceName.size());
  for (char c : deviceName) {
    buf << static_cast<uint8_t>(c);
  }
  m_xrp_bound_seq++;
  return seq;
}

uint16_t XRP::SetupIdentifyBuffer(wpi::net::raw_uv_ostream& buf) {
  uint16_t seq = m_xrp_bound_seq;
  SetupSendHeader(buf, CONTROL_IDENTIFY);
  m_xrp_bound_seq++;
  return seq;
}

void XRP::ResetStatusPacketSequence() {
  std::scoped_lock lock(m_data_snapshot_mutex);
  m_have_wpilib_bound_seq = false;
  m_wpilib_bound_seq = 0;
}

XRPDataSnapshot XRP::GetDataSnapshot() const {
  std::scoped_lock lock(m_data_snapshot_mutex);
  return m_data_snapshot;
}

void XRP::ReadHALOutputs() {
  constexpr std::array MOTOR_NAMES{"XRPMotor:motorL", "XRPMotor:motorR",
                                   "XRPMotor:motor3", "XRPMotor:motor4"};
  constexpr std::array SERVO_NAMES{"XRPServo:servo1", "XRPServo:servo2",
                                   "XRPServo:servo3", "XRPServo:servo4"};
  for (int channel = 0; channel < 4; ++channel) {
    m_motor_outputs[channel] =
        GetSimDouble(MOTOR_NAMES[channel], "throttle", 0.0f);
  }

  m_servo_outputs.clear();
  for (int channel = 0; channel < 4; ++channel) {
    // Preserve the two onboard servos' neutral defaults without requiring a
    // robot program to allocate them. Additional servos are sent when present.
    if (channel < 2 || HALSIM_GetSimDeviceHandle(SERVO_NAMES[channel])) {
      m_servo_outputs[channel + 4] =
          GetSimDouble(SERVO_NAMES[channel], "position", 0.5f);
    }
  }

  m_digital_outputs.clear();
  for (int channel = 0; channel < 8; ++channel) {
    if (HALSIM_GetDIOInitialized(channel) && !HALSIM_GetDIOIsInput(channel)) {
      m_digital_outputs[channel] = HALSIM_GetDIOValue(channel);
    }
  }
}

// ==================================
// XRP Buffer Generation/Read Methods
// ==================================

uint16_t XRP::GetControlFieldMask() const {
  uint16_t fieldMask = 0;

  for (const auto& [channel, value] : m_motor_outputs) {
    (void)value;
    if (channel < 4) {
      fieldMask |= CONTROL_MOTOR_0 << channel;
    }
  }

  for (const auto& [channel, value] : m_servo_outputs) {
    (void)value;
    if (channel >= 4 && channel < 8) {
      fieldMask |= 1u << channel;
    }
  }

  for (const auto& [channel, value] : m_digital_outputs) {
    (void)value;
    if (channel < 8) {
      fieldMask |= CONTROL_DIO;
      break;
    }
  }

  return fieldMask;
}

void XRP::SetupSendHeader(wpi::net::raw_uv_ostream& buf, uint16_t fieldMask) {
  m_robot_enabled = HALSIM_GetDriverStationEnabled();
  uint8_t pktSeq[2];
  wpi::util::support::endian::write16be(pktSeq, m_xrp_bound_seq);

  buf << pktSeq[0] << pktSeq[1]
      << static_cast<uint8_t>(m_robot_enabled ? 1 : 0);
  WriteUint16(buf, fieldMask);
}

void XRP::SetupMotorFields(wpi::net::raw_uv_ostream& buf, uint16_t fieldMask) {
  for (int channel = 0; channel < 4; channel++) {
    if (HasField(fieldMask, CONTROL_MOTOR_0 << channel)) {
      auto motor = m_motor_outputs.find(channel);
      WriteInt16(buf, EncodeMotorOutput(motor == m_motor_outputs.end()
                                            ? 0.0f
                                            : motor->second));
    }
  }
}

void XRP::SetupServoFields(wpi::net::raw_uv_ostream& buf, uint16_t fieldMask) {
  for (int channel = 4; channel < 8; channel++) {
    if (HasField(fieldMask, 1u << channel)) {
      auto servo = m_servo_outputs.find(channel);
      buf << EncodeServoOutput(servo == m_servo_outputs.end() ? 0.5f
                                                              : servo->second);
    }
  }
}

void XRP::SetupDigitalOutFields(wpi::net::raw_uv_ostream& buf,
                                uint16_t fieldMask) {
  if (!HasField(fieldMask, CONTROL_DIO)) {
    return;
  }

  uint8_t presentMask = 0;
  uint8_t valueMask = 0;
  for (const auto& [channel, value] : m_digital_outputs) {
    if (channel < 8) {
      presentMask |= 1u << channel;
      valueMask |= value ? 1u << channel : 0u;
    }
  }
  buf << presentMask << valueMask;
}

void XRP::RecordControlData(uint16_t fieldMask) {
  auto now = std::chrono::steady_clock::now();
  std::scoped_lock lock(m_data_snapshot_mutex);

  auto& control = m_data_snapshot.control;
  control.enabled = m_robot_enabled;
  control.packet.present = true;
  control.packet.sequence = m_xrp_bound_seq;
  control.packet.fieldMask = fieldMask;
  control.packet.lastUpdate = now;

  for (int channel = 0; channel < 4; channel++) {
    if (!HasField(fieldMask, CONTROL_MOTOR_0 << channel)) {
      continue;
    }
    auto& motor = control.motors[channel];
    auto motorOutput = m_motor_outputs.find(channel);
    motor.value =
        motorOutput == m_motor_outputs.end() ? 0.0f : motorOutput->second;
    motor.present = true;
    motor.lastUpdate = now;
  }

  for (int channel = 4; channel < 8; channel++) {
    if (!HasField(fieldMask, 1u << channel)) {
      continue;
    }
    auto& servo = control.servos[channel - 4];
    auto servoOutput = m_servo_outputs.find(channel);
    servo.value =
        servoOutput == m_servo_outputs.end() ? 0.5f : servoOutput->second;
    servo.present = true;
    servo.lastUpdate = now;
  }

  if (HasField(fieldMask, CONTROL_DIO)) {
    for (const auto& [channel, value] : m_digital_outputs) {
      if (channel >= control.digitalOutputs.size()) {
        continue;
      }
      auto& digitalOutput = control.digitalOutputs[channel];
      digitalOutput.value = value;
      digitalOutput.present = true;
      digitalOutput.lastUpdate = now;
    }
  }
}

void XRP::ReadGyroData(std::span<const uint8_t> packet) {
  if (packet.size() < 24) {
    return;
  }

  float rate_x = ReadFloat(packet, 0);
  float rate_y = ReadFloat(packet, 4);
  float rate_z = ReadFloat(packet, 8);
  float angle_x = ReadFloat(packet, 12);
  float angle_y = ReadFloat(packet, 16);
  float angle_z = ReadFloat(packet, 20);

  {
    std::scoped_lock lock(m_data_snapshot_mutex);
    auto& gyro = m_data_snapshot.status.gyro;
    gyro.value = {{rate_x, rate_y, rate_z}, {angle_x, angle_y, angle_z}};
    gyro.present = true;
    gyro.lastUpdate = std::chrono::steady_clock::now();
  }

  constexpr double DEGREES_TO_RADIANS = std::numbers::pi / 180.0;
  HALSIM_SetIMUGyroRateX(rate_x * DEGREES_TO_RADIANS);
  HALSIM_SetIMUGyroRateY(rate_y * DEGREES_TO_RADIANS);
  HALSIM_SetIMUGyroRateZ(rate_z * DEGREES_TO_RADIANS);
  HALSIM_SetIMUAngleX(angle_x * DEGREES_TO_RADIANS);
  HALSIM_SetIMUAngleY(angle_y * DEGREES_TO_RADIANS);
  HALSIM_SetIMUAngleZ(angle_z * DEGREES_TO_RADIANS);
  HALSIM_SetIMUYaw(angle_z * DEGREES_TO_RADIANS);
}

void XRP::ReadAccelData(std::span<const uint8_t> packet) {
  if (packet.size() < 12) {
    return;
  }

  float accel_x = ReadFloat(packet, 0);
  float accel_y = ReadFloat(packet, 4);
  float accel_z = ReadFloat(packet, 8);
  {
    std::scoped_lock lock(m_data_snapshot_mutex);
    auto& accel = m_data_snapshot.status.accel;
    accel.value = {accel_x, accel_y, accel_z};
    accel.present = true;
    accel.lastUpdate = std::chrono::steady_clock::now();
  }

  constexpr double STANDARD_GRAVITY = 9.80665;
  HALSIM_SetIMUAccelX(accel_x * STANDARD_GRAVITY);
  HALSIM_SetIMUAccelY(accel_y * STANDARD_GRAVITY);
  HALSIM_SetIMUAccelZ(accel_z * STANDARD_GRAVITY);
}

void XRP::ReadDIOData(uint8_t presentMask, uint8_t valueMask) {
  auto now = std::chrono::steady_clock::now();
  {
    std::scoped_lock lock(m_data_snapshot_mutex);
    for (int channel = 0; channel < 8; channel++) {
      uint8_t bit = 1u << channel;
      if ((presentMask & bit) == 0) {
        continue;
      }
      auto& digitalInput = m_data_snapshot.status.digitalInputs[channel];
      digitalInput.value = (valueMask & bit) != 0;
      digitalInput.present = true;
      digitalInput.lastUpdate = now;
    }
  }

  for (int channel = 0; channel < 8; channel++) {
    uint8_t bit = 1u << channel;
    if ((presentMask & bit) == 0) {
      continue;
    }
    if (HALSIM_GetDIOInitialized(channel) && HALSIM_GetDIOIsInput(channel)) {
      HALSIM_SetDIOValue(channel, (valueMask & bit) != 0);
    }
  }
}

void XRP::ReadEncoderData(uint8_t encoderId, std::span<const uint8_t> packet) {
  if (packet.size() < 8) {
    return;
  }

  int32_t count =
      static_cast<int32_t>(wpi::util::support::endian::read32be(&packet[0]));
  uint32_t period_numerator =
      static_cast<uint32_t>(wpi::util::support::endian::read32be(&packet[4]));
  XRPEncoderData encoderData;
  encoderData.count = count;
  if (period_numerator != (std::numeric_limits<uint32_t>::max)()) {
    encoderData.period =
        static_cast<double>(period_numerator >> 1) / ENCODER_PERIOD_DENOMINATOR;

    // If direction is not forward, return negative value for period.
    if (!(period_numerator & 1)) {
      encoderData.period = -encoderData.period;
    }
    encoderData.periodValid = true;
  }

  {
    std::scoped_lock lock(m_data_snapshot_mutex);
    auto& encoder = m_data_snapshot.status.encoders[encoderId];
    encoder.value = encoderData;
    encoder.present = true;
    encoder.lastUpdate = std::chrono::steady_clock::now();
  }

  int channelA = 4 + 2 * encoderId;
  int channelB = channelA + 1;
  int index = HALSIM_FindEncoderForChannel(channelA);
  if (index < 0) {
    return;
  }
  int a = HALSIM_GetEncoderDigitalChannelA(index);
  int b = HALSIM_GetEncoderDigitalChannelB(index);
  if (!((a == channelA && b == channelB) || (a == channelB && b == channelA))) {
    return;
  }

  auto offset = m_encoders[index].countOffset.load();
  HALSIM_SetEncoderCount(index,
                         static_cast<int32_t>(static_cast<uint32_t>(count) -
                                              static_cast<uint32_t>(offset)));
  if (encoderData.periodValid) {
    HALSIM_SetEncoderRate(
        index, HALSIM_GetEncoderDistancePerPulse(index) / encoderData.period);
  }
}

void XRP::ReadAnalogData(uint8_t analogId, std::span<const uint8_t> packet) {
  if (packet.size() < 2) {
    return;
  }

  float voltage = static_cast<float>(ReadUint16(packet)) * ANALOG_MAX_VOLTAGE /
                  ANALOG_MAX_VALUE;

  {
    std::scoped_lock lock(m_data_snapshot_mutex);
    auto& analogInput = m_data_snapshot.status.analogInputs[analogId];
    analogInput.value = voltage;
    analogInput.present = true;
    analogInput.lastUpdate = std::chrono::steady_clock::now();
  }

  HALSIM_SetAnalogInVoltage(analogId, voltage);
}

void XRP::ReadCommandAckData(std::span<const uint8_t> packet) {
  if (packet.size() < 5) {
    return;
  }

  std::scoped_lock lock(m_data_snapshot_mutex);
  auto& commandAck = m_data_snapshot.status.commandAck;
  commandAck.value.controlSeq = ReadUint16(packet, 0);
  commandAck.value.controlFieldMask = ReadUint16(packet, 2);
  commandAck.value.result = packet[4];
  commandAck.present = true;
  commandAck.lastUpdate = std::chrono::steady_clock::now();
}
