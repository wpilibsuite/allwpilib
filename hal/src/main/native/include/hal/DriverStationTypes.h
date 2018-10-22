/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_driverstation Driver Station Functions
 * @ingroup hal_capi
 * @{
 */

#define HAL_IO_CONFIG_DATA_SIZE 32
#define HAL_SYS_STATUS_DATA_SIZE 44
#define HAL_USER_STATUS_DATA_SIZE \
  (984 - HAL_IO_CONFIG_DATA_SIZE - HAL_SYS_STATUS_DATA_SIZE)

#define HALFRC_NetworkCommunication_DynamicType_DSEnhancedIO_Input 17
#define HALFRC_NetworkCommunication_DynamicType_DSEnhancedIO_Output 18
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Header 19
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Extra1 20
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Vertices1 21
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Extra2 22
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Vertices2 23
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Joystick 24
#define HALFRC_NetworkCommunication_DynamicType_Kinect_Custom 25

struct HAL_ControlWord {
  uint32_t enabled : 1;
  uint32_t autonomous : 1;
  uint32_t test : 1;
  uint32_t eStop : 1;
  uint32_t fmsAttached : 1;
  uint32_t dsAttached : 1;
  uint32_t control_reserved : 26;
};
typedef struct HAL_ControlWord HAL_ControlWord;

// clang-format off
HAL_ENUM(HAL_AllianceStationID) {
  HAL_AllianceStationID_kRed1,
  HAL_AllianceStationID_kRed2,
  HAL_AllianceStationID_kRed3,
  HAL_AllianceStationID_kBlue1,
  HAL_AllianceStationID_kBlue2,
  HAL_AllianceStationID_kBlue3,
};

HAL_ENUM(HAL_MatchType) {
  HAL_kMatchType_none,
  HAL_kMatchType_practice,
  HAL_kMatchType_qualification,
  HAL_kMatchType_elimination,
};
// clang-format on

/* The maximum number of axes that will be stored in a single HALJoystickAxes
 * struct. This is used for allocating buffers, not bounds checking, since
 * there are usually less axes in practice.
 */
#define HAL_kMaxJoystickAxes 12
#define HAL_kMaxJoystickPOVs 12
#define HAL_kMaxJoysticks 6

struct HAL_JoystickAxes {
  int16_t count;
  float axes[HAL_kMaxJoystickAxes];
};
typedef struct HAL_JoystickAxes HAL_JoystickAxes;

struct HAL_JoystickPOVs {
  int16_t count;
  int16_t povs[HAL_kMaxJoystickPOVs];
};
typedef struct HAL_JoystickPOVs HAL_JoystickPOVs;

struct HAL_JoystickButtons {
  uint32_t buttons;
  uint8_t count;
};
typedef struct HAL_JoystickButtons HAL_JoystickButtons;

struct HAL_JoystickDescriptor {
  uint8_t isXbox;
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
