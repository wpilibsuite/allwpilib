/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2013-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "Accelerometer.h"
#include "AnalogAccumulator.h"
#include "AnalogGyro.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "AnalogTrigger.h"
#include "Compressor.h"
#include "Constants.h"
#include "Counter.h"
#include "DIO.h"
#include "Errors.h"
#include "FRC_NetworkCommunication/UsageReporting.h"
#include "Handles.h"
#include "I2C.h"
#include "Interrupts.h"
#include "Notifier.h"
#include "PDP.h"
#include "PWM.h"
#include "Ports.h"
#include "Power.h"
#include "Relay.h"
#include "SPI.h"
#include "Semaphore.h"
#include "SerialPort.h"
#include "Solenoid.h"
#include "Task.h"

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

extern "C" {
HAL_PortHandle HAL_GetPort(uint8_t pin);
HAL_PortHandle HAL_GetPortWithModule(uint8_t module, uint8_t pin);
const char* HAL_GetErrorMessage(int32_t code);

uint16_t HAL_GetFPGAVersion(int32_t* status);
uint32_t HAL_GetFPGARevision(int32_t* status);
uint64_t HAL_GetFPGATime(int32_t* status);

bool HAL_GetFPGAButton(int32_t* status);

int HAL_SetErrorData(const char* errors, int errorsLength, int wait_ms);
int HAL_SendError(int isError, int32_t errorCode, int isLVCode,
                  const char* details, const char* location,
                  const char* callStack, int printMsg);

int HAL_GetControlWord(HAL_ControlWord* controlWord);
HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status);
int HAL_GetJoystickAxes(uint8_t joystickNum, HAL_JoystickAxes* axes);
int HAL_GetJoystickPOVs(uint8_t joystickNum, HAL_JoystickPOVs* povs);
int HAL_GetJoystickButtons(uint8_t joystickNum, HAL_JoystickButtons* buttons);
int HAL_GetJoystickDescriptor(uint8_t joystickNum,
                              HAL_JoystickDescriptor* desc);
int HAL_GetJoystickIsXbox(uint8_t joystickNum);
int HAL_GetJoystickType(uint8_t joystickNum);
char* HAL_GetJoystickName(uint8_t joystickNum);
int HAL_GetJoystickAxisType(uint8_t joystickNum, uint8_t axis);
int HAL_SetJoystickOutputs(uint8_t joystickNum, uint32_t outputs,
                           uint16_t leftRumble, uint16_t rightRumble);
float HAL_GetMatchTime(int32_t* status);

void HAL_SetNewDataSem(MULTIWAIT_ID sem);

bool HAL_GetSystemActive(int32_t* status);
bool HAL_GetBrownedOut(int32_t* status);

int HAL_Initialize(int mode = 0);
void HAL_ObserveUserProgramStarting();
void HAL_ObserveUserProgramDisabled();
void HAL_ObserveUserProgramAutonomous();
void HAL_ObserveUserProgramTeleop();
void HAL_ObserveUserProgramTest();

uint32_t HAL_Report(uint8_t resource, uint8_t instanceNumber,
                    uint8_t context = 0, const char* feature = nullptr);
}
