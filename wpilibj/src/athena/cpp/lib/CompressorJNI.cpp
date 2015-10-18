/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.hpp"
#include "HALUtil.h"
#include "Log.hpp"
#include "edu_wpi_first_wpilibj_hal_CompressorJNI.h"

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    initializeCompressor
 * Signature: (B)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_initializeCompressor(
    JNIEnv *env, jclass, jbyte module) {
  void *pcm = initializeCompressor(module);
  return (jlong)pcm;
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val = getCompressor((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer, jboolean value) {
  int32_t status = 0;
  setClosedLoopControl((void *)pcm_pointer, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CompressorJNI
 * Method:    getClosedLoopControl
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CompressorJNI_getClosedLoopControl(
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val = getClosedLoopControl((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val = getPressureSwitch((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  float val = getCompressorCurrent((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val = getCompressorCurrentTooHighFault((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val =
      getCompressorCurrentTooHighStickyFault((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val = getCompressorShortedStickyFault((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val = getCompressorShortedFault((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val = getCompressorNotConnectedStickyFault((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  bool val = getCompressorNotConnectedFault((void *)pcm_pointer, &status);
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
    JNIEnv *env, jclass, jlong pcm_pointer) {
  int32_t status = 0;
  clearAllPCMStickyFaults((void *)pcm_pointer, &status);
  CheckStatus(env, status);
}

}  // extern "C"
