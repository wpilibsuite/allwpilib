/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "RoboRioDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeRoboRioData() {
  static RoboRioData srrd[1];
  ::hal::SimRoboRioData = srrd;
}
}  // namespace init
}  // namespace hal

RoboRioData* hal::SimRoboRioData;
void RoboRioData::ResetData() {
  fpgaButton.Reset(false);
  vInVoltage.Reset(0.0);
  vInCurrent.Reset(0.0);
  userVoltage6V.Reset(6.0);
  userCurrent6V.Reset(0.0);
  userActive6V.Reset(false);
  userVoltage5V.Reset(5.0);
  userCurrent5V.Reset(0.0);
  userActive5V.Reset(false);
  userVoltage3V3.Reset(3.3);
  userCurrent3V3.Reset(0.0);
  userActive3V3.Reset(false);
  userFaults6V.Reset(0);
  userFaults5V.Reset(0);
  userFaults3V3.Reset(0);
}

extern "C" {
void HALSIM_ResetRoboRioData(int32_t index) {
  SimRoboRioData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                  \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, RoboRio##CAPINAME, \
                               SimRoboRioData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, FPGAButton, fpgaButton)
DEFINE_CAPI(double, VInVoltage, vInVoltage)
DEFINE_CAPI(double, VInCurrent, vInCurrent)
DEFINE_CAPI(double, UserVoltage6V, userVoltage6V)
DEFINE_CAPI(double, UserCurrent6V, userCurrent6V)
DEFINE_CAPI(HAL_Bool, UserActive6V, userActive6V)
DEFINE_CAPI(double, UserVoltage5V, userVoltage5V)
DEFINE_CAPI(double, UserCurrent5V, userCurrent5V)
DEFINE_CAPI(HAL_Bool, UserActive5V, userActive5V)
DEFINE_CAPI(double, UserVoltage3V3, userVoltage3V3)
DEFINE_CAPI(double, UserCurrent3V3, userCurrent3V3)
DEFINE_CAPI(HAL_Bool, UserActive3V3, userActive3V3)
DEFINE_CAPI(int32_t, UserFaults6V, userFaults6V)
DEFINE_CAPI(int32_t, UserFaults5V, userFaults5V)
DEFINE_CAPI(int32_t, UserFaults3V3, userFaults3V3)

#define REGISTER(NAME) \
  SimRoboRioData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterRoboRioAllCallbacks(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {
  REGISTER(fpgaButton);
  REGISTER(vInVoltage);
  REGISTER(vInCurrent);
  REGISTER(userVoltage6V);
  REGISTER(userCurrent6V);
  REGISTER(userActive6V);
  REGISTER(userVoltage5V);
  REGISTER(userCurrent5V);
  REGISTER(userActive5V);
  REGISTER(userVoltage3V3);
  REGISTER(userCurrent3V3);
  REGISTER(userActive3V3);
  REGISTER(userFaults6V);
  REGISTER(userFaults5V);
  REGISTER(userFaults3V3);
}
}  // extern "C"
