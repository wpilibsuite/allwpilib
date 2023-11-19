// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "XRP.h"

#include <bit>
#include <string>

#include <fmt/format.h>
#include <wpi/Endian.h>
#include <wpi/json.h>

using namespace wpilibxrp;

XRP::XRP()
    : m_gyro_name{"XRPGyro"}, m_wpilib_update_func([](const wpi::json&) {}) {
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

void XRP::HandleWPILibUpdate(const wpi::json& data) {
  if (data.count("type") == 0) {
    return;
  }

  if (data["type"] == "DriverStation") {
    HandleDriverStationSimValueChanged(data);
  } else if (data["type"] == "XRPMotor") {
    HandleMotorSimValueChanged(data);
  } else if (data["type"] == "XRPServo") {
    HandleServoSimValueChanged(data);
  } else if (data["type"] == "DIO") {
    HandleDIOSimValueChanged(data);
  } else if (data["type"] == "Gyro") {
    HandleGyroSimValueChanged(data);
  } else if (data["type"] == "Encoder") {
    HandleEncoderSimValueChanged(data);
  }
}

void XRP::HandleXRPUpdate(std::span<const uint8_t> packet) {
  uint16_t seq = (packet[0] << 8) + packet[1];

  if (seq <= m_wpilib_bound_seq) {
    // If the old sequence was within 3 or uint16_t max and the new
    // sequence is < 3 - we've prob rolled over
    if (!((0xFFFF - m_wpilib_bound_seq < 3) && seq < 3)) {
      return;
    }
  }

  m_wpilib_bound_seq = seq;

  // Tagged data starts at byte 3
  packet = packet.subspan(3);

  // Loop to handle multiple tags
  while (!packet.empty()) {
    auto tagLength = packet[0];
    auto tagPacket = packet.subspan(0, tagLength + 1);

    // NOTE: tagPacket contains the size and tag bytes as well
    // Verify that the packet is indeed the right size
    if (tagPacket.size() != static_cast<size_t>(tagLength + 1)) {
      break;
    }

    switch (packet[1]) {
      case XRP_TAG_GYRO:
        ReadGyroTag(tagPacket);
        break;
      case XRP_TAG_ACCEL:
        ReadAccelTag(tagPacket);
        break;
      case XRP_TAG_DIO:
        ReadDIOTag(tagPacket);
        break;
      case XRP_TAG_ENCODER:
        ReadEncoderTag(tagPacket);
        break;
      case XRP_TAG_ANALOG:
        ReadAnalogTag(tagPacket);
        break;
    }

    packet = packet.subspan(tagLength + 1);
  }
}

void XRP::SetupXRPSendBuffer(wpi::raw_uv_ostream& buf) {
  SetupSendHeader(buf);
  SetupMotorTag(buf);
  SetupServoTag(buf);
  SetupDigitalOutTag(buf);
  m_xrp_bound_seq++;
}

// WPILib Sim Handlers
void XRP::HandleDriverStationSimValueChanged(const wpi::json& data) {
  auto dsData = data["data"];
  if (dsData.find(">enabled") != dsData.end()) {
    m_robot_enabled = dsData[">enabled"];
  }
}

void XRP::HandleMotorSimValueChanged(const wpi::json& data) {
  int deviceId = -1;
  auto motorData = data["data"];

  if (data["device"] == "motorL") {
    deviceId = 0;
  } else if (data["device"] == "motorR") {
    deviceId = 1;
  } else if (data["device"] == "motor3") {
    deviceId = 2;
  } else if (data["device"] == "motor4") {
    deviceId = 3;
  }

  if (deviceId != -1 && motorData.find("<speed") != motorData.end()) {
    m_motor_outputs[deviceId] = motorData["<speed"];
  }
}

void XRP::HandleServoSimValueChanged(const wpi::json& data) {
  int deviceId = -1;
  auto servoData = data["data"];

  if (data["device"] == "servo1") {
    deviceId = 4;
  } else if (data["device"] == "servo2") {
    deviceId = 5;
  }

  if (deviceId != -1 && servoData.find("<position") != servoData.end()) {
    m_servo_outputs[deviceId] = servoData["<position"];
  }
}

void XRP::HandleDIOSimValueChanged(const wpi::json& data) {
  int deviceId = -1;
  auto dioData = data["data"];

  try {
    deviceId = std::stoi(data["device"].get<std::string>());
  } catch (const std::invalid_argument&) {
    deviceId = -1;
  }

  // Bail out early if device ID is invalid or if it's "spoken for"
  if (deviceId == -1) {
    return;
  }

  if (dioData.find("<init") != dioData.end() && dioData["<init"]) {
    // All DIOs are initialized as inputs by default
    m_digital_inputs.emplace(deviceId, false);
  }

  if (dioData.find("<input") != dioData.end() && dioData["<input"] == false) {
    // We're registering an output device
    // Remove from the digital inputs list (if present)
    m_digital_inputs.erase(deviceId);
    m_digital_outputs.emplace(deviceId, false);
  }

  if (dioData.find("<>value") != dioData.end() &&
      m_digital_outputs.count(deviceId) > 0) {
    m_digital_outputs[deviceId] = dioData["<>value"];
  }
}

void XRP::HandleGyroSimValueChanged(const wpi::json& data) {
  m_gyro_name = data["device"].get<std::string>();
}

void XRP::HandleEncoderSimValueChanged(const wpi::json& data) {
  // We need to handle the various encoder cases
  // 4/5 -> Encoder 0
  // 6/7 -> Encoder 1
  // 8/9 -> Encoder 2
  // 10/11 -> Encoder 3
  int deviceId = -1;
  auto encData = data["data"];

  try {
    deviceId = std::stoi(data["device"].get<std::string>());
  } catch (const std::invalid_argument&) {
    deviceId = -1;
  }

  if (deviceId == -1) {
    return;
  }

  if (encData.find("<init") != encData.end() && encData["<init"]) {
    // The <channel_a and <channel_b values come with the init message
    int chA = encData["<channel_a"];
    int chB = encData["<channel_b"];

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

void XRP::SetupSendHeader(wpi::raw_uv_ostream& buf) {
  uint8_t pktSeq[2];
  wpi::support::endian::write16be(pktSeq, m_xrp_bound_seq);

  buf << pktSeq[0] << pktSeq[1]
      << static_cast<uint8_t>(m_robot_enabled ? 1 : 0);
}

void XRP::SetupMotorTag(wpi::raw_uv_ostream& buf) {
  uint8_t value[4];

  for (auto motor : m_motor_outputs) {
    // Motor payload is 6 bytes
    buf << static_cast<uint8_t>(6)              // Size
        << static_cast<uint8_t>(XRP_TAG_MOTOR)  // Tag
        << static_cast<uint8_t>(motor.first);   // Channel

    // Convert the value
    wpi::support::endian::write32be(value,
                                    std::bit_cast<uint32_t>(motor.second));
    buf << value[0] << value[1] << value[2] << value[3];
  }
}

void XRP::SetupServoTag(wpi::raw_uv_ostream& buf) {
  uint8_t value[4];

  for (auto servo : m_servo_outputs) {
    // Servo payload is 6 bytes
    buf << static_cast<uint8_t>(6)              // Size
        << static_cast<uint8_t>(XRP_TAG_SERVO)  // Tag
        << static_cast<uint8_t>(servo.first);   // Channel

    // Convert the value
    wpi::support::endian::write32be(value,
                                    std::bit_cast<uint32_t>(servo.second));
    buf << value[0] << value[1] << value[2] << value[3];
  }
}

void XRP::SetupDigitalOutTag(wpi::raw_uv_ostream& buf) {
  for (auto digitalOut : m_digital_outputs) {
    // DIO payload is 3 bytes
    buf << static_cast<uint8_t>(3)                           // Size
        << static_cast<uint8_t>(XRP_TAG_DIO)                 // Tag
        << static_cast<uint8_t>(digitalOut.first)            // Channel
        << static_cast<uint8_t>(digitalOut.second ? 1 : 0);  // Value
  }
}

void XRP::ReadGyroTag(std::span<const uint8_t> packet) {
  if (packet.size() < 26) {
    return;  // size(1) + tag(1) + 6x 4byte
  }

  packet = packet.subspan(2);  // Skip past the size and tag
  float rate_x =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[0]));
  float rate_y =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[4]));
  float rate_z =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[8]));
  float angle_x =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[12]));
  float angle_y =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[16]));
  float angle_z =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[20]));

  // Make the json object
  wpi::json gyroJson;
  gyroJson["type"] = "Gyro";
  gyroJson["device"] = m_gyro_name;
  gyroJson["data"] = {{">rate_x", rate_x},   {">rate_y", rate_y},
                      {">rate_z", rate_z},   {">angle_x", angle_x},
                      {">angle_y", angle_y}, {">angle_z", angle_z}};

  // Update WPILib
  m_wpilib_update_func(gyroJson);
}

