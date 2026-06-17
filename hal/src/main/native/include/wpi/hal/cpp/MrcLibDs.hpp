// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>

#include "wpi/hal/DriverStationTypes.h"
#include "wpi/util/Synchronization.h"
#include "wpi/util/string.hpp"

namespace wpi::hal {
using BackendPrintFunction = std::function<int32_t(
    bool isError, int32_t errorCode, const struct WPI_String* details,
    const struct WPI_String* location, const struct WPI_String* callStack,
    bool* forcePrintMsg)>;

int32_t DefaultSendErrorImpl(bool isError, int32_t errorCode,
                             const struct WPI_String* details,
                             const struct WPI_String* location,
                             const struct WPI_String* callStack, bool printMsg,
                             wpi::hal::BackendPrintFunction& backendWriteFunc);

class MrcLibDs {
 public:
  virtual ~MrcLibDs() = 0;

  virtual int32_t sendError(bool isError, int32_t errorCode,
                            const struct WPI_String* details,
                            const struct WPI_String* location,
                            const struct WPI_String* callStack,
                            bool printMsg) = 0;

  virtual int32_t sendConsoleLine(const struct WPI_String* line) = 0;

  virtual int32_t sendProgramCrash(const struct WPI_String* details,
                                   const struct WPI_String* location,
                                   const struct WPI_String* callStack) = 0;

  virtual int32_t getControlWord(HAL_ControlWord* controlWord) = 0;

  virtual int32_t getUncachedControlWord(HAL_ControlWord* controlWord) = 0;

  virtual int32_t setOpModeOptions(const struct HAL_OpModeOption* options,
                                   int32_t count) = 0;

  virtual int32_t getAllianceStation(
      HAL_AllianceStationID* allianceStation) = 0;

  virtual int32_t getJoystickAxes(int32_t joystickNum,
                                  HAL_JoystickAxes* axes) = 0;

  virtual int32_t getJoystickPOVs(int32_t joystickNum,
                                  HAL_JoystickPOVs* povs) = 0;

  virtual int32_t getJoystickButtons(int32_t joystickNum,
                                     HAL_JoystickButtons* buttons) = 0;

  virtual int32_t getJoystickTouchpads(int32_t joystickNum,
                                       HAL_JoystickTouchpads* touchpads) = 0;

  virtual int32_t getAllJoystickData(int32_t joystickNum,
                                     HAL_JoystickAxes* axes,
                                     HAL_JoystickPOVs* povs,
                                     HAL_JoystickButtons* buttons,
                                     HAL_JoystickTouchpads* touchpads) = 0;

  virtual int32_t getJoystickDescriptor(int32_t joystickNum,
                                        HAL_JoystickDescriptor* desc) = 0;

  virtual int32_t getGameData(HAL_GameData* gameData) = 0;

  virtual int32_t setJoystickRumble(int32_t joystickNum, int32_t leftRumble,
                                    int32_t rightRumble,
                                    int32_t leftTriggerRumble,
                                    int32_t rightTriggerRumble) = 0;

  virtual int32_t setJoystickLeds(int32_t joystickNum, int32_t leds) = 0;

  virtual int32_t getMatchTime(double* matchTime) = 0;

  virtual int32_t getMatchInfo(HAL_MatchInfo* info) = 0;

  virtual int32_t getOutputsEnabled(bool* outputsEnabled) = 0;

  virtual int32_t refreshDSData(bool* wasRefreshed) = 0;

  virtual void provideNewDataEventHandle(WPI_EventHandle handle) = 0;

  virtual void removeNewDataEventHandle(WPI_EventHandle handle) = 0;

  virtual int32_t observeUserProgramStarting() = 0;

  virtual int32_t observeUserProgram(HAL_ControlWord controlWord) = 0;

  virtual int32_t getSystemTimeValid(bool* systemTimeValid) = 0;
};

MrcLibDs* GetMrcLibDs();
MrcLibDs* GetDefaultDriverStationImpl();
void ForceDsInstance(MrcLibDs* ds);
}  // namespace wpi::hal
