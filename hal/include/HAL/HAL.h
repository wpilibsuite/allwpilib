/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2013-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "FRC_NetworkCommunication/UsageReporting.h"
#include "HAL/Accelerometer.h"
#include "HAL/AnalogAccumulator.h"
#include "HAL/AnalogGyro.h"
#include "HAL/AnalogInput.h"
#include "HAL/AnalogOutput.h"
#include "HAL/AnalogTrigger.h"
#include "HAL/Compressor.h"
#include "HAL/Constants.h"
#include "HAL/Counter.h"
#include "HAL/DIO.h"
#include "HAL/Errors.h"
#include "HAL/I2C.h"
#include "HAL/Interrupts.h"
#include "HAL/Notifier.h"
#include "HAL/PDP.h"
#include "HAL/PWM.h"
#include "HAL/Ports.h"
#include "HAL/Power.h"
#include "HAL/Relay.h"
#include "HAL/SPI.h"
#include "HAL/SerialPort.h"
#include "HAL/Solenoid.h"
#include "HAL/Task.h"
#include "HAL/Types.h"

namespace HALUsageReporting = nUsageReporting;

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

enum HAL_AllianceStationID {
  HAL_AllianceStationID_kRed1,
  HAL_AllianceStationID_kRed2,
  HAL_AllianceStationID_kRed3,
  HAL_AllianceStationID_kBlue1,
  HAL_AllianceStationID_kBlue2,
  HAL_AllianceStationID_kBlue3,
};

/* The maximum number of axes that will be stored in a single HALJoystickAxes
 * struct. This is used for allocating buffers, not bounds checking, since
 * there are usually less axes in practice.
 */
static constexpr size_t HAL_kMaxJoystickAxes = 12;
static constexpr size_t HAL_kMaxJoystickPOVs = 12;

struct HAL_JoystickAxes {
  uint16_t count;
  float axes[HAL_kMaxJoystickAxes];
};

struct HAL_JoystickPOVs {
  uint16_t count;
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

#ifdef __cplusplus
extern "C" {
#endif
HAL_PortHandle HAL_GetPort(int32_t pin);
HAL_PortHandle HAL_GetPortWithModule(int32_t module, int32_t pin);
const char* HAL_GetErrorMessage(int32_t code);

int32_t HAL_GetFPGAVersion(int32_t* status);
int64_t HAL_GetFPGARevision(int32_t* status);
uint64_t HAL_GetFPGATime(int32_t* status);

HAL_Bool HAL_GetFPGAButton(int32_t* status);

int32_t HAL_SetErrorData(const char* errors, int32_t errorsLength,
                         int32_t wait_ms);
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
int32_t HAL_GetJoystickAxisType(int32_t joystickNum, int32_t axis);
int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble);
double HAL_GetMatchTime(int32_t* status);

void HAL_WaitForDSData(void);

HAL_Bool HAL_GetSystemActive(int32_t* status);
HAL_Bool HAL_GetBrownedOut(int32_t* status);

int32_t HAL_Initialize(int32_t mode);
void HAL_ObserveUserProgramStarting(void);
void HAL_ObserveUserProgramDisabled(void);
void HAL_ObserveUserProgramAutonomous(void);
void HAL_ObserveUserProgramTeleop(void);
void HAL_ObserveUserProgramTest(void);

// ifdef's definition is to allow for default parameters in C++.
#ifdef __cplusplus
int64_t HAL_Report(int32_t resource, int32_t instanceNumber,
                   int32_t context = 0, const char* feature = nullptr);
#else
int64_t HAL_Report(int32_t resource, int32_t instanceNumber, int32_t context,
                   const char* feature);
#endif
#ifdef __cplusplus
}
#endif
