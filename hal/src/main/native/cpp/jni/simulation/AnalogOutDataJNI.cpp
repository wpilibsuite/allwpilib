/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_AnalogOutDataJNI.h"
#include "hal/simulation/AnalogOutData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    getDisplayName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_getDisplayName
  (JNIEnv* env, jclass, jint index)
{
  const char* displayName = HALSIM_GetAnalogOutDisplayName(index);
  return wpi::java::MakeJString(env, displayName);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    setDisplayName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_setDisplayName
  (JNIEnv* env, jclass, jint index, jstring displayName)
{
  wpi::java::JStringRef displayNameRef{env, displayName};
  HALSIM_SetAnalogOutDisplayName(index, displayNameRef.c_str());
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    registerVoltageCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_registerVoltageCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogOutVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    cancelVoltageCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_cancelVoltageCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogOutVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    getVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_getVoltage
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogOutVoltage(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    setVoltage
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_setVoltage
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogOutVoltage(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogOutInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogOutInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogOutInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAnalogOutInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_AnalogOutDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_AnalogOutDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetAnalogOutData(index);
}

}  // extern "C"
