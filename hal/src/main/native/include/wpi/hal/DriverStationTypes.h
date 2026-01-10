// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"
#include "wpi/util/string.h"

/**
 * @defgroup hal_driverstation Driver Station Functions
 * @ingroup hal_capi
 * @{
 */

#define HAL_CONTROLWORD_OPMODE_HASH_MASK 0x00FFFFFFFFFFFFFFLL
#define HAL_CONTROLWORD_ROBOT_MODE_MASK 0x0300000000000000LL
#define HAL_CONTROLWORD_ROBOT_MODE_SHIFT 56
#define HAL_CONTROLWORD_ENABLED_MASK 0x0400000000000000LL
#define HAL_CONTROLWORD_ESTOP_MASK 0x0800000000000000LL
#define HAL_CONTROLWORD_FMS_ATTACHED_MASK 0x1000000000000000LL
#define HAL_CONTROLWORD_DS_ATTACHED_MASK 0x2000000000000000LL

struct HAL_ControlWord {
  int64_t value;
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
 * The maximum number of touchpads that will be stored in a single
 * HAL_JoystickTouchpads struct. This is used for allocating buffers, not
 * bounds checking, since there are usually less touchpads in practice.
 */
#define HAL_kMaxJoystickTouchpads 2

/**
 * The maximum number of fingers that will be stored in a single
 * HAL_JoystickTouchpad struct. This is used for allocating buffers, not
 * bounds checking, since there are usually less fingers in practice.
 */
#define HAL_kMaxJoystickTouchpadFingers 2

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
  uint16_t available;
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
  uint8_t available;
  HAL_JoystickPOV povs[HAL_kMaxJoystickPOVs];
};
typedef struct HAL_JoystickPOVs HAL_JoystickPOVs;

struct HAL_JoystickButtons {
  uint64_t buttons;
  uint64_t available;
};
typedef struct HAL_JoystickButtons HAL_JoystickButtons;

struct HAL_JoystickTouchpadFinger {
  uint8_t down;
  float x;
  float y;
};
typedef struct HAL_JoystickTouchpadFinger HAL_JoystickTouchpadFinger;

struct HAL_JoystickTouchpad {
  uint8_t count;
  HAL_JoystickTouchpadFinger fingers[HAL_kMaxJoystickTouchpadFingers];
};
typedef struct HAL_JoystickTouchpad HAL_JoystickTouchpad;

struct HAL_JoystickTouchpads {
  uint8_t count;
  HAL_JoystickTouchpad touchpads[HAL_kMaxJoystickTouchpads];
};
typedef struct HAL_JoystickTouchpads HAL_JoystickTouchpads;

