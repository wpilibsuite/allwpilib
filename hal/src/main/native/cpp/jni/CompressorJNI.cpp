/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALUtil.h"
#include "edu_wpi_first_hal_CompressorJNI.h"
#include "hal/Compressor.h"
#include "hal/Ports.h"
#include "hal/Solenoid.h"
#include "hal/cpp/Log.h"

using namespace frc;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    initializeCompressor
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CompressorJNI_initializeCompressor
  (JNIEnv* env, jclass, jbyte module)
{
  int32_t status = 0;
  auto handle = HAL_InitializeCompressor(module, &status);
  CheckStatusRange(env, status, 0, HAL_GetNumPCMModules(), module);

  return (jint)handle;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    checkCompressorModule
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_checkCompressorModule
  (JNIEnv* env, jclass, jbyte module)
{
  return HAL_CheckCompressorModule(module);
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressor
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressor
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressor((HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    setCompressorClosedLoopControl
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CompressorJNI_setCompressorClosedLoopControl
  (JNIEnv* env, jclass, jint compressorHandle, jboolean value)
{
  int32_t status = 0;
  HAL_SetCompressorClosedLoopControl((HAL_CompressorHandle)compressorHandle,
                                     value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorClosedLoopControl
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorClosedLoopControl
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressorClosedLoopControl(
      (HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorPressureSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorPressureSwitch
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressorPressureSwitch(
      (HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorCurrent
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  double val =
      HAL_GetCompressorCurrent((HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorCurrentTooHighFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorCurrentTooHighFault
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressorCurrentTooHighFault(
      (HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorCurrentTooHighStickyFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorCurrentTooHighStickyFault
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressorCurrentTooHighStickyFault(
      (HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorShortedStickyFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorShortedStickyFault
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressorShortedStickyFault(
      (HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorShortedFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorShortedFault
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressorShortedFault(
      (HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorNotConnectedStickyFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorNotConnectedStickyFault
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressorNotConnectedStickyFault(
      (HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    getCompressorNotConnectedFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CompressorJNI_getCompressorNotConnectedFault
  (JNIEnv* env, jclass, jint compressorHandle)
{
  int32_t status = 0;
  bool val = HAL_GetCompressorNotConnectedFault(
      (HAL_CompressorHandle)compressorHandle, &status);
  CheckStatus(env, status);
  return val;
}
/*
 * Class:     edu_wpi_first_hal_CompressorJNI
 * Method:    clearAllPCMStickyFaults
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CompressorJNI_clearAllPCMStickyFaults
  (JNIEnv* env, jclass, jbyte module)
{
  int32_t status = 0;
  HAL_ClearAllPCMStickyFaults(static_cast<int32_t>(module), &status);
  CheckStatus(env, status);
}

}  // extern "C"
