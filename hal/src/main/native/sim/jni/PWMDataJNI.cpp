/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_PWMDataJNI.h"
#include "mockdata/PWMData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPWMInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    registerRawValueCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerRawValueCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMRawValueCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    cancelRawValueCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelRawValueCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMRawValueCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    getRawValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getRawValue
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMRawValue(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    setRawValue
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setRawValue
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetPWMRawValue(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    registerSpeedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerSpeedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMSpeedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    cancelSpeedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelSpeedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index, &HALSIM_CancelPWMSpeedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    getSpeed
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getSpeed
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMSpeed(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    setSpeed
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setSpeed
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPWMSpeed(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    registerPositionCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerPositionCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMPositionCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    cancelPositionCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelPositionCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMPositionCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    getPosition
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getPosition
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMPosition(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    setPosition
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setPosition
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetPWMPosition(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    registerPeriodScaleCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerPeriodScaleCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMPeriodScaleCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    cancelPeriodScaleCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelPeriodScaleCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMPeriodScaleCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    getPeriodScale
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getPeriodScale
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMPeriodScale(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    setPeriodScale
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setPeriodScale
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetPWMPeriodScale(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    registerZeroLatchCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_registerZeroLatchCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMZeroLatchCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    cancelZeroLatchCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_cancelZeroLatchCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMZeroLatchCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    getZeroLatch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_getZeroLatch
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMZeroLatch(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    setZeroLatch
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_setZeroLatch
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPWMZeroLatch(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_PWMDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_PWMDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetPWMData(index);
}

}  // extern "C"
