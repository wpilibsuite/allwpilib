// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_can_CANJNI.h"
#include "wpi/hal/CAN.h"
#include "wpi/hal/Errors.h"
#include "wpi/util/jni_util.hpp"

using namespace hal;
using namespace wpi::java;

extern "C" {

static bool PackCANMessage(JNIEnv* env, jbyteArray data, jint dataLength,
                           jint flags, HAL_CANMessage* message) {
  if (data == nullptr) {
    ThrowNullPointerException(env, "data array cannot be null");
    return false;
  }

  auto arrLen = env->GetArrayLength(data);
  if (arrLen < dataLength) {
    ThrowIllegalArgumentException(env, "array length less than data length");
    return false;
  }

  if ((flags & HAL_CAN_FD_DATALENGTH) && dataLength > 64) {
    ThrowIllegalArgumentException(env, "FD frame has max length of 64 bytes");
    return false;
  } else if (!(flags & HAL_CAN_FD_DATALENGTH) && dataLength > 8) {
    ThrowIllegalArgumentException(env,
                                  "Non FD frame has max length of 8 bytes");
    return false;
  }

  std::memset(message, 0, sizeof(*message));
  message->dataSize = dataLength;
  message->flags = flags;
  JSpan<const jbyte> arr{env, data, static_cast<size_t>(dataLength)};
  std::memcpy(message->data, arr.data(), dataLength);
  return true;
}

/*
 * Class:     org_wpilib_hardware_hal_can_CANJNI
 * Method:    sendMessage
 * Signature: (II[BIII)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_can_CANJNI_sendMessage
  (JNIEnv* env, jclass, jint busId, jint messageId, jbyteArray data,
   jint dataLength, jint flags, jint periodMs)
{
  HAL_CANMessage message;
  if (!PackCANMessage(env, data, dataLength, flags, &message)) {
    return PARAMETER_OUT_OF_RANGE;
  }

  int32_t status = 0;
  HAL_CAN_SendMessage(busId, messageId, &message, periodMs, &status);
  return status;
}

/*
 * Class:     org_wpilib_hardware_hal_can_CANJNI
 * Method:    receiveMessage
 * Signature: (IILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_can_CANJNI_receiveMessage
  (JNIEnv* env, jclass, jint busId, jint messageId, jobject data)
{
  HAL_CANReceiveMessage message;
  std::memset(&message, 0, sizeof(message));
  int32_t status = 0;
  HAL_CAN_ReceiveMessage(busId, messageId, &message, &status);
  jbyteArray toSetArray =
      SetCANReceiveMessageObject(env, data, message.message.dataSize,
                                 message.message.flags, message.timeStamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < message.message.dataSize) {
    ThrowIllegalArgumentException(env,
                                  "Message buffer not long enough for message");
    return status;
  }
  env->SetByteArrayRegion(toSetArray, 0, message.message.dataSize,
                          reinterpret_cast<jbyte*>(message.message.data));
  return status;
}

/*
 * Class:     org_wpilib_hardware_hal_can_CANJNI
 * Method:    getCANStatus
 * Signature: (ILjava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_can_CANJNI_getCANStatus
  (JNIEnv* env, jclass, jint busId, jobject canStatus)
{
  float percentBusUtilization = 0;
  uint32_t busOffCount = 0;
  uint32_t txFullCount = 0;
  uint32_t receiveErrorCount = 0;
  uint32_t transmitErrorCount = 0;
  int32_t status = 0;
  HAL_CAN_GetCANStatus(busId, &percentBusUtilization, &busOffCount,
                       &txFullCount, &receiveErrorCount, &transmitErrorCount,
                       &status);

  if (!CheckStatus(env, status)) {
    return;
  }

  SetCanStatusObject(env, canStatus, percentBusUtilization, busOffCount,
                     txFullCount, receiveErrorCount, transmitErrorCount);
}

/*
 * Class:     org_wpilib_hardware_hal_can_CANJNI
 * Method:    openCANStreamSession
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_can_CANJNI_openCANStreamSession
  (JNIEnv* env, jclass, jint busId, jint messageId, jint messageIDMask,
   jint maxMessages)
{
  int32_t status = 0;
  HAL_CANStreamHandle handle =
      HAL_CAN_OpenStreamSession(busId, static_cast<uint32_t>(messageId),
                                static_cast<uint32_t>(messageIDMask),
                                static_cast<uint32_t>(maxMessages), &status);

  if (!CheckStatus(env, status)) {
    return static_cast<jint>(0);
  }

  return static_cast<jint>(handle);
}

/*
 * Class:     org_wpilib_hardware_hal_can_CANJNI
 * Method:    closeCANStreamSession
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_can_CANJNI_closeCANStreamSession
  (JNIEnv* env, jclass, jint sessionHandle)
{
  HAL_CAN_CloseStreamSession(static_cast<uint32_t>(sessionHandle));
}

/*
 * Class:     org_wpilib_hardware_hal_can_CANJNI
 * Method:    readCANStreamSession
 * Signature: (I[Ljava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_can_CANJNI_readCANStreamSession
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
        env, SetCANStreamObject(env, elem, msg->message.message.flags,
                                msg->message.message.dataSize, msg->messageId,
                                msg->message.timeStamp)};
    auto javaLen = env->GetArrayLength(toSetArray);
    if (javaLen < msg->message.message.dataSize) {
      ThrowIllegalArgumentException(
          env, "Message buffer not long enough for message");
      return PARAMETER_OUT_OF_RANGE;
    }
    env->SetByteArrayRegion(
        toSetArray, 0, msg->message.message.dataSize,
        reinterpret_cast<jbyte*>(msg->message.message.data));
  }

  if (status == HAL_ERR_CANSessionMux_SessionOverrun) {
    ThrowCANStreamOverflowException(env, messages,
                                    static_cast<jint>(messagesRead));
    return 0;
  }

  return static_cast<jint>(messagesRead);
}

}  // extern "C"
