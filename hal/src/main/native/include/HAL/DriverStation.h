/*----------------------------------------------------------------------------*/
/* Copyright (c) 2013-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <cstddef>

#include "HAL/Types.h"

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

enum HAL_AllianceStationID : int32_t {
  HAL_AllianceStationID_kRed1,
  HAL_AllianceStationID_kRed2,
  HAL_AllianceStationID_kRed3,
  HAL_AllianceStationID_kBlue1,
  HAL_AllianceStationID_kBlue2,
  HAL_AllianceStationID_kBlue3,
};

enum HAL_MatchType {
  HAL_kMatchType_none,
  HAL_kMatchType_practice,
  HAL_kMatchType_qualification,
  HAL_kMatchType_elimination,
};

/* The maximum number of axes that will be stored in a single HALJoystickAxes
 * struct. This is used for allocating buffers, not bounds checking, since
 * there are usually less axes in practice.
 */
#define HAL_kMaxJoystickAxes 12
#define HAL_kMaxJoystickPOVs 12

struct HAL_JoystickAxes {
  int16_t count;
  float axes[HAL_kMaxJoystickAxes];
};

struct HAL_JoystickPOVs {
  int16_t count;
  int16_t povs[HAL_kMaxJoystickPOVs];
};

struct HAL_JoystickButtons {
  uint32_t buttons;
  uint8_t count;
};

struct HAL_JoystickDescriptor {
  uint8_t isXbox;
  uint8_t type;
  char name[256];
  uint8_t axisCount;
  uint8_t axisTypes[HAL_kMaxJoystickAxes];
  uint8_t buttonCount;
  uint8_t povCount;
};

struct HAL_MatchInfo {
  char* eventName;
  HAL_MatchType matchType;
  uint16_t matchNumber;
  uint8_t replayNumber;
  char* gameSpecificMessage;
};

#ifdef __cplusplus
extern "C" {
#endif

int32_t HAL_SendError(HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode,
                      const char* details, const char* location,
                      const char* callStack, HAL_Bool printMsg);

int32_t HAL_GetControlWord(HAL_ControlWord* controlWord);
HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status);
int32_t HAL_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes);
int32_t HAL_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs);
int32_t HAL_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons);
int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc);
HAL_Bool HAL_GetJoystickIsXbox(int32_t joystickNum);
int32_t HAL_GetJoystickType(int32_t joystickNum);
char* HAL_GetJoystickName(int32_t joystickNum);
void HAL_FreeJoystickName(char* name);
int32_t HAL_GetJoystickAxisType(int32_t joystickNum, int32_t axis);
int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble);
double HAL_GetMatchTime(int32_t* status);

int HAL_GetMatchInfo(HAL_MatchInfo* info);
void HAL_FreeMatchInfo(HAL_MatchInfo* info);

#ifndef HAL_USE_LABVIEW

void HAL_ReleaseDSMutex(void);
bool HAL_IsNewControlData(void);
void HAL_WaitForDSData(void);
HAL_Bool HAL_WaitForDSDataTimeout(double timeout);
void HAL_InitializeDriverStation(void);

void HAL_ObserveUserProgramStarting(void);
void HAL_ObserveUserProgramDisabled(void);
void HAL_ObserveUserProgramAutonomous(void);
void HAL_ObserveUserProgramTeleop(void);
void HAL_ObserveUserProgramTest(void);

#endif  // HAL_USE_LABVIEW

#ifdef __cplusplus
}  // extern "C"
#endif
