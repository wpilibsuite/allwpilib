// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

#ifdef __cplusplus
#include <wpi/struct/Struct.h>
#endif  // __cplusplus

/**
 * @defgroup hal_driverstation Driver Station Functions
 * @ingroup hal_capi
 * @{
 */

struct HAL_ControlWord {
  uint32_t enabled : 1;
  uint32_t robotMode : 2;  // HAL_RobotMode enum
  uint32_t eStop : 1;
  uint32_t fmsAttached : 1;
  uint32_t dsAttached : 1;
  uint32_t control_reserved : 26;
};
typedef struct HAL_ControlWord HAL_ControlWord;

HAL_ENUM(HAL_AllianceStationID) {
  /** Unknown Alliance Station */
  HAL_AllianceStationID_kUnknown = 0,
  /** Red Alliance Station 1 */
  HAL_AllianceStationID_kRed1,
  /** Red Alliance Station 2 */
  HAL_AllianceStationID_kRed2,
  /** Red Alliance Station 3 */
  HAL_AllianceStationID_kRed3,
  /** Blue Alliance Station 1 */
  HAL_AllianceStationID_kBlue1,
  /** Blue Alliance Station 2 */
  HAL_AllianceStationID_kBlue2,
  /** Blue Alliance Station 3 */
  HAL_AllianceStationID_kBlue3,
};

HAL_ENUM(HAL_MatchType) {
  HAL_kMatchType_none = 0,
  HAL_kMatchType_practice,
  HAL_kMatchType_qualification,
  HAL_kMatchType_elimination,
};

HAL_ENUM(HAL_RobotMode) {
  HAL_ROBOTMODE_UNKNOWN = 0,
  HAL_ROBOTMODE_AUTONOMOUS,
  HAL_ROBOTMODE_TELEOPERATED,
  HAL_ROBOTMODE_TEST,
};

/**
 * The maximum number of axes that will be stored in a single HAL_JoystickAxes
 * struct. This is used for allocating buffers, not bounds checking, since there
 * are usually less axes in practice.
 */
#define HAL_kMaxJoystickAxes 12
/**
 * The maximum number of POVs that will be stored in a single HAL_JoystickPOVs
 * struct. This is used for allocating buffers, not bounds checking, since there
 * are usually less POVs in practice.
 */
#define HAL_kMaxJoystickPOVs 8
/**
 * The maximum number of joysticks.
 */
#define HAL_kMaxJoysticks 6

struct HAL_JoystickAxes {
  int16_t count;
  float axes[HAL_kMaxJoystickAxes];
  int16_t raw[HAL_kMaxJoystickAxes];
};
typedef struct HAL_JoystickAxes HAL_JoystickAxes;

HAL_ENUM_WITH_UNDERLYING_TYPE(HAL_JoystickPOV, uint8_t){
    /** Centered */
    HAL_JoystickPOV_kCentered = 0x00u,
    /** Up */
    HAL_JoystickPOV_kUp = 0x01u,
    /** Right */
    HAL_JoystickPOV_kRight = 0x02u,
    /** Down */
    HAL_JoystickPOV_kDown = 0x04u,
    /** Left */
    HAL_JoystickPOV_kLeft = 0x08u,
    /** Right-Up */
    HAL_JoystickPOV_kRightUp = HAL_JoystickPOV_kRight | HAL_JoystickPOV_kUp,
    /** Right-Down */
    HAL_JoystickPOV_kRightDown = HAL_JoystickPOV_kRight | HAL_JoystickPOV_kDown,
    /** Left-Up */
    HAL_JoystickPOV_kLeftUp = HAL_JoystickPOV_kLeft | HAL_JoystickPOV_kUp,
    /** Left-Down */
    HAL_JoystickPOV_kLeftDown = HAL_JoystickPOV_kLeft | HAL_JoystickPOV_kDown,
};

struct HAL_JoystickPOVs {
  int16_t count;
  HAL_JoystickPOV povs[HAL_kMaxJoystickPOVs];
};
typedef struct HAL_JoystickPOVs HAL_JoystickPOVs;

struct HAL_JoystickButtons {
  uint32_t buttons;
  uint8_t count;
};
typedef struct HAL_JoystickButtons HAL_JoystickButtons;

struct HAL_JoystickDescriptor {
  uint8_t isGamepad;
  uint8_t type;
  char name[256];
  uint8_t axisCount;
  uint8_t axisTypes[HAL_kMaxJoystickAxes];
  uint8_t buttonCount;
  uint8_t povCount;
};
typedef struct HAL_JoystickDescriptor HAL_JoystickDescriptor;

struct HAL_MatchInfo {
  char eventName[64];
  HAL_MatchType matchType;
  uint16_t matchNumber;
  uint8_t replayNumber;
  uint8_t gameSpecificMessage[64];
  uint16_t gameSpecificMessageSize;
};
typedef struct HAL_MatchInfo HAL_MatchInfo;
/** @} */

#ifdef __cplusplus
template <>
struct wpi::Struct<HAL_ControlWord> {
  static constexpr std::string_view GetTypeName() { return "ControlWord"; }
  static constexpr size_t GetSize() { return 1; }
  static constexpr std::string_view GetSchema() {
    return "bool enabled:1;bool eStop:1;bool fmsAttached:1;"
           "bool dsAttached:1;"
           "enum{unknown=0,autonomous=1,teleoperated=2,test=3}"
           "int8 robotMode:2";
  }

  static inline HAL_ControlWord Unpack(std::span<const uint8_t> data) {
    uint8_t b = data[0];
    return {.enabled = static_cast<uint32_t>(b & 0x01),
            .robotMode = static_cast<uint32_t>(b >> 4),
            .eStop = static_cast<uint32_t>(b >> 1),
            .fmsAttached = static_cast<uint32_t>(b >> 2),
            .dsAttached = static_cast<uint32_t>(b >> 3),
            .control_reserved = 0};
  }
  static inline void Pack(std::span<uint8_t> data,
                          const HAL_ControlWord& value) {
    data[0] = (value.enabled ? 0x01u : 0u) | (value.eStop ? 0x02u : 0u) |
              (value.fmsAttached ? 0x04u : 0u) |
              (value.dsAttached ? 0x08u : 0u) | (value.robotMode << 4);
  }
};

static_assert(wpi::StructSerializable<HAL_ControlWord>);
#endif  // __cplusplus
