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
 * Method:    getCANPacketBaseTime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_getCANPacketBaseTime
  (JNIEnv*, jclass)
{
  return HAL_GetCANPacketBaseTime();
}
/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    initializeCAN
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_initializeCAN
  (JNIEnv* env, jclass, jint manufacturer, jint deviceId, jint deviceType)
{
  int32_t status = 0;
  auto handle =
      HAL_InitializeCAN(static_cast<HAL_CANManufacturer>(manufacturer),
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
  if (handle != HAL_kInvalidHandle) {
    HAL_CleanCAN(static_cast<HAL_CANHandle>(handle));
  }
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANPacket
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANPacket
  (JNIEnv* env, jclass, jint handle, jbyteArray data, jint apiId)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  JSpan<const jbyte> arr{env, data};
  int32_t status = 0;
  HAL_WriteCANPacket(halHandle, reinterpret_cast<const uint8_t*>(arr.data()),
                     arr.size(), apiId, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANPacketRepeating
 * Signature: (I[BII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANPacketRepeating
  (JNIEnv* env, jclass, jint handle, jbyteArray data, jint apiId,
   jint timeoutMs)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  JSpan<const jbyte> arr{env, data};
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(halHandle,
                              reinterpret_cast<const uint8_t*>(arr.data()),
                              arr.size(), apiId, timeoutMs, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANRTRFrame
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANRTRFrame
  (JNIEnv* env, jclass, jint handle, jint length, jint apiId)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  int32_t status = 0;
  HAL_WriteCANRTRFrame(halHandle, static_cast<int32_t>(length), apiId, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANPacketNoThrow
 * Signature: (I[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANPacketNoThrow
  (JNIEnv* env, jclass, jint handle, jbyteArray data, jint apiId)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  JSpan<const jbyte> arr{env, data};
  int32_t status = 0;
  HAL_WriteCANPacket(halHandle, reinterpret_cast<const uint8_t*>(arr.data()),
                     arr.size(), apiId, &status);
  return status;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANPacketRepeatingNoThrow
 * Signature: (I[BII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANPacketRepeatingNoThrow
  (JNIEnv* env, jclass, jint handle, jbyteArray data, jint apiId,
   jint timeoutMs)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  JSpan<const jbyte> arr{env, data};
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(halHandle,
                              reinterpret_cast<const uint8_t*>(arr.data()),
                              arr.size(), apiId, timeoutMs, &status);
  return status;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    writeCANRTRFrameNoThrow
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_writeCANRTRFrameNoThrow
  (JNIEnv* env, jclass, jint handle, jint length, jint apiId)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  int32_t status = 0;
  HAL_WriteCANRTRFrame(halHandle, static_cast<int32_t>(length), apiId, &status);
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
  uint8_t dataTemp[8];
  int32_t dataLength = 0;
  uint64_t timestamp = 0;
  int32_t status = 0;
  HAL_ReadCANPacketNew(halHandle, apiId, dataTemp, &dataLength, &timestamp,
                       &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (!CheckStatus(env, status)) {
    return false;
  }
  if (dataLength > 8) {
    dataLength = 8;
  }

  jbyteArray toSetArray = SetCANDataObject(env, data, dataLength, timestamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < dataLength) {
    dataLength = javaLen;
  }
  env->SetByteArrayRegion(toSetArray, 0, dataLength,
                          reinterpret_cast<jbyte*>(dataTemp));
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
  uint8_t dataTemp[8];
  int32_t dataLength = 0;
  uint64_t timestamp = 0;
  int32_t status = 0;
  HAL_ReadCANPacketLatest(halHandle, apiId, dataTemp, &dataLength, &timestamp,
                          &status);
  if (status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (!CheckStatus(env, status)) {
    return false;
  }
  if (dataLength > 8) {
    dataLength = 8;
  }

  jbyteArray toSetArray = SetCANDataObject(env, data, dataLength, timestamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < dataLength) {
    dataLength = javaLen;
  }
  env->SetByteArrayRegion(toSetArray, 0, dataLength,
                          reinterpret_cast<jbyte*>(dataTemp));
  return true;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    readCANPacketTimeout
 * Signature: (IIILjava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_readCANPacketTimeout
  (JNIEnv* env, jclass, jint handle, jint apiId, jint timeoutMs, jobject data)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  uint8_t dataTemp[8];
  int32_t dataLength = 0;
  uint64_t timestamp = 0;
  int32_t status = 0;
  HAL_ReadCANPacketTimeout(halHandle, apiId, dataTemp, &dataLength, &timestamp,
                           timeoutMs, &status);
  if (status == HAL_CAN_TIMEOUT ||
      status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (!CheckStatus(env, status)) {
    return false;
  }
  if (dataLength > 8) {
    dataLength = 8;
  }

  jbyteArray toSetArray = SetCANDataObject(env, data, dataLength, timestamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < dataLength) {
    dataLength = javaLen;
  }
  env->SetByteArrayRegion(toSetArray, 0, dataLength,
                          reinterpret_cast<jbyte*>(dataTemp));
  return true;
}
}  // extern "C"
