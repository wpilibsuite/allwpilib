/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_PWMDataJNI.h"
#include "hal/simulation/PWMData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    getDisplayName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_getDisplayName
  (JNIEnv* env, jclass, jint index)
{
  const char* displayName = HALSIM_GetPWMDisplayName(index);
  return wpi::java::MakeJString(env, displayName);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    setDisplayName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_setDisplayName
  (JNIEnv* env, jclass, jint index, jstring displayName)
{
  wpi::java::JStringRef displayNameRef{env, displayName};
  HALSIM_SetPWMDisplayName(index, displayNameRef.c_str());
}

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
 * Method:    registerRawValueCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_registerRawValueCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMRawValueCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    cancelRawValueCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_cancelRawValueCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMRawValueCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    getRawValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_getRawValue
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMRawValue(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_PWMDataJNI
 * Method:    setRawValue
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_PWMDataJNI_setRawValue
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetPWMRawValue(index, value);
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
