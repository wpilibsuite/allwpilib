// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CanDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeCanData() {
  static CanData scd;
  ::hal::SimCanData = &scd;
}
}  // namespace hal::init

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

void HALSIM_ResetCanData(void) {
  SimCanData->ResetData();
}

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
