// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <atomic>
#include <cassert>
#include <thread>

#include <wpi/SafeThread.h>
#include <wpi/mutex.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_InterruptJNI.h"
#include "hal/Interrupts.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    initializeInterrupts
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_InterruptJNI_initializeInterrupts
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  HAL_InterruptHandle interrupt = HAL_InitializeInterrupts(&status);

  CheckStatusForceThrow(env, status);
  return (jint)interrupt;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    cleanInterrupts
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_cleanInterrupts
  (JNIEnv* env, jclass, jint interruptHandle)
{
  if (interruptHandle != HAL_kInvalidHandle) {
    HAL_CleanInterrupts((HAL_InterruptHandle)interruptHandle);
  }
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    waitForInterrupt
 * Signature: (IDZ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_InterruptJNI_waitForInterrupt
  (JNIEnv* env, jclass, jint interruptHandle, jdouble timeout,
   jboolean ignorePrevious)
{
  int32_t status = 0;
  int64_t result = HAL_WaitForInterrupt((HAL_InterruptHandle)interruptHandle,
                                        timeout, ignorePrevious, &status);

  CheckStatus(env, status);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    waitForMultipleInterrupts
 * Signature: (IJDZ)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_InterruptJNI_waitForMultipleInterrupts
  (JNIEnv* env, jclass, jint interruptHandle, jlong mask, jdouble timeout,
   jboolean ignorePrevious)
{
  int32_t status = 0;
  int64_t result =
      HAL_WaitForMultipleInterrupts((HAL_InterruptHandle)interruptHandle, mask,
                                    timeout, ignorePrevious, &status);

  CheckStatus(env, status);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    readInterruptRisingTimestamp
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_InterruptJNI_readInterruptRisingTimestamp
  (JNIEnv* env, jclass, jint interruptHandle)
{
  int32_t status = 0;
  jlong timeStamp = HAL_ReadInterruptRisingTimestamp(
      (HAL_InterruptHandle)interruptHandle, &status);

  CheckStatus(env, status);
  return timeStamp;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    readInterruptFallingTimestamp
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_InterruptJNI_readInterruptFallingTimestamp
  (JNIEnv* env, jclass, jint interruptHandle)
{
  int32_t status = 0;
  jlong timeStamp = HAL_ReadInterruptFallingTimestamp(
      (HAL_InterruptHandle)interruptHandle, &status);

  CheckStatus(env, status);
  return timeStamp;
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    requestInterrupts
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_requestInterrupts
  (JNIEnv* env, jclass, jint interruptHandle, jint digitalSourceHandle,
   jint analogTriggerType)
{
  int32_t status = 0;
  HAL_RequestInterrupts((HAL_InterruptHandle)interruptHandle,
                        (HAL_Handle)digitalSourceHandle,
                        (HAL_AnalogTriggerType)analogTriggerType, &status);

  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    setInterruptUpSourceEdge
 * Signature: (IZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_setInterruptUpSourceEdge
  (JNIEnv* env, jclass, jint interruptHandle, jboolean risingEdge,
   jboolean fallingEdge)
{
  int32_t status = 0;
  HAL_SetInterruptUpSourceEdge((HAL_InterruptHandle)interruptHandle, risingEdge,
                               fallingEdge, &status);

  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_InterruptJNI
 * Method:    releaseWaitingInterrupt
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_InterruptJNI_releaseWaitingInterrupt
  (JNIEnv* env, jclass, jint interruptHandle)
{
  int32_t status = 0;
  HAL_ReleaseWaitingInterrupt((HAL_InterruptHandle)interruptHandle, &status);

  CheckStatus(env, status);
}

}  // extern "C"
