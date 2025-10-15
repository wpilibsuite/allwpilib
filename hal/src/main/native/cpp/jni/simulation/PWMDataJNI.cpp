// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "org_wpilib_hardware_hal_simulation_PWMDataJNI.h"
#include "wpi/hal/simulation/PWMData.h"

using namespace hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMInitializedCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMInitializedCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMInitialized(index);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetPWMInitialized(index, value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    registerPulseMicrosecondCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_registerPulseMicrosecondCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMPulseMicrosecondCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    cancelPulseMicrosecondCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_cancelPulseMicrosecondCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMPulseMicrosecondCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    getPulseMicrosecond
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_getPulseMicrosecond
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMPulseMicrosecond(index);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    setPulseMicrosecond
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_setPulseMicrosecond
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetPWMPulseMicrosecond(index, value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    registerOutputPeriodCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_registerOutputPeriodCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterPWMOutputPeriodCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    cancelOutputPeriodCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_cancelOutputPeriodCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelPWMOutputPeriodCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    getOutputPeriod
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_getOutputPeriod
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetPWMOutputPeriod(index);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    setOutputPeriod
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_setOutputPeriod
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetPWMOutputPeriod(index, value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_PWMDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_PWMDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetPWMData(index);
}

}  // extern "C"
