/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cstring>

#include <wpi/jni_util.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI.h"
#include "mockdata/DriverStationData.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    registerEnabledCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_registerEnabledCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationEnabledCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    cancelEnabledCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_cancelEnabledCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationEnabledCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    getEnabled
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_getEnabled
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationEnabled();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setEnabled
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setEnabled
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationEnabled(value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    registerAutonomousCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_registerAutonomousCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationAutonomousCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    cancelAutonomousCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_cancelAutonomousCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationAutonomousCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    getAutonomous
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_getAutonomous
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationAutonomous();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setAutonomous
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setAutonomous
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationAutonomous(value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    registerTestCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_registerTestCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterDriverStationTestCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    cancelTestCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_cancelTestCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationTestCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    getTest
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_getTest
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationTest();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setTest
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setTest
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationTest(value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    registerEStopCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_registerEStopCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterDriverStationEStopCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    cancelEStopCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_cancelEStopCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationEStopCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    getEStop
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_getEStop
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationEStop();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setEStop
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setEStop
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationEStop(value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    registerFmsAttachedCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_registerFmsAttachedCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationFmsAttachedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    cancelFmsAttachedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_cancelFmsAttachedCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationFmsAttachedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    getFmsAttached
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_getFmsAttached
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationFmsAttached();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setFmsAttached
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setFmsAttached
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationFmsAttached(value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    registerDsAttachedCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_registerDsAttachedCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationDsAttachedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    cancelDsAttachedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_cancelDsAttachedCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationDsAttachedCallback);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    getDsAttached
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_getDsAttached
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationDsAttached();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setDsAttached
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setDsAttached
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationDsAttached(value);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setJoystickAxes
 * Signature: (B[F)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setJoystickAxes
  (JNIEnv* env, jclass, jbyte joystickNum, jfloatArray axesArray)
{
  HAL_JoystickAxes axes;
  {
    wpi::java::JFloatArrayRef jArrayRef(env, axesArray);
    auto arrayRef = jArrayRef.array();
    auto arraySize = arrayRef.size();
    int maxCount =
        arraySize < HAL_kMaxJoystickAxes ? arraySize : HAL_kMaxJoystickAxes;
    axes.count = maxCount;
    for (int i = 0; i < maxCount; i++) {
      axes.axes[i] = arrayRef[i];
    }
  }
  HALSIM_SetJoystickAxes(joystickNum, &axes);
  return;
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setJoystickPOVs
 * Signature: (B[S)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setJoystickPOVs
  (JNIEnv* env, jclass, jbyte joystickNum, jshortArray povsArray)
{
  HAL_JoystickPOVs povs;
  {
    wpi::java::JShortArrayRef jArrayRef(env, povsArray);
    auto arrayRef = jArrayRef.array();
    auto arraySize = arrayRef.size();
    int maxCount =
        arraySize < HAL_kMaxJoystickPOVs ? arraySize : HAL_kMaxJoystickPOVs;
    povs.count = maxCount;
    for (int i = 0; i < maxCount; i++) {
      povs.povs[i] = arrayRef[i];
    }
  }
  HALSIM_SetJoystickPOVs(joystickNum, &povs);
  return;
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setJoystickButtons
 * Signature: (BII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setJoystickButtons
  (JNIEnv* env, jclass, jbyte joystickNum, jint buttons, jint count)
{
  if (count > 32) {
    count = 32;
  }
  HAL_JoystickButtons joystickButtons;
  joystickButtons.count = count;
  joystickButtons.buttons = buttons;
  HALSIM_SetJoystickButtons(joystickNum, &joystickButtons);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    setMatchInfo
 * Signature: (Ljava/lang/String;Ljava/lang/String;III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_setMatchInfo
  (JNIEnv* env, jclass, jstring eventName, jstring gameSpecificMessage,
   jint matchNumber, jint replayNumber, jint matchType)
{
  JStringRef eventNameRef{env, eventName};
  JStringRef gameSpecificMessageRef{env, gameSpecificMessage};

  HAL_MatchInfo halMatchInfo;
  std::snprintf(halMatchInfo.eventName, sizeof(halMatchInfo.eventName), "%s",
                eventNameRef.c_str());
  std::snprintf(reinterpret_cast<char*>(halMatchInfo.gameSpecificMessage),
                sizeof(halMatchInfo.gameSpecificMessage), "%s",
                gameSpecificMessageRef.c_str());
  halMatchInfo.gameSpecificMessageSize = gameSpecificMessageRef.size();
  halMatchInfo.matchType = (HAL_MatchType)matchType;
  halMatchInfo.matchNumber = matchNumber;
  halMatchInfo.replayNumber = replayNumber;
  HALSIM_SetMatchInfo(&halMatchInfo);
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    registerAllCallbacks
 * Signature: (Ljava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_registerAllCallbacks
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      [](HAL_NotifyCallback cb, void* param, HAL_Bool in) {
        HALSIM_RegisterDriverStationAllCallbacks(cb, param, in);
        return 0;
      });
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    notifyNewData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_notifyNewData
  (JNIEnv*, jclass)
{
  HALSIM_NotifyDriverStationNewData();
}

/*
 * Class:     edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI
 * Method:    resetData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_sim_mockdata_DriverStationDataJNI_resetData
  (JNIEnv*, jclass)
{
  HALSIM_ResetDriverStationData();
}

}  // extern "C"