struct HAL_JoystickDescriptor {
  uint8_t isGamepad;
  uint8_t gamepadType;
  uint8_t supportedOutputs;
  char name[256];
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

#define HAL_OPMODE_HASH_MASK HAL_CONTROLWORD_OPMODE_HASH_MASK
#define HAL_OPMODE_ROBOT_MODE_MASK HAL_CONTROLWORD_ROBOT_MODE_MASK
#define HAL_OPMODE_ROBOT_MODE_SHIFT HAL_CONTROLWORD_ROBOT_MODE_SHIFT

struct HAL_OpModeOption {
  int64_t id;  // encodes robot mode in bits 57-56, LSB 56 bits is hash of name
  struct WPI_String name;
  struct WPI_String group;
  struct WPI_String description;
  int32_t textColor;        // 0x00RRGGBB or -1 for default
  int32_t backgroundColor;  // 0x00RRGGBB or -1 for default
};
typedef struct HAL_OpModeOption HAL_OpModeOption;

#ifdef __cplusplus
extern "C" {
#endif

inline HAL_ControlWord HAL_MakeControlWord(int64_t opModeHash,
                                           HAL_RobotMode robotMode,
                                           HAL_Bool enabled, HAL_Bool eStop,
                                           HAL_Bool fmsAttached,
                                           HAL_Bool dsAttached) {
  HAL_ControlWord word;
  word.value =
      (opModeHash & HAL_CONTROLWORD_OPMODE_HASH_MASK) |
      (((uint64_t)(robotMode) << HAL_CONTROLWORD_ROBOT_MODE_SHIFT) &  // NOLINT
       HAL_CONTROLWORD_ROBOT_MODE_MASK) |
      (enabled ? HAL_CONTROLWORD_ENABLED_MASK : 0) |
      (eStop ? HAL_CONTROLWORD_ESTOP_MASK : 0) |
      (fmsAttached ? HAL_CONTROLWORD_FMS_ATTACHED_MASK : 0) |
      (dsAttached ? HAL_CONTROLWORD_DS_ATTACHED_MASK : 0);
  return word;
}

inline int64_t HAL_ControlWord_GetOpModeHash(HAL_ControlWord word) {
  return word.value & HAL_CONTROLWORD_OPMODE_HASH_MASK;
}

inline int64_t HAL_ControlWord_GetOpModeId(HAL_ControlWord word) {
  // if the hash portion is zero, return 0
  if ((word.value & HAL_CONTROLWORD_OPMODE_HASH_MASK) == 0) {
    return 0;
  }
  // otherwise return the full ID (which includes the robot mode)
  return word.value &
         (HAL_CONTROLWORD_OPMODE_HASH_MASK | HAL_CONTROLWORD_ROBOT_MODE_MASK);
}

inline void HAL_ControlWord_SetOpModeId(HAL_ControlWord* word, int64_t id) {
  // clear out the old hash and robot mode
  word->value &=
      ~(HAL_CONTROLWORD_OPMODE_HASH_MASK | HAL_CONTROLWORD_ROBOT_MODE_MASK);
  // set the new id
  word->value |=
      id & (HAL_CONTROLWORD_OPMODE_HASH_MASK | HAL_CONTROLWORD_ROBOT_MODE_MASK);
}

inline HAL_RobotMode HAL_ControlWord_GetRobotMode(HAL_ControlWord word) {
  // NOLINTBEGIN
  return (HAL_RobotMode)((word.value & HAL_CONTROLWORD_ROBOT_MODE_MASK) >>
                         HAL_CONTROLWORD_ROBOT_MODE_SHIFT);
  // NOLINTEND
}

inline HAL_Bool HAL_ControlWord_IsEnabled(HAL_ControlWord word) {
  return (word.value & HAL_CONTROLWORD_ENABLED_MASK) != 0;
}

inline HAL_Bool HAL_ControlWord_IsEStopped(HAL_ControlWord word) {
  return (word.value & HAL_CONTROLWORD_ESTOP_MASK) != 0;
}

inline HAL_Bool HAL_ControlWord_IsFMSAttached(HAL_ControlWord word) {
  return (word.value & HAL_CONTROLWORD_FMS_ATTACHED_MASK) != 0;
}

inline HAL_Bool HAL_ControlWord_IsDSAttached(HAL_ControlWord word) {
  return (word.value & HAL_CONTROLWORD_DS_ATTACHED_MASK) != 0;
}

// NOLINTBEGIN
// for use at compile time
#define HAL_MAKE_OPMODEID(mode, hash)                  \
  ((((int64_t)(mode) << HAL_OPMODE_ROBOT_MODE_SHIFT) & \
    HAL_OPMODE_ROBOT_MODE_MASK) |                      \
   ((hash) & HAL_OPMODE_HASH_MASK))
// NOLINTEND

inline int64_t HAL_MakeOpModeId(HAL_RobotMode mode, int64_t hash) {
  return HAL_MAKE_OPMODEID(mode, hash);
}

inline HAL_RobotMode HAL_OpMode_GetRobotMode(int64_t id) {
  return (HAL_RobotMode)((id & HAL_OPMODE_ROBOT_MODE_MASK) >>  // NOLINT
                         HAL_OPMODE_ROBOT_MODE_SHIFT);
}

inline int64_t HAL_OpMode_GetHash(int64_t id) {
  return id & HAL_OPMODE_HASH_MASK;
}
#ifdef __cplusplus
}  // extern "C"
#endif

/** @} */
