// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_AnalogTriggerDataJNI.h"
#include "hal/simulation/AnalogTriggerData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogTriggerInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogTriggerInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAnalogTriggerInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    registerTriggerLowerBoundCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_registerTriggerLowerBoundCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerTriggerLowerBoundCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    cancelTriggerLowerBoundCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_cancelTriggerLowerBoundCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(
      env, handle, index, &HALSIM_CancelAnalogTriggerTriggerLowerBoundCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    getTriggerLowerBound
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_getTriggerLowerBound
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogTriggerTriggerLowerBound(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    setTriggerLowerBound
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_setTriggerLowerBound
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogTriggerTriggerLowerBound(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    registerTriggerUpperBoundCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_registerTriggerUpperBoundCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerTriggerUpperBoundCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    cancelTriggerUpperBoundCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_cancelTriggerUpperBoundCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(
      env, handle, index, &HALSIM_CancelAnalogTriggerTriggerUpperBoundCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    getTriggerUpperBound
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_getTriggerUpperBound
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogTriggerTriggerUpperBound(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    setTriggerUpperBound
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_setTriggerUpperBound
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogTriggerTriggerUpperBound(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetAnalogTriggerData(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogTriggerDataJNI
 * Method:    findForChannel
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogTriggerDataJNI_findForChannel
  (JNIEnv*, jclass, jint channel)
{
  return HALSIM_FindAnalogTriggerForChannel(channel);
}

}  // extern "C"
