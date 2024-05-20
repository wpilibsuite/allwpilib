// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_RoboRioDataJNI.h"
#include "hal/simulation/RoboRioData.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerFPGAButtonCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerFPGAButtonCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterRoboRioFPGAButtonCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelFPGAButtonCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelFPGAButtonCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioFPGAButtonCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getFPGAButton
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getFPGAButton
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioFPGAButton();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setFPGAButton
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setFPGAButton
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetRoboRioFPGAButton(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerVInVoltageCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerVInVoltageCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterRoboRioVInVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelVInVoltageCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelVInVoltageCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioVInVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getVInVoltage
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getVInVoltage
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioVInVoltage();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setVInVoltage
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setVInVoltage
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioVInVoltage(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerVInCurrentCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerVInCurrentCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterRoboRioVInCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelVInCurrentCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelVInCurrentCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioVInCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getVInCurrent
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getVInCurrent
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioVInCurrent();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setVInCurrent
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setVInCurrent
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioVInCurrent(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserVoltage6VCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserVoltage6VCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserVoltage6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserVoltage6VCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserVoltage6VCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserVoltage6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserVoltage6V
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserVoltage6V
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserVoltage6V();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserVoltage6V
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserVoltage6V
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioUserVoltage6V(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserCurrent6VCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserCurrent6VCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserCurrent6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserCurrent6VCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserCurrent6VCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserCurrent6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserCurrent6V
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserCurrent6V
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserCurrent6V();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserCurrent6V
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserCurrent6V
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioUserCurrent6V(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserActive6VCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserActive6VCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserActive6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserActive6VCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserActive6VCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserActive6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserActive6V
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserActive6V
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserActive6V();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserActive6V
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserActive6V
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetRoboRioUserActive6V(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserVoltage5VCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserVoltage5VCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserVoltage5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserVoltage5VCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserVoltage5VCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserVoltage5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserVoltage5V
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserVoltage5V
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserVoltage5V();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserVoltage5V
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserVoltage5V
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioUserVoltage5V(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserCurrent5VCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserCurrent5VCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserCurrent5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserCurrent5VCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserCurrent5VCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserCurrent5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserCurrent5V
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserCurrent5V
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserCurrent5V();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserCurrent5V
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserCurrent5V
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioUserCurrent5V(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserActive5VCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserActive5VCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserActive5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserActive5VCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserActive5VCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserActive5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserActive5V
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserActive5V
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserActive5V();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserActive5V
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserActive5V
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetRoboRioUserActive5V(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserVoltage3V3Callback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserVoltage3V3Callback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserVoltage3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserVoltage3V3Callback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserVoltage3V3Callback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserVoltage3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserVoltage3V3
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserVoltage3V3
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserVoltage3V3();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserVoltage3V3
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserVoltage3V3
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioUserVoltage3V3(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserCurrent3V3Callback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserCurrent3V3Callback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserCurrent3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserCurrent3V3Callback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserCurrent3V3Callback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserCurrent3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserCurrent3V3
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserCurrent3V3
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserCurrent3V3();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserCurrent3V3
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserCurrent3V3
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioUserCurrent3V3(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserActive3V3Callback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserActive3V3Callback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserActive3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserActive3V3Callback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserActive3V3Callback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserActive3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserActive3V3
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserActive3V3
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserActive3V3();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserActive3V3
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserActive3V3
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetRoboRioUserActive3V3(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserFaults6VCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserFaults6VCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserFaults6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserFaults6VCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserFaults6VCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserFaults6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserFaults6V
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserFaults6V
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserFaults6V();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserFaults6V
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserFaults6V
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetRoboRioUserFaults6V(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserFaults5VCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserFaults5VCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserFaults5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserFaults5VCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserFaults5VCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserFaults5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserFaults5V
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserFaults5V
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserFaults5V();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserFaults5V
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserFaults5V
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetRoboRioUserFaults5V(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerUserFaults3V3Callback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerUserFaults3V3Callback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserFaults3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserFaults3V3Callback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelUserFaults3V3Callback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserFaults3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getUserFaults3V3
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getUserFaults3V3
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserFaults3V3();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setUserFaults3V3
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setUserFaults3V3
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetRoboRioUserFaults3V3(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerBrownoutVoltageCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerBrownoutVoltageCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioBrownoutVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelBrownoutVoltageCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelBrownoutVoltageCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioBrownoutVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getBrownoutVoltage
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getBrownoutVoltage
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioBrownoutVoltage();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setBrownoutVoltage
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setBrownoutVoltage
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioBrownoutVoltage(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerCPUTempCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerCPUTempCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(env, callback, initialNotify,
                                      &HALSIM_RegisterRoboRioCPUTempCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelCPUTempCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelCPUTempCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioCPUTempCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getCPUTemp
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getCPUTemp
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioCPUTemp();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setCPUTemp
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setCPUTemp
  (JNIEnv*, jclass, jdouble cpuTemp)
{
  HALSIM_SetRoboRioCPUTemp(cpuTemp);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerTeamNumberCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerTeamNumberCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterRoboRioTeamNumberCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelTeamNumberCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelTeamNumberCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioTeamNumberCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getTeamNumber
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getTeamNumber
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioTeamNumber();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setTeamNumber
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setTeamNumber
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetRoboRioTeamNumber(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getSerialNumber
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getSerialNumber
  (JNIEnv* env, jclass)
{
  WPI_String str;
  HALSIM_GetRoboRioSerialNumber(&str);
  auto jstr = MakeJString(env, wpi::to_string_view(&str));
  WPI_FreeString(&str);
  return jstr;
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setSerialNumber
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setSerialNumber
  (JNIEnv* env, jclass, jstring serialNumber)
{
  JStringRef serialNumberJString{env, serialNumber};
  auto str = wpi::make_string(serialNumberJString);
  HALSIM_SetRoboRioSerialNumber(&str);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getComments
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getComments
  (JNIEnv* env, jclass)
{
  WPI_String str;
  HALSIM_GetRoboRioComments(&str);
  auto jstr = MakeJString(env, wpi::to_string_view(&str));
  WPI_FreeString(&str);
  return jstr;
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setComments
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setComments
  (JNIEnv* env, jclass, jstring comments)
{
  JStringRef commentsJString{env, comments};
  auto str = wpi::make_string(commentsJString);
  HALSIM_SetRoboRioComments(&str);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    registerRadioLEDStateCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_registerRadioLEDStateCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioRadioLEDStateCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    cancelRadioLEDStateCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_cancelRadioLEDStateCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioRadioLEDStateCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    getRadioLEDState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_getRadioLEDState
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioRadioLEDState();
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    setRadioLEDState
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_setRadioLEDState
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetRoboRioRadioLEDState(static_cast<HAL_RadioLEDState>(value));
}

/*
 * Class:     edu_wpi_first_hal_simulation_RoboRioDataJNI
 * Method:    resetData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_RoboRioDataJNI_resetData
  (JNIEnv*, jclass)
{
  HALSIM_ResetRoboRioData();
}

}  // extern "C"
