// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/halsim/xrp/XRP.hpp"

#include <algorithm>
#include <bit>
#include <chrono>
#include <limits>
#include <mutex>
#include <string>
#include <utility>

#include "wpi/util/Endian.hpp"
#include "wpi/util/json.hpp"

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
  return static_cast<int16_t>(std::clamp(value, -1.0f, 1.0f) * MOTOR_MAX_PWM);
}

uint8_t EncodeServoOutput(float value) {
  return static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) *
                              SERVO_MAX_DEGREES);
}

}  // namespace

XRP::XRP()
    : m_gyro_name{"XRPGyro"},
      m_wpilib_update_func([](const wpi::util::json&) {}) {
  // Set up the inputs and outputs
  m_motor_outputs.emplace(0, 0.0f);
  m_motor_outputs.emplace(1, 0.0f);
  m_motor_outputs.emplace(2, 0.0f);
  m_motor_outputs.emplace(3, 0.0f);

  m_servo_outputs.emplace(4, 0.5f);
  m_servo_outputs.emplace(5, 0.5f);

  m_encoder_inputs.emplace(1, 0);
  m_encoder_inputs.emplace(2, 0);
  m_encoder_inputs.emplace(0, 0);
  m_encoder_inputs.emplace(3, 0);
}

void XRP::HandleWPILibUpdate(const wpi::util::json& data) {
  auto type = data.lookup("type");
  if (!type || !type->is_string()) {
    return;
  }

  auto& typeStr = type->get_string();
  if (typeStr == "DriverStation") {
    HandleDriverStationSimValueChanged(data);
  } else if (typeStr == "XRPMotor") {
    HandleMotorSimValueChanged(data);
  } else if (typeStr == "XRPServo") {
    HandleServoSimValueChanged(data);
  } else if (typeStr == "DIO") {
    HandleDIOSimValueChanged(data);
  } else if (typeStr == "Gyro") {
    HandleGyroSimValueChanged(data);
  } else if (typeStr == "Encoder") {
    HandleEncoderSimValueChanged(data);
  }
}

