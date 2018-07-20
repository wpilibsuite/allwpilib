/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_RelayDataJNI.h"
#include "mockdata/RelayData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    registerInitializedForwardCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_registerInitializedForwardCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRelayInitializedForwardCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    cancelInitializedForwardCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_cancelInitializedForwardCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRelayInitializedForwardCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    getInitializedForward
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_getInitializedForward
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRelayInitializedForward(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    setInitializedForward
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_setInitializedForward
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetRelayInitializedForward(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    registerInitializedReverseCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_registerInitializedReverseCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRelayInitializedReverseCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    cancelInitializedReverseCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_cancelInitializedReverseCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRelayInitializedReverseCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    getInitializedReverse
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_getInitializedReverse
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRelayInitializedReverse(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    setInitializedReverse
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_setInitializedReverse
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetRelayInitializedReverse(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    registerForwardCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_registerForwardCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRelayForwardCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    cancelForwardCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_cancelForwardCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRelayForwardCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    getForward
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_getForward
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRelayForward(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    setForward
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_setForward
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetRelayForward(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    registerReverseCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_registerReverseCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRelayReverseCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    cancelReverseCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_cancelReverseCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRelayReverseCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    getReverse
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_getReverse
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRelayReverse(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    setReverse
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_setReverse
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetRelayReverse(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RelayDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RelayDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetRelayData(index);
}

}  // extern "C"
