/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include <wpi/SmallString.h>
#include <wpi/jni_util.h>
#include <wpi/raw_ostream.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_CANAPIJNI.h"
#include "hal/CAN.h"
#include "hal/CANAPI.h"
#include "hal/Errors.h"
#include "hal/cpp/Log.h"

using namespace frc;
using namespace wpi::java;

extern "C" {
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
  HAL_CleanCAN(static_cast<HAL_CANHandle>(handle));
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
  JByteArrayRef arr{env, data};
  auto arrRef = arr.array();
  int32_t status = 0;
  HAL_WriteCANPacket(halHandle, reinterpret_cast<const uint8_t*>(arrRef.data()),
                     arrRef.size(), apiId, &status);
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
  JByteArrayRef arr{env, data};
  auto arrRef = arr.array();
  int32_t status = 0;
  HAL_WriteCANPacketRepeating(halHandle,
                              reinterpret_cast<const uint8_t*>(arrRef.data()),
                              arrRef.size(), apiId, timeoutMs, &status);
  CheckStatus(env, status);
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
  if (dataLength > 8) dataLength = 8;

  jbyteArray toSetArray = SetCANDataObject(env, data, dataLength, timestamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < dataLength) dataLength = javaLen;
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
  if (dataLength > 8) dataLength = 8;

  jbyteArray toSetArray = SetCANDataObject(env, data, dataLength, timestamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < dataLength) dataLength = javaLen;
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
  if (dataLength > 8) dataLength = 8;

  jbyteArray toSetArray = SetCANDataObject(env, data, dataLength, timestamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < dataLength) dataLength = javaLen;
  env->SetByteArrayRegion(toSetArray, 0, dataLength,
                          reinterpret_cast<jbyte*>(dataTemp));
  return true;
}

/*
 * Class:     edu_wpi_first_hal_CANAPIJNI
 * Method:    readCANPeriodicPacket
 * Signature: (IIIILjava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CANAPIJNI_readCANPeriodicPacket
  (JNIEnv* env, jclass, jint handle, jint apiId, jint timeoutMs, jint periodMs,
   jobject data)
{
  auto halHandle = static_cast<HAL_CANHandle>(handle);
  uint8_t dataTemp[8];
  int32_t dataLength = 0;
  uint64_t timestamp = 0;
  int32_t status = 0;
  HAL_ReadCANPeriodicPacket(halHandle, apiId, dataTemp, &dataLength, &timestamp,
                            timeoutMs, periodMs, &status);
  if (status == HAL_CAN_TIMEOUT ||
      status == HAL_ERR_CANSessionMux_MessageNotFound) {
    return false;
  }
  if (!CheckStatus(env, status)) {
    return false;
  }
  if (dataLength > 8) dataLength = 8;

  jbyteArray toSetArray = SetCANDataObject(env, data, dataLength, timestamp);
  auto javaLen = env->GetArrayLength(toSetArray);
  if (javaLen < dataLength) dataLength = javaLen;
  env->SetByteArrayRegion(toSetArray, 0, dataLength,
                          reinterpret_cast<jbyte*>(dataTemp));
  return true;
}
}  // extern "C"
