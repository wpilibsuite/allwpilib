/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI.h"
#include "mockdata/DigitalPWMData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDigitalPWMInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDigitalPWMInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDigitalPWMInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetDigitalPWMInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    registerDutyCycleCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_registerDutyCycleCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDigitalPWMDutyCycleCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    cancelDutyCycleCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_cancelDutyCycleCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDigitalPWMDutyCycleCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    getDutyCycle
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_getDutyCycle
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDigitalPWMDutyCycle(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    setDutyCycle
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_setDutyCycle
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetDigitalPWMDutyCycle(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    registerPinCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_registerPinCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDigitalPWMPinCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    cancelPinCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_cancelPinCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDigitalPWMPinCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    getPin
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_getPin
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDigitalPWMPin(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    setPin
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_setPin
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetDigitalPWMPin(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DigitalPWMDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetDigitalPWMData(index);
}

}  // extern "C"
