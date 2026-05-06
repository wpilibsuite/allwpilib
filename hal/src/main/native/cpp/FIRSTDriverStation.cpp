// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/DashboardOpMode.hpp"
#include "wpi/hal/cpp/MrcLibDs.hpp"
#include "wpi/util/Synchronization.hpp"

static wpi::hal::MrcLibDs* mrcLibDs;

extern "C" {

int32_t HAL_SendError(HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode,
                      const char* details, const char* location,
                      const char* callStack, HAL_Bool printMsg) {
  WPI_String detailsStr = wpi::util::make_string(details);
  WPI_String locationStr = wpi::util::make_string(location);
  WPI_String callStackStr = wpi::util::make_string(callStack);
  return mrcLibDs->sendError(isError, errorCode, &detailsStr, &locationStr,
                             &callStackStr, printMsg);
}

int32_t HAL_SendConsoleLine(const char* line) {
  WPI_String lineStr = wpi::util::make_string(line);
  return mrcLibDs->sendConsoleLine(&lineStr);
}

int32_t HAL_GetControlWord(HAL_ControlWord* controlWord) {
  return mrcLibDs->getControlWord(controlWord);
}

int32_t HAL_GetUncachedControlWord(HAL_ControlWord* controlWord) {
  return mrcLibDs->getUncachedControlWord(controlWord);
}

int32_t HAL_SetOpModeOptions(const struct HAL_OpModeOption* options,
                             int32_t count) {
  return mrcLibDs->setOpModeOptions(options, count);
}

int32_t HAL_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes) {
  return mrcLibDs->getJoystickAxes(joystickNum, axes);
}

int32_t HAL_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs) {
  return mrcLibDs->getJoystickPOVs(joystickNum, povs);
}

int32_t HAL_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons) {
  return mrcLibDs->getJoystickButtons(joystickNum, buttons);
}

int32_t HAL_GetJoystickTouchpads(int32_t joystickNum,
                                 HAL_JoystickTouchpads* touchpads) {
  return mrcLibDs->getJoystickTouchpads(joystickNum, touchpads);
}

void HAL_GetAllJoystickData(int32_t joystickNum, HAL_JoystickAxes* axes,
                            HAL_JoystickPOVs* povs,
                            HAL_JoystickButtons* buttons,
                            HAL_JoystickTouchpads* touchpads) {
  mrcLibDs->getAllJoystickData(joystickNum, axes, povs, buttons, touchpads);
}

int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc) {
  return mrcLibDs->getJoystickDescriptor(joystickNum, desc);
}

int32_t HAL_GetMatchInfo(HAL_MatchInfo* info) {
  return mrcLibDs->getMatchInfo(info);
}

HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status) {
  HAL_AllianceStationID allianceStation;
  *status = mrcLibDs->getAllianceStation(&allianceStation);
  return allianceStation;
}

HAL_Bool HAL_GetJoystickIsGamepad(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  int32_t status = mrcLibDs->getJoystickDescriptor(joystickNum, &joystickDesc);
  if (status == 0) {
    return joystickDesc.isGamepad != 0;
  }
  return false;
}

int32_t HAL_GetJoystickGamepadType(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  int32_t status = mrcLibDs->getJoystickDescriptor(joystickNum, &joystickDesc);
  if (status == 0) {
    return joystickDesc.gamepadType;
  }
  return false;
}

int32_t HAL_GetGameData(HAL_GameData* gameData) {
  return mrcLibDs->getGameData(gameData);
}

int32_t HAL_GetJoystickSupportedOutputs(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  int32_t status = mrcLibDs->getJoystickDescriptor(joystickNum, &joystickDesc);
  if (status == 0) {
    return joystickDesc.supportedOutputs;
  }
  return false;
}

void HAL_GetJoystickName(struct WPI_String* name, int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  const char* cName = joystickDesc.name;
  if (mrcLibDs->getJoystickDescriptor(joystickNum, &joystickDesc) == 0) {
    cName = joystickDesc.name;
  } else {
    cName = "";
  }
  auto len = std::strlen(cName);
  auto write = WPI_AllocateString(name, len);
  std::memcpy(write, cName, len);
}

int32_t HAL_SetJoystickRumble(int32_t joystickNum, int32_t leftRumble,
                              int32_t rightRumble, int32_t leftTriggerRumble,
                              int32_t rightTriggerRumble) {
  return mrcLibDs->setJoystickRumble(joystickNum, leftRumble, rightRumble,
                                     leftTriggerRumble, rightTriggerRumble);
}

int32_t HAL_SetJoystickLeds(int32_t joystickNum, int32_t leds) {
  return mrcLibDs->setJoystickLeds(joystickNum, leds);
}

double HAL_GetMatchTime(int32_t* status) {
  double matchTime;
  *status = mrcLibDs->getMatchTime(&matchTime);
  if (*status != 0) {
    return -1.0;
  }
  return matchTime;
}

void HAL_ObserveUserProgramStarting(void) {
  mrcLibDs->observeUserProgramStarting();
}

void HAL_ObserveUserProgram(HAL_ControlWord word) {
  mrcLibDs->observeUserProgram(word);
}

HAL_Bool HAL_RefreshDSData(void) {
  bool wasRefreshed;
  if (mrcLibDs->refreshDSData(&wasRefreshed) < 0) {
    return false;
  }
  return wasRefreshed;
}

void HAL_ProvideNewDataEventHandle(WPI_EventHandle handle) {
  mrcLibDs->provideNewDataEventHandle(handle);
}

void HAL_RemoveNewDataEventHandle(WPI_EventHandle handle) {
  mrcLibDs->removeNewDataEventHandle(handle);
}

HAL_Bool HAL_GetOutputsEnabled(void) {
  bool outputsEnabled;
  if (mrcLibDs->getOutputsEnabled(&outputsEnabled) < 0) {
    return false;
  }
  return outputsEnabled;
}

HAL_Bool HAL_GetSystemTimeValid(int32_t* status) {
  bool systemTimeValid;
  *status = mrcLibDs->getSystemTimeValid(&systemTimeValid);
  return systemTimeValid;
}

}  // extern "C"

namespace wpi::hal {
void InitializeDriverStation() {
  InitializeDashboardOpMode();
  mrcLibDs = wpi::hal::GetDefaultDriverStationImpl();
}

void ForceRealDs() {
  mrcLibDs = wpi::hal::GetMrcLibDs();
}

void WaitForInitialPacket() {
  wpi::util::Event waitForInitEvent;
  mrcLibDs->provideNewDataEventHandle(waitForInitEvent.GetHandle());
  bool timed_out = false;
  wpi::util::WaitForObject(waitForInitEvent.GetHandle(), 0.1, &timed_out);
  // Don't care what the result is, just want to give it a chance.
  mrcLibDs->removeNewDataEventHandle(waitForInitEvent.GetHandle());
}
}  // namespace wpi::hal
