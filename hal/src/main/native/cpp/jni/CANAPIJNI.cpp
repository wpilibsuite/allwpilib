// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_CANAPIJNI.h"
#include "hal/CAN.h"
#include "hal/CANAPI.h"
#include "hal/Errors.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    initializeCAN
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_initializeCAN
  (JNIEnv* env, jclass, jint busId, jint manufacturer, jint deviceId,
   jint deviceType)
{
  int32_t status = 0;
  auto handle =
      HAL_InitializeCAN(static_cast<int32_t>(busId),
                        static_cast<HAL_CANManufacturer>(manufacturer),
                        static_cast<int32_t>(deviceId),
                        static_cast<HAL_CANDeviceType>(deviceType), &status);

  CheckStatusForceThrow(env, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    cleanCAN
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_cleanCAN
  (JNIEnv* env, jclass, jint handle)
{
  if (handle != HAL_InvalidHandle) {
    HAL_CleanCAN(static_cast<HAL_CANHandle>(handle));
  }
}

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
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANPacket
 * Signature: (II[BII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANPacket
  (JNIEnv* env, jclass, jint handle, jint apiId, jbyteArray data,
   jint dataLength, jint flags)
{
  HAL_CANMessage message;
  if (!PackCANMessage(env, data, dataLength, flags, &message)) {
    return;
  }

  int32_t status = 0;
  HAL_WriteCANPacket(static_cast<HAL_CANHandle>(handle), apiId, &message,
                     &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANPacketRepeating
 * Signature: (II[BIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANPacketRepeating
  (JNIEnv* env, jclass, jint handle, jint apiId, jbyteArray data,
   jint dataLength, jint flags, jint repeatMs)
{
  HAL_CANMessage message;
  if (!PackCANMessage(env, data, dataLength, flags, &message)) {
    return;
  }

  int32_t status = 0;
  HAL_WriteCANPacketRepeating(static_cast<HAL_CANHandle>(handle), apiId,
                              &message, repeatMs, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANRTRFrame
 * Signature: (II[BII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANRTRFrame
  (JNIEnv* env, jclass, jint handle, jint apiId, jbyteArray data,
   jint dataLength, jint flags)
{
  HAL_CANMessage message;

  if (data == nullptr) {
    // We will allow RTR frames to have a null data array
    if ((flags & HAL_CAN_FD_DATALENGTH) && dataLength > 64) {
      ThrowIllegalArgumentException(env, "FD frame has max length of 64 bytes");
      return;
    } else if (!(flags & HAL_CAN_FD_DATALENGTH) && dataLength > 8) {
      ThrowIllegalArgumentException(env,
                                    "Non FD frame has max length of 8 bytes");
      return;
    }

    std::memset(&message, 0, sizeof(message));
    message.flags = flags;
    message.dataSize = dataLength;
  } else if (!PackCANMessage(env, data, dataLength, flags, &message)) {
    return;
  }

  int32_t status = 0;
  HAL_WriteCANRTRFrame(static_cast<HAL_CANHandle>(handle), apiId, &message,
                       &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANPacketNoThrow
 * Signature: (II[BII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANPacketNoThrow
  (JNIEnv* env, jclass, jint handle, jint apiId, jbyteArray data,
   jint dataLength, jint flags)
{
  HAL_CANMessage message;
  if (!PackCANMessage(env, data, dataLength, flags, &message)) {
    return PARAMETER_OUT_OF_RANGE;
  }

  int32_t status = 0;
  HAL_WriteCANPacket(static_cast<HAL_CANHandle>(handle), apiId, &message,
                     &status);
  return status;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANPacketRepeatingNoThrow
 * Signature: (II[BIII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANPacketRepeatingNoThrow
  (JNIEnv* env, jclass, jint handle, jint apiId, jbyteArray data,
   jint dataLength, jint flags, jint repeatMs)
{
  HAL_CANMessage message;
  if (!PackCANMessage(env, data, dataLength, flags, &message)) {
    return PARAMETER_OUT_OF_RANGE;
  }

  int32_t status = 0;
  HAL_WriteCANPacketRepeating(static_cast<HAL_CANHandle>(handle), apiId,
                              &message, repeatMs, &status);
  return status;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANRTRFrameNoThrow
 * Signature: (II[BII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANRTRFrameNoThrow
  (JNIEnv* env, jclass, jint handle, jint apiId, jbyteArray data,
   jint dataLength, jint flags)
{
  HAL_CANMessage message;

  if (data == nullptr) {
    // We will allow RTR frames to have a null data array
    if ((flags & HAL_CAN_FD_DATALENGTH) && dataLength > 64) {
      ThrowIllegalArgumentException(env, "FD frame has max length of 64 bytes");
      return PARAMETER_OUT_OF_RANGE;
    } else if (!(flags & HAL_CAN_FD_DATALENGTH) && dataLength > 8) {
      ThrowIllegalArgumentException(env,
                                    "Non FD frame has max length of 8 bytes");
      return PARAMETER_OUT_OF_RANGE;
    }

    std::memset(&message, 0, sizeof(message));
    message.flags = flags;
    message.dataSize = dataLength;
  } else if (!PackCANMessage(env, data, dataLength, flags, &message)) {
    return PARAMETER_OUT_OF_RANGE;
  }

  int32_t status = 0;
  HAL_WriteCANRTRFrame(static_cast<HAL_CANHandle>(handle), apiId, &message,
                       &status);
  return status;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    stopCANPacketRepeating
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_stopCANPacketRepeating
  (JNIEnv* env, jclass, jint handle, jint apiId)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  int32_t status = 0;
  HAL_StopCANPacketRepeating(halHandle, apiId, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    readCANPacketNew
 * Signature: (IILjava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_readCANPacketNew
  (JNIEnv* env, jclass, jint handle, jint apiId, jobject data)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  HAL_CANReceiveMessage message;
  std::memset(&message, 0, sizeof(message));
  int32_t status = 0;
  HAL_ReadCANPacketNew(halHandle, apiId, &message, &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (!CheckStatus(env, status)) {
    return false;
  }

  jbyteArray toSetArray =
      SetCANReceiveMessageObject(env, data, message.message.dataSize,
                                 message.message.flags, message.timeStamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < message.message.dataSize) {
    ThrowIllegalArgumentException(env,
                                  "Message buffer not long enough for message");
    return false;
  }
  env->SetByteArrayRegion(toSetArray, 0, message.message.dataSize,
                          reinterpret_cast<jbyte*>(message.message.data));
  return true;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    readCANPacketLatest
 * Signature: (IILjava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_readCANPacketLatest
  (JNIEnv* env, jclass, jint handle, jint apiId, jobject data)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  HAL_CANReceiveMessage message;
  std::memset(&message, 0, sizeof(message));
  int32_t status = 0;
  HAL_ReadCANPacketLatest(halHandle, apiId, &message, &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (!CheckStatus(env, status)) {
    return false;
  }

  jbyteArray toSetArray =
      SetCANReceiveMessageObject(env, data, message.message.dataSize,
                                 message.message.flags, message.timeStamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < message.message.dataSize) {
    ThrowIllegalArgumentException(env,
                                  "Message buffer not long enough for message");
    return false;
  }
  env->SetByteArrayRegion(toSetArray, 0, message.message.dataSize,
                          reinterpret_cast<jbyte*>(message.message.data));
  return true;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    readCANPacketTimeout
 * Signature: (IILjava/lang/Object;I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_readCANPacketTimeout
  (JNIEnv* env, jclass, jint handle, jint apiId, jobject data, jint timeoutMs)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  HAL_CANReceiveMessage message;
  std::memset(&message, 0, sizeof(message));
  int32_t status = 0;
  HAL_ReadCANPacketTimeout(halHandle, apiId, &message, timeoutMs, &status);
  if (status == HAL_CAN_TIMEOUT ||
      status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (!CheckStatus(env, status)) {
    return false;
  }

  jbyteArray toSetArray =
      SetCANReceiveMessageObject(env, data, message.message.dataSize,
                                 message.message.flags, message.timeStamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < message.message.dataSize) {
    ThrowIllegalArgumentException(env,
                                  "Message buffer not long enough for message");
    return false;
  }
  env->SetByteArrayRegion(toSetArray, 0, message.message.dataSize,
                          reinterpret_cast<jbyte*>(message.message.data));
  return true;
}
}  // extern "C"
