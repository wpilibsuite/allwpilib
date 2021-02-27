// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_AddressableLEDJNI.h"
#include "hal/AddressableLED.h"

using namespace hal;
using namespace wpi::java;

static_assert(sizeof(jbyte) * 4 == sizeof(HAL_AddressableLEDData));

extern "C" {
/*
 * Class:     edu_wpi_first_hal_AddressableLEDJNI
 * Method:    initialize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AddressableLEDJNI_initialize
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto ret = HAL_InitializeAddressableLED(
      static_cast<HAL_DigitalHandle>(handle), &status);
  CheckStatus(env, status);
  return ret;
}

/*
 * Class:     edu_wpi_first_hal_AddressableLEDJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AddressableLEDJNI_free
  (JNIEnv* env, jclass, jint handle)
{
  HAL_FreeAddressableLED(static_cast<HAL_AddressableLEDHandle>(handle));
}

/*
 * Class:     edu_wpi_first_hal_AddressableLEDJNI
 * Method:    setLength
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AddressableLEDJNI_setLength
  (JNIEnv* env, jclass, jint handle, jint length)
{
  int32_t status = 0;
  HAL_SetAddressableLEDLength(static_cast<HAL_AddressableLEDHandle>(handle),
                              length, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AddressableLEDJNI
 * Method:    setData
 * Signature: (I[B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AddressableLEDJNI_setData
  (JNIEnv* env, jclass, jint handle, jbyteArray arr)
{
  int32_t status = 0;
  JByteArrayRef jArrRef{env, arr};
  auto arrRef = jArrRef.array();
  HAL_WriteAddressableLEDData(
      static_cast<HAL_AddressableLEDHandle>(handle),
      reinterpret_cast<const HAL_AddressableLEDData*>(arrRef.data()),
      arrRef.size() / 4, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AddressableLEDJNI
 * Method:    setBitTiming
 * Signature: (IIIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AddressableLEDJNI_setBitTiming
  (JNIEnv* env, jclass, jint handle, jint lowTime0, jint highTime0,
   jint lowTime1, jint highTime1)
{
  int32_t status = 0;
  HAL_SetAddressableLEDBitTiming(static_cast<HAL_AddressableLEDHandle>(handle),
                                 lowTime0, highTime0, lowTime1, highTime1,
                                 &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AddressableLEDJNI
 * Method:    setSyncTime
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AddressableLEDJNI_setSyncTime
  (JNIEnv* env, jclass, jint handle, jint syncTime)
{
  int32_t status = 0;
  HAL_SetAddressableLEDSyncTime(static_cast<HAL_AddressableLEDHandle>(handle),
                                syncTime, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AddressableLEDJNI
 * Method:    start
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AddressableLEDJNI_start
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_StartAddressableLEDOutput(static_cast<HAL_AddressableLEDHandle>(handle),
                                &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AddressableLEDJNI
 * Method:    stop
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AddressableLEDJNI_stop
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_StopAddressableLEDOutput(static_cast<HAL_AddressableLEDHandle>(handle),
                               &status);
  CheckStatus(env, status);
}
}  // extern "C"
