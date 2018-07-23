/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI.h"
#include "mockdata/RoboRioData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerFPGAButtonCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerFPGAButtonCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioFPGAButtonCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelFPGAButtonCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelFPGAButtonCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioFPGAButtonCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getFPGAButton
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getFPGAButton
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioFPGAButton(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setFPGAButton
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setFPGAButton
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetRoboRioFPGAButton(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerVInVoltageCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerVInVoltageCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioVInVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelVInVoltageCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelVInVoltageCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioVInVoltageCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getVInVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getVInVoltage
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioVInVoltage(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setVInVoltage
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setVInVoltage
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetRoboRioVInVoltage(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerVInCurrentCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerVInCurrentCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioVInCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelVInCurrentCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelVInCurrentCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioVInCurrentCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getVInCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getVInCurrent
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioVInCurrent(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setVInCurrent
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setVInCurrent
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetRoboRioVInCurrent(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserVoltage6VCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserVoltage6VCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserVoltage6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserVoltage6VCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserVoltage6VCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserVoltage6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserVoltage6V
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserVoltage6V
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserVoltage6V(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserVoltage6V
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserVoltage6V
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetRoboRioUserVoltage6V(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserCurrent6VCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserCurrent6VCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserCurrent6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserCurrent6VCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserCurrent6VCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserCurrent6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserCurrent6V
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserCurrent6V
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserCurrent6V(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserCurrent6V
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserCurrent6V
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetRoboRioUserCurrent6V(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserActive6VCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserActive6VCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserActive6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserActive6VCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserActive6VCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserActive6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserActive6V
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserActive6V
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserActive6V(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserActive6V
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserActive6V
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetRoboRioUserActive6V(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserVoltage5VCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserVoltage5VCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserVoltage5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserVoltage5VCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserVoltage5VCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserVoltage5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserVoltage5V
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserVoltage5V
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserVoltage5V(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserVoltage5V
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserVoltage5V
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetRoboRioUserVoltage5V(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserCurrent5VCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserCurrent5VCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserCurrent5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserCurrent5VCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserCurrent5VCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserCurrent5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserCurrent5V
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserCurrent5V
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserCurrent5V(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserCurrent5V
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserCurrent5V
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetRoboRioUserCurrent5V(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserActive5VCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserActive5VCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserActive5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserActive5VCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserActive5VCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserActive5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserActive5V
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserActive5V
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserActive5V(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserActive5V
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserActive5V
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetRoboRioUserActive5V(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserVoltage3V3Callback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserVoltage3V3Callback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserVoltage3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserVoltage3V3Callback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserVoltage3V3Callback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserVoltage3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserVoltage3V3
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserVoltage3V3
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserVoltage3V3(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserVoltage3V3
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserVoltage3V3
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetRoboRioUserVoltage3V3(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserCurrent3V3Callback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserCurrent3V3Callback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserCurrent3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserCurrent3V3Callback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserCurrent3V3Callback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserCurrent3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserCurrent3V3
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserCurrent3V3
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserCurrent3V3(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserCurrent3V3
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserCurrent3V3
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetRoboRioUserCurrent3V3(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserActive3V3Callback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserActive3V3Callback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserActive3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserActive3V3Callback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserActive3V3Callback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserActive3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserActive3V3
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserActive3V3
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserActive3V3(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserActive3V3
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserActive3V3
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetRoboRioUserActive3V3(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserFaults6VCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserFaults6VCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserFaults6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserFaults6VCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserFaults6VCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserFaults6VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserFaults6V
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserFaults6V
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserFaults6V(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserFaults6V
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserFaults6V
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetRoboRioUserFaults6V(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserFaults5VCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserFaults5VCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserFaults5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserFaults5VCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserFaults5VCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserFaults5VCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserFaults5V
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserFaults5V
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserFaults5V(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserFaults5V
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserFaults5V
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetRoboRioUserFaults5V(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    registerUserFaults3V3Callback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_registerUserFaults3V3Callback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterRoboRioUserFaults3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    cancelUserFaults3V3Callback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_cancelUserFaults3V3Callback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelRoboRioUserFaults3V3Callback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    getUserFaults3V3
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_getUserFaults3V3
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetRoboRioUserFaults3V3(index);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    setUserFaults3V3
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_setUserFaults3V3
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetRoboRioUserFaults3V3(index, value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_RoboRioDataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetRoboRioData(index);
}

}  // extern "C"
