// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_DutyCycleDataJNI.h"
#include "hal/simulation/DutyCycleData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDutyCycleInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDutyCycleInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDutyCycleInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetDutyCycleInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    registerFrequencyCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_registerFrequencyCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDutyCycleFrequencyCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    cancelFrequencyCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_cancelFrequencyCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDutyCycleFrequencyCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    getFrequency
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_getFrequency
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDutyCycleFrequency(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    setFrequency
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_setFrequency
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetDutyCycleFrequency(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    registerOutputCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_registerOutputCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDutyCycleOutputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    cancelOutputCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_cancelOutputCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDutyCycleOutputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    getOutput
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_getOutput
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDutyCycleOutput(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    setOutput
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_setOutput
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetDutyCycleOutput(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetDutyCycleData(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DutyCycleDataJNI
 * Method:    findForChannel
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DutyCycleDataJNI_findForChannel
  (JNIEnv*, jclass, jint channel)
{
  return HALSIM_FindDutyCycleForChannel(channel);
}

}  // extern "C"
