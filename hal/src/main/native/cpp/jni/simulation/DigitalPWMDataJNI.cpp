// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "CallbackStore.h"
#include "org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI.h"
#include "hal/simulation/DigitalPWMData.h"

using namespace hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDigitalPWMInitializedCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDigitalPWMInitializedCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDigitalPWMInitialized(index);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetDigitalPWMInitialized(index, value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    registerDutyCycleCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_registerDutyCycleCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDigitalPWMDutyCycleCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    cancelDutyCycleCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_cancelDutyCycleCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDigitalPWMDutyCycleCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    getDutyCycle
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_getDutyCycle
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDigitalPWMDutyCycle(index);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    setDutyCycle
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_setDutyCycle
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetDigitalPWMDutyCycle(index, value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    registerPinCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_registerPinCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDigitalPWMPinCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    cancelPinCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_cancelPinCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDigitalPWMPinCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    getPin
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_getPin
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDigitalPWMPin(index);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    setPin
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_setPin
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetDigitalPWMPin(index, value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetDigitalPWMData(index);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI
 * Method:    findForChannel
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DigitalPWMDataJNI_findForChannel
  (JNIEnv*, jclass, jint channel)
{
  return HALSIM_FindDigitalPWMForChannel(channel);
}

}  // extern "C"
