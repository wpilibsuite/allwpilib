// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_can_CANJNI.h"
#include "hal/CAN.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    FRCNetCommCANSessionMuxSendMessage
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_can_CANJNI_FRCNetCommCANSessionMuxSendMessage
  (JNIEnv* env, jclass, jint messageID, jbyteArray data, jint periodMs)
{
  JByteArrayRef dataArray{env, data};

  const uint8_t* dataBuffer =
      reinterpret_cast<const uint8_t*>(dataArray.array().data());
  uint8_t dataSize = dataArray.array().size();

  int32_t status = 0;
  HAL_CAN_SendMessage(messageID, dataBuffer, dataSize, periodMs, &status);
  CheckCANStatus(env, status, messageID);
}

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    FRCNetCommCANSessionMuxReceiveMessage
 * Signature: (Ljava/lang/Object;ILjava/lang/Object;)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_hal_can_CANJNI_FRCNetCommCANSessionMuxReceiveMessage
  (JNIEnv* env, jclass, jobject messageID, jint messageIDMask,
   jobject timeStamp)
{
  uint32_t* messageIDPtr =
      reinterpret_cast<uint32_t*>(env->GetDirectBufferAddress(messageID));
  uint32_t* timeStampPtr =
      reinterpret_cast<uint32_t*>(env->GetDirectBufferAddress(timeStamp));

  uint8_t dataSize = 0;
  uint8_t buffer[8];

  int32_t status = 0;
  HAL_CAN_ReceiveMessage(messageIDPtr, messageIDMask, buffer, &dataSize,
                         timeStampPtr, &status);

  if (!CheckCANStatus(env, status, *messageIDPtr)) {
    return nullptr;
  }
  return MakeJByteArray(env, {buffer, static_cast<size_t>(dataSize)});
}

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    getCANStatus
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_can_CANJNI_getCANStatus
  (JNIEnv* env, jclass, jobject canStatus)
{
  float percentBusUtilization = 0;
  uint32_t busOffCount = 0;
  uint32_t txFullCount = 0;
  uint32_t receiveErrorCount = 0;
  uint32_t transmitErrorCount = 0;
  int32_t status = 0;
  HAL_CAN_GetCANStatus(&percentBusUtilization, &busOffCount, &txFullCount,
                       &receiveErrorCount, &transmitErrorCount, &status);

  if (!CheckStatus(env, status)) {
    return;
  }

  SetCanStatusObject(env, canStatus, percentBusUtilization, busOffCount,
                     txFullCount, receiveErrorCount, transmitErrorCount);
}

}  // extern "C"
