// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/DriverStationTypes.h"
#include "wpi/util/struct/Struct.hpp"

namespace wpi::hal {

/**
 * The overall robot mode (not including enabled state).
 */
enum class RobotMode {
  /// Unknown.
  UNKNOWN = HAL_ROBOTMODE_UNKNOWN,
  /// Autonomous.
  AUTONOMOUS = HAL_ROBOTMODE_AUTONOMOUS,
  /// Teleoperated.
  TELEOPERATED = HAL_ROBOTMODE_TELEOPERATED,
  /// Test.
  TEST = HAL_ROBOTMODE_TEST
};

/**
 * A wrapper around Driver Station control word.
 */
class ControlWord {
 public:
  /**
   * Default constructor.
   */
  ControlWord() = default;

  /**
   * Constructs from state values.
   *
   * @param opModeHash opmode hash
   * @param robotMode robot mode
   * @param enabled enabled
   * @param eStop emergency stopped
   * @param fmsAttached FMS attached
   * @param dsAttached DS attached
   */
  ControlWord(int64_t opModeHash, RobotMode robotMode, bool enabled, bool eStop,
              bool fmsAttached, bool dsAttached)
      : m_word{HAL_MakeControlWord(opModeHash,
                                   static_cast<HAL_RobotMode>(robotMode),
                                   enabled, eStop, fmsAttached, dsAttached)} {}

  /**
   * Constructs from the native HAL value.
   *
   * @param word value
   */
  explicit ControlWord(HAL_ControlWord word) : m_word{word} {}

  ControlWord(const ControlWord&) = default;
  ControlWord& operator=(const ControlWord&) = default;

  /**
   * Updates from state values.
   *
   * @param opModeHash opmode hash
   * @param robotMode robot mode
   * @param enabled enabled
   * @param eStop emergency stopped
   * @param fmsAttached FMS attached
   * @param dsAttached DS attached
   */
  void Update(int64_t opModeHash, RobotMode robotMode, bool enabled, bool eStop,
              bool fmsAttached, bool dsAttached) {
    m_word =
        HAL_MakeControlWord(opModeHash, static_cast<HAL_RobotMode>(robotMode),
                            enabled, eStop, fmsAttached, dsAttached);
  }

  /**
   * Updates from the native HAL value.
   *
   * @param word value
   */
  void Update(HAL_ControlWord word) { m_word = word; }

  /**
   * Check if the DS has enabled the robot.
   *
   * @return True if the robot is enabled and the DS is connected
   */
  bool IsEnabled() const { return HAL_ControlWord_IsEnabled(m_word); }

  /**
   * Gets the current robot mode.
   *
   * <p>Note that this does not indicate whether the robot is enabled or
   * disabled.
   *
   * @return robot mode
   */
  RobotMode GetRobotMode() const {
    return static_cast<RobotMode>(HAL_ControlWord_GetRobotMode(m_word));
  }

  /**
   * Gets the current opmode ID.
   *
   * @return opmode
   */
  int64_t GetOpModeId() const { return HAL_ControlWord_GetOpModeId(m_word); }

  /**
   * Sets the opmode ID.
   *
   * @param id opmode ID
   */
  void SetOpModeId(int64_t id) { HAL_ControlWord_SetOpModeId(&m_word, id); }

  /**
   * Check if the robot is e-stopped.
   *
   * @return True if the robot is e-stopped
   */
  bool IsEStopped() const { return HAL_ControlWord_IsEStopped(m_word); }

  /**
   * Is the driver station attached to a Field Management System?
   *
   * @return True if the robot is competing on a field being controlled by a
   *         Field Management System
   */
  bool IsFMSAttached() const { return HAL_ControlWord_IsFMSAttached(m_word); }

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  bool IsDSAttached() const { return HAL_ControlWord_IsDSAttached(m_word); }

  /**
   * Check if the DS is commanding autonomous mode.
   *
   * @return True if the robot is being commanded to be in autonomous mode
   */
  bool IsAutonomous() const { return GetRobotMode() == RobotMode::AUTONOMOUS; }

  /**
   * Check if the DS is commanding autonomous mode and if it has enabled the
   * robot.
   *
   * @return True if the robot is being commanded to be in autonomous mode and
   * enabled.
   */
  bool IsAutonomousEnabled() const {
    return IsAutonomous() && IsEnabled() && IsDSAttached();
  }

  /**
   * Check if the DS is commanding teleop mode.
   *
   * @return True if the robot is being commanded to be in teleop mode
   */
  bool IsTeleop() const { return GetRobotMode() == RobotMode::TELEOPERATED; }

  /**
   * Check if the DS is commanding teleop mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in teleop mode and
   * enabled.
   */
  bool IsTeleopEnabled() const {
    return IsTeleop() && IsEnabled() && IsDSAttached();
  }

  /**
   * Check if the DS is commanding test mode.
   *
   * @return True if the robot is being commanded to be in test mode
   */
  bool IsTest() const { return GetRobotMode() == RobotMode::TEST; }

  /**
   * Check if the DS is commanding test mode and if it has enabled the robot.
   *
   * @return True if the robot is being commanded to be in test mode and
   * enabled.
   */
  bool IsTestEnabled() const {
    return IsTest() && IsEnabled() && IsDSAttached();
  }

  /**
   * Get the HAL raw value.
   *
   * @return Control word value
   */
  HAL_ControlWord GetValue() const { return m_word; }

 private:
  HAL_ControlWord m_word{.value = 0};
};

inline bool operator==(const ControlWord& lhs, const ControlWord& rhs) {
  return lhs.GetValue().value == rhs.GetValue().value;
}

inline bool operator!=(const ControlWord& lhs, const ControlWord& rhs) {
  return !(lhs == rhs);
}

}  // namespace wpi::hal

template <>
struct wpi::util::Struct<wpi::hal::ControlWord> {
  static constexpr std::string_view GetTypeName() { return "ControlWord"; }
  static constexpr size_t GetSize() { return 8; }
  static constexpr std::string_view GetSchema() {
    return "uint64 opModeHash:56;"
           "enum{unknown=0,autonomous=1,teleoperated=2,test=3}"
           "uint64 robotMode:2;"
           "bool enabled:1;bool eStop:1;bool fmsAttached:1;bool dsAttached:1;";
  }

  static inline wpi::hal::ControlWord Unpack(std::span<const uint8_t> data) {
    return wpi::hal::ControlWord{
        {.value = wpi::util::UnpackStruct<int64_t>(data)}};
  }
  static inline void Pack(std::span<uint8_t> data,
                          wpi::hal::ControlWord value) {
    wpi::util::PackStruct(data, value.GetValue().value);
  }
};

static_assert(wpi::util::StructSerializable<wpi::hal::ControlWord>);
