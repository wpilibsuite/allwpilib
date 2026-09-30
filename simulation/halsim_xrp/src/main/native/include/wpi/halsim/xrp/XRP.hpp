// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <span>
#include <string_view>

#include "wpi/net/raw_uv_ostream.hpp"

namespace wpilibxrp {

constexpr int PACKET_HEADER_SIZE = 5;
constexpr uint32_t ENCODER_PERIOD_DENOMINATOR = 1000000;
constexpr uint32_t CONTROL_RX_AGE_UNIT_US = 10;
constexpr uint16_t INVALID_CONTROL_RX_AGE_10_US = UINT16_MAX;
constexpr int16_t MOTOR_MAX_PWM = 255;
constexpr uint8_t SERVO_MAX_DEGREES = 180;
constexpr uint16_t ANALOG_MAX_VALUE = UINT16_MAX;
constexpr float ANALOG_MAX_VOLTAGE = 5.0f;

constexpr uint16_t CONTROL_MOTOR_0 = 1u << 0;
constexpr uint16_t CONTROL_MOTOR_1 = 1u << 1;
constexpr uint16_t CONTROL_MOTOR_2 = 1u << 2;
constexpr uint16_t CONTROL_MOTOR_3 = 1u << 3;
constexpr uint16_t CONTROL_SERVO_4 = 1u << 4;
constexpr uint16_t CONTROL_SERVO_5 = 1u << 5;
constexpr uint16_t CONTROL_SERVO_6 = 1u << 6;
constexpr uint16_t CONTROL_SERVO_7 = 1u << 7;
constexpr uint16_t CONTROL_DIO = 1u << 8;
constexpr uint16_t CONTROL_IDENTIFY = 1u << 14;
constexpr uint16_t CONTROL_DEVICE_NAME = 1u << 15;
constexpr uint8_t CONTROL_DEVICE_NAME_MAX_LENGTH = 26;
constexpr uint16_t CONTROL_ALL_FIELDS =
    CONTROL_MOTOR_0 | CONTROL_MOTOR_1 | CONTROL_MOTOR_2 | CONTROL_MOTOR_3 |
    CONTROL_SERVO_4 | CONTROL_SERVO_5 | CONTROL_SERVO_6 | CONTROL_SERVO_7 |
    CONTROL_DIO | CONTROL_IDENTIFY | CONTROL_DEVICE_NAME;

constexpr uint16_t STATUS_ENCODER_0 = 1u << 0;
constexpr uint16_t STATUS_ENCODER_1 = 1u << 1;
constexpr uint16_t STATUS_ENCODER_2 = 1u << 2;
constexpr uint16_t STATUS_ENCODER_3 = 1u << 3;
constexpr uint16_t STATUS_DIO = 1u << 4;
constexpr uint16_t STATUS_GYRO = 1u << 5;
constexpr uint16_t STATUS_ACCEL = 1u << 6;
constexpr uint16_t STATUS_ANALOG_0 = 1u << 7;
constexpr uint16_t STATUS_ANALOG_1 = 1u << 8;
constexpr uint16_t STATUS_ANALOG_2 = 1u << 9;
constexpr uint16_t STATUS_TIMING = 1u << 10;
constexpr uint16_t STATUS_COMMAND_ACK = 1u << 11;
constexpr uint16_t STATUS_ALL_FIELDS =
    STATUS_ENCODER_0 | STATUS_ENCODER_1 | STATUS_ENCODER_2 | STATUS_ENCODER_3 |
    STATUS_DIO | STATUS_GYRO | STATUS_ACCEL | STATUS_ANALOG_0 |
    STATUS_ANALOG_1 | STATUS_ANALOG_2 | STATUS_TIMING | STATUS_COMMAND_ACK;
constexpr uint8_t COMMAND_ACK_SUCCESS = 0;
constexpr uint8_t COMMAND_ACK_REJECTED = 1;

template <typename T>
struct XRPDataField {
  T value{};
  bool present = false;
  std::chrono::steady_clock::time_point lastUpdate;
};

struct XRPPacketInfo {
  bool present = false;
  uint16_t sequence = 0;
  uint16_t fieldMask = 0;
  std::chrono::steady_clock::time_point lastUpdate;
};

struct XRPAxisData {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};

struct XRPGyroData {
  XRPAxisData rate;
  XRPAxisData angle;
};

struct XRPEncoderData {
  int32_t count = 0;
  double period = 0.0;
  bool periodValid = false;
};

struct XRPCommandAckData {
  uint16_t controlSeq = 0;
  uint16_t controlFieldMask = 0;
  uint8_t result = COMMAND_ACK_REJECTED;
};

struct XRPControlData {
  XRPPacketInfo packet;
  bool enabled = false;
  std::array<XRPDataField<float>, 4> motors;
  std::array<XRPDataField<float>, 4> servos;
  std::array<XRPDataField<bool>, 8> digitalOutputs;
};

struct XRPStatusData {
  XRPPacketInfo packet;
  std::array<XRPDataField<XRPEncoderData>, 4> encoders;
  std::array<XRPDataField<bool>, 8> digitalInputs;
  XRPDataField<XRPGyroData> gyro;
  XRPDataField<XRPAxisData> accel;
  std::array<XRPDataField<float>, 3> analogInputs;
  XRPDataField<XRPCommandAckData> commandAck;
};

struct XRPDataSnapshot {
  XRPControlData control;
  XRPStatusData status;
};

class XRP {
 public:
  /** Registers HAL simulation callbacks for encoder reset offsets. */
  XRP();
  /** Cancels HAL simulation callbacks. */
  ~XRP();
  XRP(const XRP&) = delete;
  XRP& operator=(const XRP&) = delete;

