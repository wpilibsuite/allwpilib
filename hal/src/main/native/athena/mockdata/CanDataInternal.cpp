/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/CanData.h"
#include "hal/simulation/SimDataValue.h"

extern "C" {

void HALSIM_ResetCanData(void) {}

#define DEFINE_CAPI(TYPE, CAPINAME) \
  HAL_SIMCALLBACKREGISTRY_STUB_CAPI_NOINDEX(TYPE, HALSIM, Can##CAPINAME)

DEFINE_CAPI(HAL_CAN_SendMessageCallback, SendMessage)
DEFINE_CAPI(HAL_CAN_ReceiveMessageCallback, ReceiveMessage)
DEFINE_CAPI(HAL_CAN_OpenStreamSessionCallback, OpenStream)
DEFINE_CAPI(HAL_CAN_CloseStreamSessionCallback, CloseStream)
DEFINE_CAPI(HAL_CAN_ReadStreamSessionCallback, ReadStream)
DEFINE_CAPI(HAL_CAN_GetCANStatusCallback, GetCANStatus)

}  // extern "C"
