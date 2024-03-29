// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_PWMDataJNI.h"
#include "hal/simulation/PWMData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPWMInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    registerPulseMicrosecondCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_registerPulseMicrosecondCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMPulseMicrosecondCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    cancelPulseMicrosecondCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_cancelPulseMicrosecondCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMPulseMicrosecondCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    getPulseMicrosecond
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_getPulseMicrosecond
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMPulseMicrosecond(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    setPulseMicrosecond
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_setPulseMicrosecond
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetPWMPulseMicrosecond(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    registerSpeedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_registerSpeedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMSpeedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    cancelSpeedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_cancelSpeedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index, &HALSIM_CancelPWMSpeedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    getSpeed
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_getSpeed
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMSpeed(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    setSpeed
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_setSpeed
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPWMSpeed(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    registerPositionCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_registerPositionCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMPositionCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    cancelPositionCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_cancelPositionCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMPositionCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    getPosition
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_getPosition
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMPosition(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    setPosition
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_setPosition
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPWMPosition(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    registerPeriodScaleCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_registerPeriodScaleCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMPeriodScaleCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    cancelPeriodScaleCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_cancelPeriodScaleCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMPeriodScaleCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    getPeriodScale
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_getPeriodScale
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMPeriodScale(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    setPeriodScale
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_setPeriodScale
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetPWMPeriodScale(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    registerZeroLatchCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_registerZeroLatchCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMZeroLatchCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    cancelZeroLatchCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_cancelZeroLatchCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMZeroLatchCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    getZeroLatch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_getZeroLatch
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMZeroLatch(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    setZeroLatch
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_setZeroLatch
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPWMZeroLatch(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetPWMData(index);
}

}  // extern "C"