void XRP::HandleXRPUpdate(std::span<const uint8_t> packet) {
  if (packet.size() < PACKET_HEADER_SIZE) {
    return;
  }

  uint16_t seq = (packet[0] << 8) + packet[1];
  uint16_t fieldMask = ReadUint16(packet, 3);
  if ((fieldMask & ~STATUS_ALL_FIELDS) != 0 ||
      packet.size() !=
          PACKET_HEADER_SIZE + ExpectedStatusPayloadSize(fieldMask)) {
    return;
  }

  {
    std::scoped_lock lock(m_data_snapshot_mutex);
    if (m_have_wpilib_bound_seq && seq <= m_wpilib_bound_seq) {
      // If the old sequence was within 3 or uint16_t max and the new
      // sequence is < 3 - we've prob rolled over
      if (!((0xFFFF - m_wpilib_bound_seq < 3) && seq < 3)) {
        return;
      }
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
}

void XRP::SetupXRPSendBuffer(wpi::net::raw_uv_ostream& buf) {
  uint16_t fieldMask = GetControlFieldMask();
  SetupSendHeader(buf, fieldMask);
  SetupMotorFields(buf, fieldMask);
  SetupServoFields(buf, fieldMask);
  SetupDigitalOutFields(buf, fieldMask);
  RecordControlData(fieldMask);
  m_xrp_bound_seq++;
}

void XRP::SetupRenameDeviceBuffer(wpi::net::raw_uv_ostream& buf,
                                  std::string_view deviceName) {
  SetupSendHeader(buf, CONTROL_DEVICE_NAME);
  buf << static_cast<uint8_t>(deviceName.size());
  for (char c : deviceName) {
    buf << static_cast<uint8_t>(c);
  }
  m_xrp_bound_seq++;
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

// WPILib Sim Handlers
void XRP::HandleDriverStationSimValueChanged(const wpi::util::json& data) {
  auto dsData = data.lookup("data");
  if (!dsData || !dsData->is_object()) {
    return;
  }
  auto enabled = dsData->lookup(">enabled");
  if (enabled && enabled->is_bool()) {
    m_robot_enabled = enabled->get_bool();
  }
}

void XRP::HandleMotorSimValueChanged(const wpi::util::json& data) {
  int deviceId = -1;
  auto motorData = data.lookup("data");
  if (!motorData || !motorData->is_object()) {
    return;
  }

  auto device = data.lookup("device");
  if (!device || !device->is_string()) {
    return;
  }

  auto& deviceStr = device->get_string();
  if (deviceStr == "motorL") {
    deviceId = 0;
  } else if (deviceStr == "motorR") {
    deviceId = 1;
  } else if (deviceStr == "motor3") {
    deviceId = 2;
  } else if (deviceStr == "motor4") {
    deviceId = 3;
  }

  auto throttle = motorData->lookup("<throttle");
  if (deviceId != -1 && throttle && throttle->is_number()) {
    m_motor_outputs[deviceId] = throttle->get_number();
  }
}

void XRP::HandleServoSimValueChanged(const wpi::util::json& data) {
  int deviceId = -1;
  auto servoData = data.lookup("data");
  if (!servoData || !servoData->is_object()) {
    return;
  }

  auto device = data.lookup("device");
  if (!device || !device->is_string()) {
    return;
  }

  auto& deviceStr = device->get_string();
  if (deviceStr == "servo1") {
    deviceId = 4;
  } else if (deviceStr == "servo2") {
    deviceId = 5;
  } else if (deviceStr == "servo3") {
    deviceId = 6;
  } else if (deviceStr == "servo4") {
    deviceId = 7;
  }

  auto position = servoData->lookup("<position");
  if (deviceId != -1 && position && position->is_number()) {
    m_servo_outputs[deviceId] = position->get_number();
  }
}

void XRP::HandleDIOSimValueChanged(const wpi::util::json& data) {
  int deviceId = -1;
  auto dioData = data.lookup("data");

  auto device = data.lookup("device");
  if (!device || !device->is_string()) {
    return;
  }
  try {
    deviceId = std::stoi(device->get_string());
  } catch (const std::invalid_argument&) {
    deviceId = -1;
  }

  // Bail out early if device ID is invalid or if it's "spoken for"
  if (deviceId == -1) {
    return;
  }

  auto init = dioData->lookup("<init");
  if (init && init->is_bool() && init->get_bool()) {
    // All DIOs are initialized as inputs by default
    m_digital_inputs.emplace(deviceId, false);
  }

  auto input = dioData->lookup("<input");
  if (input && input->is_bool() && !input->get_bool()) {
    // We're registering an output device
    // Remove from the digital inputs list (if present)
    m_digital_inputs.erase(deviceId);
    m_digital_outputs.emplace(deviceId, false);
  }

  auto value = dioData->lookup("<>value");
  if (value && value->is_bool() && m_digital_outputs.count(deviceId) > 0) {
    m_digital_outputs[deviceId] = value->get_bool();
  }
}

void XRP::HandleGyroSimValueChanged(const wpi::util::json& data) {
  auto name = data.lookup("device");
  if (name && name->is_string()) {
    m_gyro_name = name->get_string();
  }
}

void XRP::HandleEncoderSimValueChanged(const wpi::util::json& data) {
  // We need to handle the various encoder cases
  // 4/5 -> Encoder 0
  // 6/7 -> Encoder 1
  // 8/9 -> Encoder 2
  // 10/11 -> Encoder 3
  int deviceId = -1;
  auto encData = data.lookup("data");
  if (!encData || !encData->is_object()) {
    return;
  }

  auto device = data.lookup("device");
  if (!device || !device->is_string()) {
    return;
  }

  try {
    deviceId = std::stoi(device->get_string());
  } catch (const std::invalid_argument&) {
    deviceId = -1;
  }

  if (deviceId == -1) {
    return;
  }

  auto init = encData->lookup("<init");
  auto jchA = encData->lookup("<channel_a");
  auto jchB = encData->lookup("<channel_b");
  if (init && init->is_bool() && init->get_bool() && jchA && jchA->is_int() &&
      jchB && jchB->is_int()) {
    // The <channel_a and <channel_b values come with the init message
    int chA = jchA->get_int();
    int chB = jchB->get_int();

    if ((chA == 4 && chB == 5) || (chA == 5 && chB == 4)) {
      m_encoder_channel_map.emplace(0, deviceId);
    } else if ((chA == 6 && chB == 7) || (chA == 7 && chB == 6)) {
      m_encoder_channel_map.emplace(1, deviceId);
    } else if ((chA == 8 && chB == 9) || (chA == 9 && chB == 8)) {
      m_encoder_channel_map.emplace(2, deviceId);
    } else if ((chA == 10 && chB == 11) || (chA == 11 && chB == 10)) {
      m_encoder_channel_map.emplace(3, deviceId);
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

  // Make the json object
  wpi::util::json gyroJson;
  gyroJson["type"] = "Gyro";
  gyroJson["device"] = m_gyro_name;
  auto data = wpi::util::json::object();
  data[">rate_x"] = rate_x;
  data[">rate_y"] = rate_y;
  data[">rate_z"] = rate_z;
  data[">angle_x"] = angle_x;
  data[">angle_y"] = angle_y;
  data[">angle_z"] = angle_z;
  gyroJson["data"] = std::move(data);

  // Update WPILib
  m_wpilib_update_func(gyroJson);
}

void XRP::ReadAccelData(std::span<const uint8_t> packet) {
  if (packet.size() < 12) {
    return;
  }

  std::scoped_lock lock(m_data_snapshot_mutex);
  auto& accel = m_data_snapshot.status.accel;
  accel.value = {ReadFloat(packet, 0), ReadFloat(packet, 4),
                 ReadFloat(packet, 8)};
  accel.present = true;
  accel.lastUpdate = std::chrono::steady_clock::now();
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
    wpi::util::json dioJson;
    dioJson["type"] = "DIO";
    dioJson["device"] = std::to_string(channel);
    dioJson["data"] =
        wpi::util::json::object("<>value", (valueMask & bit) != 0);

    m_wpilib_update_func(dioJson);
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
  if (period_numerator != std::numeric_limits<uint32_t>::max()) {
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

  // Look up the registered encoders
  if (m_encoder_channel_map.count(encoderId) == 0) {
    return;
  }

  uint8_t wpilibEncoderChannel = m_encoder_channel_map[encoderId];

  wpi::util::json encJson;
  encJson["type"] = "Encoder";
  encJson["device"] = std::to_string(wpilibEncoderChannel);
  encJson["data"] = wpi::util::json::object(">count", count);

  if (encoderData.periodValid) {
    encJson["data"].emplace_back(
        wpi::util::json::object(">period", encoderData.period));
  }
  m_wpilib_update_func(encJson);
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

  wpi::util::json analogJson;
  analogJson["type"] = "AI";
  analogJson["device"] = std::to_string(analogId);
  analogJson["data"] = wpi::util::json::object(">voltage", voltage);

  m_wpilib_update_func(analogJson);
}
