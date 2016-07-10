/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"

#include <cstring>

#include "FRC_NetworkCommunication/FRCComm.h"

struct HAL_JoystickAxesInt {
  uint16_t count;
  int16_t axes[HAL_kMaxJoystickAxes];
};

extern "C" {

int HAL_GetControlWord(HAL_ControlWord* controlWord) {
  std::memset(controlWord, 0, sizeof(HAL_ControlWord));
  return FRC_NetworkCommunication_getControlWord(
      reinterpret_cast<ControlWord_t*>(controlWord));
}

void HAL_SetNewDataSem(MULTIWAIT_ID sem) {
  setNewDataSem(sem->native_handle());
}

HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status) {
  HAL_AllianceStationID allianceStation;
  *status = FRC_NetworkCommunication_getAllianceStation(
      reinterpret_cast<AllianceStationID_t*>(&allianceStation));
  return allianceStation;
}

int HAL_GetJoystickAxes(uint8_t joystickNum, HAL_JoystickAxes* axes) {
  HAL_JoystickAxesInt axesInt;

  int retVal = FRC_NetworkCommunication_getJoystickAxes(
      joystickNum, (JoystickAxes_t*)&axesInt, HAL_kMaxJoystickAxes);

  // copy int values to float values
  axes->count = axesInt.count;
  // current scaling is -128 to 127, can easily be patched in the future by
  // changing this function.
  for (unsigned int i = 0; i < axesInt.count; i++) {
    int8_t value = axesInt.axes[i];
    if (value < 0) {
      axes->axes[i] = value / 128.0f;
    } else {
      axes->axes[i] = value / 127.0f;
    }
  }

  return retVal;
}

int HAL_GetJoystickPOVs(uint8_t joystickNum, HAL_JoystickPOVs* povs) {
  return FRC_NetworkCommunication_getJoystickPOVs(
      joystickNum, (JoystickPOV_t*)povs, HAL_kMaxJoystickPOVs);
}

int HAL_GetJoystickButtons(uint8_t joystickNum, HAL_JoystickButtons* buttons) {
  return FRC_NetworkCommunication_getJoystickButtons(
      joystickNum, &buttons->buttons, &buttons->count);
}
/**
 * Retrieve the Joystick Descriptor for particular slot
 * @param desc [out] descriptor (data transfer object) to fill in.  desc is
 * filled in regardless of success. In other words, if descriptor is not
 * available, desc is filled in with default values matching the init-values in
 * Java and C++ Driverstation for when caller requests a too-large joystick
 * index.
 *
 * @return error code reported from Network Comm back-end.  Zero is good,
 * nonzero is bad.
 */
int HAL_GetJoystickDescriptor(uint8_t joystickNum,
                              HAL_JoystickDescriptor* desc) {
  desc->isXbox = 0;
  desc->type = -1;
  desc->name[0] = '\0';
  desc->axisCount =
      HAL_kMaxJoystickAxes; /* set to the desc->axisTypes's capacity */
  desc->buttonCount = 0;
  desc->povCount = 0;
  int retval = FRC_NetworkCommunication_getJoystickDesc(
      joystickNum, &desc->isXbox, &desc->type, (char*)(&desc->name),
      &desc->axisCount, (uint8_t*)&desc->axisTypes, &desc->buttonCount,
      &desc->povCount);
  /* check the return, if there is an error and the RIOimage predates FRC2017,
   * then axisCount needs to be cleared */
  if (retval != 0) {
    /* set count to zero so downstream code doesn't decode invalid axisTypes. */
    desc->axisCount = 0;
  }
  return retval;
}

int HAL_GetJoystickIsXbox(uint8_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return 0;
  } else {
    return joystickDesc.isXbox;
  }
}

int HAL_GetJoystickType(uint8_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.type;
  }
}

char* HAL_GetJoystickName(uint8_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    char* name = (char*)std::malloc(1);
    name[0] = '\0';
    return name;
  } else {
    size_t len = std::strlen(joystickDesc.name);
    char* name = (char*)std::malloc(len + 1);
    std::strcpy(name, joystickDesc.name);
    return name;
  }
}

int HAL_GetJoystickAxisType(uint8_t joystickNum, uint8_t axis) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.axisTypes[axis];
  }
}

int HAL_SetJoystickOutputs(uint8_t joystickNum, uint32_t outputs,
                           uint16_t leftRumble, uint16_t rightRumble) {
  return FRC_NetworkCommunication_setJoystickOutputs(joystickNum, outputs,
                                                     leftRumble, rightRumble);
}

float HAL_GetMatchTime(int32_t* status) {
  float matchTime;
  *status = FRC_NetworkCommunication_getMatchTime(&matchTime);
  return matchTime;
}

void HAL_ObserveUserProgramStarting(void) {
  FRC_NetworkCommunication_observeUserProgramStarting();
}

void HAL_ObserveUserProgramDisabled(void) {
  FRC_NetworkCommunication_observeUserProgramDisabled();
}

void HAL_ObserveUserProgramAutonomous(void) {
  FRC_NetworkCommunication_observeUserProgramAutonomous();
}

void HAL_ObserveUserProgramTeleop(void) {
  FRC_NetworkCommunication_observeUserProgramTeleop();
}

void HAL_ObserveUserProgramTest(void) {
  FRC_NetworkCommunication_observeUserProgramTest();
}

}  // extern "C"
