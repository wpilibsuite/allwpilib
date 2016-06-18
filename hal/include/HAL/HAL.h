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
#include "AnalogInput.h"
#include "AnalogOutput.h"
#include "AnalogTrigger.h"
#include "Compressor.h"
#include "Counter.h"
#include "DIO.h"
#include "Encoder.h"
#include "Errors.h"
#include "FRC_NetworkCommunication/UsageReporting.h"
#include "Handles.h"
#include "I2C.h"
#include "Interrupts.h"
#include "Notifier.h"
#include "PDP.h"
#include "PWM.h"
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

struct HALControlWord {
  uint32_t enabled : 1;
  uint32_t autonomous : 1;
  uint32_t test : 1;
  uint32_t eStop : 1;
  uint32_t fmsAttached : 1;
  uint32_t dsAttached : 1;
  uint32_t control_reserved : 26;
};

enum HALAllianceStationID {
  kHALAllianceStationID_red1,
  kHALAllianceStationID_red2,
  kHALAllianceStationID_red3,
  kHALAllianceStationID_blue1,
  kHALAllianceStationID_blue2,
  kHALAllianceStationID_blue3,
};

/* The maximum number of axes that will be stored in a single HALJoystickAxes
 * struct. This is used for allocating buffers, not bounds checking, since
 * there are usually less axes in practice.
 */
static const size_t kMaxJoystickAxes = 12;
static const size_t kMaxJoystickPOVs = 12;

struct HALJoystickAxes {
  uint16_t count;
  float axes[kMaxJoystickAxes];
};

struct HALJoystickPOVs {
  uint16_t count;
  int16_t povs[kMaxJoystickPOVs];
};

struct HALJoystickButtons {
  uint32_t buttons;
  uint8_t count;
};

struct HALJoystickDescriptor {
  uint8_t isXbox;
  uint8_t type;
  char name[256];
  uint8_t axisCount;
  uint8_t axisTypes[kMaxJoystickAxes];
  uint8_t buttonCount;
  uint8_t povCount;
};

extern "C" {
extern const uint32_t dio_kNumSystems;
extern const uint32_t solenoid_kNumDO7_0Elements;
extern const uint32_t interrupt_kNumSystems;
extern const uint32_t kSystemClockTicksPerMicrosecond;

HalPortHandle getPort(uint8_t pin);
HalPortHandle getPortWithModule(uint8_t module, uint8_t pin);
void freePort(HalPortHandle port);
const char* getHALErrorMessage(int32_t code);

uint16_t getFPGAVersion(int32_t* status);
uint32_t getFPGARevision(int32_t* status);
uint64_t getFPGATime(int32_t* status);

bool getFPGAButton(int32_t* status);

int HALSetErrorData(const char* errors, int errorsLength, int wait_ms);
int HALSendError(int isError, int32_t errorCode, int isLVCode,
                 const char* details, const char* location,
                 const char* callStack, int printMsg);

int HALGetControlWord(HALControlWord* data);
int HALGetAllianceStation(enum HALAllianceStationID* allianceStation);
int HALGetJoystickAxes(uint8_t joystickNum, HALJoystickAxes* axes);
int HALGetJoystickPOVs(uint8_t joystickNum, HALJoystickPOVs* povs);
int HALGetJoystickButtons(uint8_t joystickNum, HALJoystickButtons* buttons);
int HALGetJoystickDescriptor(uint8_t joystickNum, HALJoystickDescriptor* desc);
int HALGetJoystickIsXbox(uint8_t joystickNum);
int HALGetJoystickType(uint8_t joystickNum);
char* HALGetJoystickName(uint8_t joystickNum);
int HALGetJoystickAxisType(uint8_t joystickNum, uint8_t axis);
int HALSetJoystickOutputs(uint8_t joystickNum, uint32_t outputs,
                          uint16_t leftRumble, uint16_t rightRumble);
int HALGetMatchTime(float* matchTime);

void HALSetNewDataSem(MULTIWAIT_ID sem);

bool HALGetSystemActive(int32_t* status);
bool HALGetBrownedOut(int32_t* status);

int HALInitialize(int mode = 0);
void HALNetworkCommunicationObserveUserProgramStarting();
void HALNetworkCommunicationObserveUserProgramDisabled();
void HALNetworkCommunicationObserveUserProgramAutonomous();
void HALNetworkCommunicationObserveUserProgramTeleop();
void HALNetworkCommunicationObserveUserProgramTest();

uint32_t HALReport(uint8_t resource, uint8_t instanceNumber,
                   uint8_t context = 0, const char* feature = nullptr);
}

// TODO: HACKS for now...
extern "C" {

void NumericArrayResize();
void RTSetCleanupProc();
void EDVR_CreateReference();
void Occur();
}
