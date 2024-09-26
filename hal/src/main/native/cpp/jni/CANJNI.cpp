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
  JSpan<const jbyte> dataArray{env, data};

  const uint8_t* dataBuffer =
      reinterpret_cast<const uint8_t*>(dataArray.data());
  uint8_t dataSize = dataArray.size();

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

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    openCANStreamSession
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_can_CANJNI_openCANStreamSession
  (JNIEnv* env, jclass, jint messageID, jint messageIDMask, jint maxMessages)
{
  uint32_t handle = 0;
  int32_t status = 0;
  HAL_CAN_OpenStreamSession(&handle, static_cast<uint32_t>(messageID),
                            static_cast<uint32_t>(messageIDMask),
                            static_cast<uint32_t>(maxMessages), &status);

  if (!CheckStatus(env, status)) {
    return static_cast<jint>(0);
  }

  return static_cast<jint>(handle);
}

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    closeCANStreamSession
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_can_CANJNI_closeCANStreamSession
  (JNIEnv* env, jclass, jint sessionHandle)
{
  HAL_CAN_CloseStreamSession(static_cast<uint32_t>(sessionHandle));
}

/*
 * Class:     edu_wpi_first_hal_can_CANJNI
 * Method:    readCANStreamSession
 * Signature: (I[Ljava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_can_CANJNI_readCANStreamSession
  (JNIEnv* env, jclass, jint sessionHandle, jobjectArray messages,
   jint messagesToRead)
{
  if (messages == nullptr) {
    ThrowNullPointerException(env, "messages cannot be null");
    return 0;
  }

  jsize messagesArrayLen = env->GetArrayLength(messages);

  if (messagesArrayLen < messagesToRead) {
    messagesToRead = messagesArrayLen;
  }

  uint32_t handle = static_cast<uint32_t>(sessionHandle);
  uint32_t messagesRead = 0;

  wpi::SmallVector<HAL_CANStreamMessage, 16> messageBuffer;
  messageBuffer.resize_for_overwrite(messagesToRead);

  int32_t status = 0;

  HAL_CAN_ReadStreamSession(handle, messageBuffer.begin(),
                            static_cast<uint32_t>(messagesToRead),
                            &messagesRead, &status);

  if (status == HAL_ERR_CANSessionMux_MessageNotFound || messagesRead == 0) {
    return 0;
  }

  if (status != HAL_ERR_CANSessionMux_SessionOverrun &&
      !CheckStatus(env, status)) {
    return 0;
  }

  for (int i = 0; i < static_cast<int>(messagesRead); i++) {
    struct HAL_CANStreamMessage* msg = &messageBuffer[i];
    JLocal<jobject> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(messages, i))};
    if (!elem) {
      // If element doesn't exist, construct it in place. If that fails, we are
      // OOM, just return
      elem = JLocal<jobject>{env, CreateCANStreamMessage(env)};
      if (elem) {
        env->SetObjectArrayElement(messages, i, elem);
      } else {
        return 0;
      }
    }
    JLocal<jbyteArray> toSetArray{
        env, SetCANStreamObject(env, elem, msg->dataSize, msg->messageID,
                                msg->timeStamp)};
    auto javaLen = env->GetArrayLength(toSetArray);
    if (javaLen < msg->dataSize) {
      msg->dataSize = javaLen;
    }
    env->SetByteArrayRegion(toSetArray, 0, msg->dataSize,
                            reinterpret_cast<jbyte*>(msg->data));
  }

  if (status == HAL_ERR_CANSessionMux_SessionOverrun) {
    ThrowCANStreamOverflowException(env, messages,
                                    static_cast<jint>(messagesRead));
    return 0;
  }

  return static_cast<jint>(messagesRead);
}

}  // extern "C"