  /**
   * Applies an XRP status packet directly to HAL simulation.
   *
   * @param packet XRP status packet.
   * @return True if the packet is valid and newer than the last status packet.
   */
  bool HandleXRPUpdate(std::span<const uint8_t> packet);

  /**
   * Reads HAL simulation outputs and encodes an XRP control packet.
   *
   * @param buf output stream for the control packet.
   */
  void SetupXRPSendBuffer(wpi::net::raw_uv_ostream& buf);
  /**
   * Encodes a request to rename the connected XRP.
   *
   * @param buf output stream for the rename command.
   * @param deviceName Bluetooth name or suffix to send to the firmware.
   * @return Control sequence number used for the command acknowledgement.
   */
  uint16_t SetupRenameDeviceBuffer(wpi::net::raw_uv_ostream& buf,
                                   std::string_view deviceName);

  /**
   * Allows status packets from a new XRP connection to restart their sequence.
   */
  void ResetStatusPacketSequence();

  /**
   * Encodes a request to flash the onboard LED for five seconds.
   *
   * @param buf output stream for the identify command.
   * @return Control sequence number used for the command acknowledgement.
   */
  uint16_t SetupIdentifyBuffer(wpi::net::raw_uv_ostream& buf);

  /**
   * Gets a snapshot of the latest XRP control and status data.
   *
   * @return Current XRP control and status data.
   */
  XRPDataSnapshot GetDataSnapshot() const;

 private:
  // To XRP Methods
  uint16_t GetControlFieldMask() const;
  void SetupSendHeader(wpi::net::raw_uv_ostream& buf, uint16_t fieldMask);
  void SetupMotorFields(wpi::net::raw_uv_ostream& buf, uint16_t fieldMask);
  void SetupServoFields(wpi::net::raw_uv_ostream& buf, uint16_t fieldMask);
  void SetupDigitalOutFields(wpi::net::raw_uv_ostream& buf, uint16_t fieldMask);
  void RecordControlData(uint16_t fieldMask);

  void ReadHALOutputs();

  // XRP Packet Update Handlers
  void ReadGyroData(std::span<const uint8_t> packet);
  void ReadAccelData(std::span<const uint8_t> packet);
  void ReadDIOData(uint8_t presentMask, uint8_t valueMask);
  void ReadEncoderData(uint8_t encoderId, std::span<const uint8_t> packet);
  void ReadAnalogData(uint8_t analogId, std::span<const uint8_t> packet);
  void ReadCommandAckData(std::span<const uint8_t> packet);

  // Robot State
  std::map<uint8_t, bool> m_digital_outputs;
  std::map<uint8_t, float> m_motor_outputs;
  std::map<uint8_t, float> m_servo_outputs;

  struct EncoderSimData {
    int32_t index = 0;
    int32_t initializedCallback = 0;
    int32_t resetCallback = 0;
    std::atomic<int32_t> countOffset{0};
  };
  std::unique_ptr<EncoderSimData[]> m_encoders;

  uint16_t m_wpilib_bound_seq = 0;
  bool m_have_wpilib_bound_seq = false;
  uint16_t m_xrp_bound_seq = 0;

  bool m_robot_enabled = false;

  mutable std::mutex m_data_snapshot_mutex;
  XRPDataSnapshot m_data_snapshot;
};

}  // namespace wpilibxrp
