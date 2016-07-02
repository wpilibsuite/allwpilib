/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"
#include "HALUtil.h"
#include "Log.h"
#include "edu_wpi_first_wpilibj_hal_CompressorJNI.h"

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    initializeCompressor
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_initializeCompressor(
    JNIEnv *env, jclass, jbyte module) {
  int32_t status = 0;
  auto handle = initializeCompressor(module, &status);
  if (status == PARAMETER_OUT_OF_RANGE) {
    //TODO: Move 63 to a constant (Thad will do)
    ThrowBoundaryException(env, module, 0, 63);
  }
  
  return (jint)handle;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    checkCompressorModule
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_checkCompressorModule(
    JNIEnv *env, jclass, jbyte module) {
  return checkCompressorModule(module);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getCompressor
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getCompressor(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val = getCompressor((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    setClosedLoopControl
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_setClosedLoopControl(
    JNIEnv *env, jclass, jint compressor_handle, jboolean value) {
  int32_t status = 0;
  setClosedLoopControl((HalCompressorHandle)compressor_handle, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getClosedLoopControl
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getClosedLoopControl(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val = getClosedLoopControl((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getPressureSwitch
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getPressureSwitch(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val = getPressureSwitch((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getCompressorCurrent
 * Signature: (J)F
 */
JNIEXPORT jfloat JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getCompressorCurrent(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  float val = getCompressorCurrent((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getCompressorCurrentTooHighFault
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getCompressorCurrentTooHighFault(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val = getCompressorCurrentTooHighFault((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getCompressorCurrentTooHighStickyFault
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getCompressorCurrentTooHighStickyFault(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val =
      getCompressorCurrentTooHighStickyFault((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getCompressorShortedStickyFault
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getCompressorShortedStickyFault(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val = getCompressorShortedStickyFault((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getCompressorShortedFault
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getCompressorShortedFault(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val = getCompressorShortedFault((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getCompressorNotConnectedStickyFault
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getCompressorNotConnectedStickyFault(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val = getCompressorNotConnectedStickyFault((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getCompressorNotConnectedFault
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getCompressorNotConnectedFault(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  bool val = getCompressorNotConnectedFault((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
  return val;
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    clearAllPCMStickyFaults
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_clearAllPCMStickyFaults(
    JNIEnv *env, jclass, jint compressor_handle) {
  int32_t status = 0;
  clearAllPCMStickyFaults((HalCompressorHandle)compressor_handle, &status);
  CheckStatus(env, status);
}

}  // extern "C"
