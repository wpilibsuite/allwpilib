/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cstdlib>
#include <cstring>
#include <string>

#include "DriverStationDataInternal.h"
#include "hal/DriverStation.h"

namespace hal {
struct JoystickOutputStore {
  int64_t outputs = 0;
  int32_t leftRumble = 0;
  int32_t rightRumble = 0;
};
}  // namespace hal

using namespace hal;

namespace hal {
namespace init {
void InitializeDriverStationData() {
  static DriverStationData dsd;
  ::hal::SimDriverStationData = &dsd;
}
}  // namespace init
}  // namespace hal

DriverStationData* hal::SimDriverStationData;

DriverStationData::DriverStationData() { ResetData(); }

void DriverStationData::ResetData() {
  enabled.Reset(false);
  autonomous.Reset(false);
  test.Reset(false);
  eStop.Reset(false);
  fmsAttached.Reset(false);
  dsAttached.Reset(true);
  allianceStationId.Reset(static_cast<HAL_AllianceStationID>(0));
  matchTime.Reset(0.0);

  {
    std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
    m_joystickAxes = std::make_unique<HAL_JoystickAxes[]>(6);
    m_joystickPOVs = std::make_unique<HAL_JoystickPOVs[]>(6);
    m_joystickButtons = std::make_unique<HAL_JoystickButtons[]>(6);
    m_joystickOutputs = std::make_unique<JoystickOutputStore[]>(6);
    m_joystickDescriptor = std::make_unique<HAL_JoystickDescriptor[]>(6);

    for (int i = 0; i < 6; i++) {
      m_joystickAxes[i].count = 0;
      m_joystickPOVs[i].count = 0;
      m_joystickButtons[i].count = 0;
      m_joystickDescriptor[i].isXbox = 0;
      m_joystickDescriptor[i].type = -1;
      m_joystickDescriptor[i].name[0] = '\0';
    }
  }
  {
    std::lock_guard<wpi::spinlock> lock(m_matchInfoMutex);

    m_matchInfo = std::make_unique<HAL_MatchInfo>();
  }
}

void DriverStationData::GetJoystickAxes(int32_t joystickNum,
                                        HAL_JoystickAxes* axes) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  *axes = m_joystickAxes[joystickNum];
}
void DriverStationData::GetJoystickPOVs(int32_t joystickNum,
                                        HAL_JoystickPOVs* povs) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  *povs = m_joystickPOVs[joystickNum];
}
void DriverStationData::GetJoystickButtons(int32_t joystickNum,
                                           HAL_JoystickButtons* buttons) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  *buttons = m_joystickButtons[joystickNum];
}
void DriverStationData::GetJoystickDescriptor(
    int32_t joystickNum, HAL_JoystickDescriptor* descriptor) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  *descriptor = m_joystickDescriptor[joystickNum];
  // Always ensure name is null terminated
  descriptor->name[255] = '\0';
}
void DriverStationData::GetJoystickOutputs(int32_t joystickNum,
                                           int64_t* outputs,
                                           int32_t* leftRumble,
                                           int32_t* rightRumble) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  *leftRumble = m_joystickOutputs[joystickNum].leftRumble;
  *outputs = m_joystickOutputs[joystickNum].outputs;
  *rightRumble = m_joystickOutputs[joystickNum].rightRumble;
}
void DriverStationData::GetMatchInfo(HAL_MatchInfo* info) {
  std::lock_guard<wpi::spinlock> lock(m_matchInfoMutex);
  *info = *m_matchInfo;
}

void DriverStationData::SetJoystickAxes(int32_t joystickNum,
                                        const HAL_JoystickAxes* axes) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  m_joystickAxes[joystickNum] = *axes;
}
void DriverStationData::SetJoystickPOVs(int32_t joystickNum,
                                        const HAL_JoystickPOVs* povs) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  m_joystickPOVs[joystickNum] = *povs;
}
void DriverStationData::SetJoystickButtons(int32_t joystickNum,
                                           const HAL_JoystickButtons* buttons) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  m_joystickButtons[joystickNum] = *buttons;
}

void DriverStationData::SetJoystickDescriptor(
    int32_t joystickNum, const HAL_JoystickDescriptor* descriptor) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  m_joystickDescriptor[joystickNum] = *descriptor;
}

void DriverStationData::SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                                           int32_t leftRumble,
                                           int32_t rightRumble) {
  std::lock_guard<wpi::spinlock> lock(m_joystickDataMutex);
  m_joystickOutputs[joystickNum].leftRumble = leftRumble;
  m_joystickOutputs[joystickNum].outputs = outputs;
  m_joystickOutputs[joystickNum].rightRumble = rightRumble;
}

void DriverStationData::SetMatchInfo(const HAL_MatchInfo* info) {
  std::lock_guard<wpi::spinlock> lock(m_matchInfoMutex);
  *m_matchInfo = *info;
  *(std::end(m_matchInfo->eventName) - 1) = '\0';
}

void DriverStationData::NotifyNewData() { HAL_ReleaseDSMutex(); }

extern "C" {
void HALSIM_ResetDriverStationData(void) { SimDriverStationData->ResetData(); }

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                                \
  HAL_SIMDATAVALUE_DEFINE_CAPI_NOINDEX(TYPE, HALSIM, DriverStation##CAPINAME, \
                                       SimDriverStationData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Enabled, enabled)
DEFINE_CAPI(HAL_Bool, Autonomous, autonomous)
DEFINE_CAPI(HAL_Bool, Test, test)
DEFINE_CAPI(HAL_Bool, EStop, eStop)
DEFINE_CAPI(HAL_Bool, FmsAttached, fmsAttached)
DEFINE_CAPI(HAL_Bool, DsAttached, dsAttached)
DEFINE_CAPI(HAL_AllianceStationID, AllianceStationId, allianceStationId)
DEFINE_CAPI(double, MatchTime, matchTime)

void HALSIM_SetJoystickAxes(int32_t joystickNum, const HAL_JoystickAxes* axes) {
  SimDriverStationData->SetJoystickAxes(joystickNum, axes);
}

void HALSIM_SetJoystickPOVs(int32_t joystickNum, const HAL_JoystickPOVs* povs) {
  SimDriverStationData->SetJoystickPOVs(joystickNum, povs);
}

void HALSIM_SetJoystickButtons(int32_t joystickNum,
                               const HAL_JoystickButtons* buttons) {
  SimDriverStationData->SetJoystickButtons(joystickNum, buttons);
}
void HALSIM_SetJoystickDescriptor(int32_t joystickNum,
                                  const HAL_JoystickDescriptor* descriptor) {
  SimDriverStationData->SetJoystickDescriptor(joystickNum, descriptor);
}

void HALSIM_GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                               int32_t* leftRumble, int32_t* rightRumble) {
  SimDriverStationData->GetJoystickOutputs(joystickNum, outputs, leftRumble,
                                           rightRumble);
}

void HALSIM_SetMatchInfo(const HAL_MatchInfo* info) {
  SimDriverStationData->SetMatchInfo(info);
}

void HALSIM_NotifyDriverStationNewData(void) {
  SimDriverStationData->NotifyNewData();
}

#define REGISTER(NAME) \
  SimDriverStationData->NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterDriverStationAllCallbacks(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  REGISTER(enabled);
  REGISTER(autonomous);
  REGISTER(test);
  REGISTER(eStop);
  REGISTER(fmsAttached);
  REGISTER(dsAttached);
  REGISTER(allianceStationId);
  REGISTER(matchTime);
}
}  // extern "C"