void XRP::ReadAccelTag(std::span<const uint8_t> packet) {
  if (packet.size() < 14) {
    return;  // size(1) + tag(1) + 3x 4 byte
  }

  packet = packet.subspan(2);  // Skip past the size and tag
  float accel_x =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[0]));
  float accel_y =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[4]));
  float accel_z =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[8]));

  wpi::json accelJson;
  accelJson["type"] = "Accel";
  accelJson["device"] = "BuiltInAccel";
  accelJson["data"] = {{">x", accel_x}, {">y", accel_y}, {">z", accel_z}};

  // Update WPILib
  m_wpilib_update_func(accelJson);
}

void XRP::ReadDIOTag(std::span<const uint8_t> packet) {
  if (packet.size() < 4) {
    return;  // size(1) + tag(1) + id(1) + value(1)
  }

  wpi::json dioJson;
  dioJson["type"] = "DIO";
  dioJson["device"] = std::to_string(packet[2]);
  dioJson["data"] = {{"<>value", packet[3] == 1}};

  m_wpilib_update_func(dioJson);
}

void XRP::ReadEncoderTag(std::span<const uint8_t> packet) {
  if (packet.size() < 7) {
    return;  // size(1) + tag(1) + id(1) + count(4)
  }

  // size(1) + tag(1) + id(1) + count(4) + period_numerator(4) +
  // period_denominator(4)
  bool containsPeriod = packet.size() >= 15;

  uint8_t encoderId = packet[2];

  packet = packet.subspan(3);  // Skip past the size and tag and ID
  int32_t count =
      static_cast<int32_t>(wpi::support::endian::read32be(&packet[0]));

  // Look up the registered encoders
  if (m_encoder_channel_map.count(encoderId) == 0) {
    return;
  }

  uint8_t wpilibEncoderChannel = m_encoder_channel_map[encoderId];

  wpi::json encJson;
  encJson["type"] = "Encoder";
  encJson["device"] = std::to_string(wpilibEncoderChannel);
  encJson["data"] = {{">count", count}};

  if (containsPeriod) {
    // Older versions of XRP firmware do not provide Encoder Period.
    // Only calculate period if the Encoder packet contains the period data.

    // The period is a fraction consisting of two integers: a numerator and
    // denominator. The least significant bit of the numerator specifies the
    // direction: 1 -> forward, 0 -> reverse.  The direction bit must be removed
    // to calculate the magnitude of the period.

    size_t i = sizeof(count);
    uint32_t period_numerator =
        static_cast<uint32_t>(wpi::support::endian::read32be(&packet[i]));

    i += sizeof(period_numerator);
    uint32_t period_denominator =
        static_cast<uint32_t>(wpi::support::endian::read32be(&packet[i]));

    double period =
        static_cast<double>(period_numerator >> 1) / period_denominator;

    // If direction is not forward, return negative value for period.
    if (!(period_numerator & 1)) {
      period = -period;
    }

    encJson["data"].push_back({wpi::json(">period"), wpi::json(period)});
  }

  m_wpilib_update_func(encJson);
}

void XRP::ReadAnalogTag(std::span<const uint8_t> packet) {
  if (packet.size() < 7) {
    return;  // size(1) + tag(1) + id(1) + float
  }

  uint8_t analogId = packet[2];

  packet = packet.subspan(3);
  float voltage =
      std::bit_cast<float>(wpi::support::endian::read32be(&packet[0]));

  wpi::json analogJson;
  analogJson["type"] = "AI";
  analogJson["device"] = std::to_string(analogId);
  analogJson["data"] = {{">voltage", voltage}};

  m_wpilib_update_func(analogJson);
}
