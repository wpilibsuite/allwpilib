/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI.h"
#include "mockdata/AnalogGyroData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    registerAngleCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_registerAngleCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogGyroAngleCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    cancelAngleCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_cancelAngleCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogGyroAngleCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    getAngle
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_getAngle
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogGyroAngle(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    setAngle
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_setAngle
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogGyroAngle(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    registerRateCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_registerRateCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogGyroRateCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    cancelRateCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_cancelRateCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogGyroRateCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    getRate
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_getRate
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogGyroRate(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    setRate
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_setRate
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetAnalogGyroRate(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterAnalogGyroInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelAnalogGyroInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetAnalogGyroInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetAnalogGyroInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_AnalogGyroDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetAnalogGyroData(index);
}

}  // extern "C"
