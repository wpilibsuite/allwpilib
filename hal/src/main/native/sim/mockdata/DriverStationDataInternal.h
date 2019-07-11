/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/spinlock.h>

#include "mockdata/DriverStationData.h"
#include "mockdata/SimDataValue.h"

namespace hal {
struct JoystickOutputStore;

class DriverStationData {
  HAL_SIMDATAVALUE_DEFINE_NAME(Enabled)
  HAL_SIMDATAVALUE_DEFINE_NAME(Autonomous)
  HAL_SIMDATAVALUE_DEFINE_NAME(Test)
  HAL_SIMDATAVALUE_DEFINE_NAME(EStop)
  HAL_SIMDATAVALUE_DEFINE_NAME(FmsAttached)
  HAL_SIMDATAVALUE_DEFINE_NAME(DsAttached)
  HAL_SIMDATAVALUE_DEFINE_NAME(AllianceStationId)
  HAL_SIMDATAVALUE_DEFINE_NAME(MatchTime)

  static LLVM_ATTRIBUTE_ALWAYS_INLINE HAL_Value
  MakeAllianceStationIdValue(HAL_AllianceStationID value) {
    return MakeEnum(value);
  }

 public:
  DriverStationData();
  void ResetData();

  void GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes);
  void GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs);
  void GetJoystickButtons(int32_t joystickNum, HAL_JoystickButtons* buttons);
  void GetJoystickDescriptor(int32_t joystickNum,
                             HAL_JoystickDescriptor* descriptor);
  void GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                          int32_t* leftRumble, int32_t* rightRumble);
  void GetMatchInfo(HAL_MatchInfo* info);
  void FreeMatchInfo(const HAL_MatchInfo* info);

  void SetJoystickAxes(int32_t joystickNum, const HAL_JoystickAxes* axes);
  void SetJoystickPOVs(int32_t joystickNum, const HAL_JoystickPOVs* povs);
  void SetJoystickButtons(int32_t joystickNum,
                          const HAL_JoystickButtons* buttons);
  void SetJoystickDescriptor(int32_t joystickNum,
                             const HAL_JoystickDescriptor* descriptor);
  void SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                          int32_t leftRumble, int32_t rightRumble);
  void SetMatchInfo(const HAL_MatchInfo* info);

  void NotifyNewData();

  SimDataValue<HAL_Bool, MakeBoolean, GetEnabledName> enabled{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetAutonomousName> autonomous{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetTestName> test{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetEStopName> eStop{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetFmsAttachedName> fmsAttached{false};
  SimDataValue<HAL_Bool, MakeBoolean, GetDsAttachedName> dsAttached{true};
  SimDataValue<HAL_AllianceStationID, MakeAllianceStationIdValue,
               GetAllianceStationIdName>
      allianceStationId{static_cast<HAL_AllianceStationID>(0)};
  SimDataValue<double, MakeDouble, GetMatchTimeName> matchTime{0.0};

 private:
  wpi::spinlock m_joystickDataMutex;
  wpi::spinlock m_matchInfoMutex;

  std::unique_ptr<HAL_JoystickAxes[]> m_joystickAxes;
  std::unique_ptr<HAL_JoystickPOVs[]> m_joystickPOVs;
  std::unique_ptr<HAL_JoystickButtons[]> m_joystickButtons;

  std::unique_ptr<JoystickOutputStore[]> m_joystickOutputs;
  std::unique_ptr<HAL_JoystickDescriptor[]> m_joystickDescriptor;
  std::unique_ptr<HAL_MatchInfo> m_matchInfo;
};
extern DriverStationData* SimDriverStationData;
}  // namespace hal
