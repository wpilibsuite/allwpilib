/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "MockData/CanData.h"

extern "C" {

void HALSIM_ResetCanData() {}

int32_t HALSIM_RegisterCanSendMessageCallback(
    HAL_CAN_SendMessageCallback callback, void* param) {
  return 0;
}
void HALSIM_CancelCanSendMessageCallback(int32_t uid) {}

int32_t HALSIM_RegisterCanReceiveMessageCallback(
    HAL_CAN_ReceiveMessageCallback callback, void* param) {
  return 0;
}
void HALSIM_CancelCanReceiveMessageCallback(int32_t uid) {}

int32_t HALSIM_RegisterCanOpenStreamCallback(
    HAL_CAN_OpenStreamSessionCallback callback, void* param) {
  return 0;
}
void HALSIM_CancelCanOpenStreamCallback(int32_t uid) {}

int32_t HALSIM_RegisterCanCloseStreamCallback(
    HAL_CAN_CloseStreamSessionCallback callback, void* param) {
  return 0;
}
void HALSIM_CancelCanCloseStreamCallback(int32_t uid) {}

int32_t HALSIM_RegisterCanReadStreamCallback(
    HAL_CAN_ReadStreamSessionCallback callback, void* param) {
  return 0;
}
void HALSIM_CancelCanReadStreamCallback(int32_t uid) {}

int32_t HALSIM_RegisterCanGetCANStatusCallback(
    HAL_CAN_GetCANStatusCallback callback, void* param) {
  return 0;
}
void HALSIM_CancelCanGetCANStatusCallback(int32_t uid) {}

}  // extern "C"
