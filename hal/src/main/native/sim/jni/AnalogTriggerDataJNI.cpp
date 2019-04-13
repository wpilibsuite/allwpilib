/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI.h"
#include "mockdata/AnalogTriggerData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogTriggerInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogTriggerInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAnalogTriggerInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    registerTriggerLowerBoundCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_registerTriggerLowerBoundCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerTriggerLowerBoundCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    cancelTriggerLowerBoundCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_cancelTriggerLowerBoundCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(
      env, handle, index, &HALSIM_CancelAnalogTriggerTriggerLowerBoundCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    getTriggerLowerBound
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_getTriggerLowerBound
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogTriggerTriggerLowerBound(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    setTriggerLowerBound
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_setTriggerLowerBound
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogTriggerTriggerLowerBound(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    registerTriggerUpperBoundCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_registerTriggerUpperBoundCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(
      env, index, callback, initialNotify,
      &HALSIM_RegisterAnalogTriggerTriggerUpperBoundCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    cancelTriggerUpperBoundCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_cancelTriggerUpperBoundCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(
      env, handle, index, &HALSIM_CancelAnalogTriggerTriggerUpperBoundCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    getTriggerUpperBound
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_getTriggerUpperBound
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogTriggerTriggerUpperBound(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    setTriggerUpperBound
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_setTriggerUpperBound
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogTriggerTriggerUpperBound(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogTriggerDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetAnalogTriggerData(index);
}

}  // extern "C"
