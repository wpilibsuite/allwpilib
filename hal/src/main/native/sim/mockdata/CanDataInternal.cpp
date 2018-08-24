/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CanDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeCanData() {
  static CanData scd;
  ::hal::SimCanData = &scd;
}
}  // namespace init
}  // namespace hal

CanData* hal::SimCanData;

void CanData::ResetData() {
  sendMessage.Reset();
  receiveMessage.Reset();
  openStreamSession.Reset();
  closeStreamSession.Reset();
  readStreamSession.Reset();
  getCANStatus.Reset();
}

extern "C" {

void HALSIM_ResetCanData(void) { SimCanData->ResetData(); }

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                             \
  HAL_SIMCALLBACKREGISTRY_DEFINE_CAPI_NOINDEX(TYPE, HALSIM, Can##CAPINAME, \
                                              SimCanData, LOWERNAME)

DEFINE_CAPI(HAL_CAN_SendMessageCallback, SendMessage, sendMessage)
DEFINE_CAPI(HAL_CAN_ReceiveMessageCallback, ReceiveMessage, receiveMessage)
DEFINE_CAPI(HAL_CAN_OpenStreamSessionCallback, OpenStream, openStreamSession)
DEFINE_CAPI(HAL_CAN_CloseStreamSessionCallback, CloseStream, closeStreamSession)
DEFINE_CAPI(HAL_CAN_ReadStreamSessionCallback, ReadStream, readStreamSession)
DEFINE_CAPI(HAL_CAN_GetCANStatusCallback, GetCANStatus, getCANStatus)

}  // extern "C"
