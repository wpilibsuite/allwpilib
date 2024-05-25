// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/StringExtras.h>
#include <wpi/jni_util.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_DriverStationDataJNI.h"
#include "hal/simulation/DriverStationData.h"
#include "hal/simulation/MockHooks.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerEnabledCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerEnabledCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationEnabledCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    cancelEnabledCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_cancelEnabledCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationEnabledCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getEnabled
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getEnabled
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationEnabled();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setEnabled
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setEnabled
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationEnabled(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerAutonomousCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerAutonomousCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationAutonomousCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    cancelAutonomousCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_cancelAutonomousCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationAutonomousCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getAutonomous
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getAutonomous
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationAutonomous();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setAutonomous
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setAutonomous
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationAutonomous(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerTestCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerTestCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterDriverStationTestCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    cancelTestCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_cancelTestCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationTestCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getTest
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getTest
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationTest();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setTest
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setTest
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationTest(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerEStopCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerEStopCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterDriverStationEStopCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    cancelEStopCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_cancelEStopCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationEStopCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getEStop
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getEStop
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationEStop();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setEStop
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setEStop
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationEStop(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerFmsAttachedCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerFmsAttachedCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationFmsAttachedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    cancelFmsAttachedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_cancelFmsAttachedCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationFmsAttachedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getFmsAttached
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getFmsAttached
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationFmsAttached();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setFmsAttached
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setFmsAttached
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationFmsAttached(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerDsAttachedCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerDsAttachedCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationDsAttachedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    cancelDsAttachedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_cancelDsAttachedCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationDsAttachedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getDsAttached
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getDsAttached
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationDsAttached();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setDsAttached
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setDsAttached
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationDsAttached(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerAllianceStationIdCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerAllianceStationIdCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationAllianceStationIdCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    cancelAllianceStationIdCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_cancelAllianceStationIdCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationAllianceStationIdCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getAllianceStationId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getAllianceStationId
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationAllianceStationId();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setAllianceStationId
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setAllianceStationId
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetDriverStationAllianceStationId(
      static_cast<HAL_AllianceStationID>(value));
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerMatchTimeCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerMatchTimeCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationMatchTimeCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    cancelMatchTimeCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_cancelMatchTimeCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationMatchTimeCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getMatchTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getMatchTime
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationMatchTime();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setMatchTime
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setMatchTime
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetDriverStationMatchTime(value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickAxes
 * Signature: (B[F)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickAxes
  (JNIEnv* env, jclass, jbyte joystickNum, jfloatArray axesArray)
{
  HAL_JoystickAxes axes;
  {
    JSpan<const jfloat> jArrayRef(env, axesArray);
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
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickPOVs
 * Signature: (B[S)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickPOVs
  (JNIEnv* env, jclass, jbyte joystickNum, jshortArray povsArray)
{
  HAL_JoystickPOVs povs;
  {
    JSpan<const jshort> jArrayRef(env, povsArray);
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
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickButtons
 * Signature: (BII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickButtons
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
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getJoystickOutputs
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getJoystickOutputs
  (JNIEnv* env, jclass, jint stick)
{
  int64_t outputs = 0;
  int32_t leftRumble;
  int32_t rightRumble;
  HALSIM_GetJoystickOutputs(stick, &outputs, &leftRumble, &rightRumble);
  return outputs;
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    getJoystickRumble
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_getJoystickRumble
  (JNIEnv* env, jclass, jint stick, jint rumbleNum)
{
  int64_t outputs;
  int32_t leftRumble = 0;
  int32_t rightRumble = 0;
  HALSIM_GetJoystickOutputs(stick, &outputs, &leftRumble, &rightRumble);
  return rumbleNum == 0 ? leftRumble : rightRumble;
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setMatchInfo
 * Signature: (Ljava/lang/String;Ljava/lang/String;III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setMatchInfo
  (JNIEnv* env, jclass, jstring eventName, jstring gameSpecificMessage,
   jint matchNumber, jint replayNumber, jint matchType)
{
  JStringRef eventNameRef{env, eventName};
  JStringRef gameSpecificMessageRef{env, gameSpecificMessage};

  HAL_MatchInfo halMatchInfo;
  wpi::format_to_n_c_str(halMatchInfo.eventName, sizeof(halMatchInfo.eventName),
                         "{}", eventNameRef.str());
  wpi::format_to_n_c_str(
      reinterpret_cast<char*>(halMatchInfo.gameSpecificMessage),
      sizeof(halMatchInfo.gameSpecificMessage), "{}",
      gameSpecificMessageRef.str());
  halMatchInfo.gameSpecificMessageSize = gameSpecificMessageRef.size();
  halMatchInfo.matchType = (HAL_MatchType)matchType;
  halMatchInfo.matchNumber = matchNumber;
  halMatchInfo.replayNumber = replayNumber;
  HALSIM_SetMatchInfo(&halMatchInfo);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    registerAllCallbacks
 * Signature: (Ljava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_registerAllCallbacks
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
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    notifyNewData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_notifyNewData
  (JNIEnv*, jclass)
{
  HALSIM_NotifyDriverStationNewData();
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setSendError
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setSendError
  (JNIEnv*, jclass, jboolean shouldSend)
{
  if (shouldSend) {
    HALSIM_SetSendError(nullptr);
  } else {
    HALSIM_SetSendError([](HAL_Bool isError, int32_t errorCode,
                           HAL_Bool isLVCode, const char* details,
                           const char* location, const char* callStack,
                           HAL_Bool printMsg) { return 0; });
  }
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setSendConsoleLine
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setSendConsoleLine
  (JNIEnv*, jclass, jboolean shouldSend)
{
  if (shouldSend) {
    HALSIM_SetSendConsoleLine(nullptr);
  } else {
    HALSIM_SetSendConsoleLine([](const char* line) { return 0; });
  }
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickButton
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickButton
  (JNIEnv*, jclass, jint stick, jint button, jboolean state)
{
  HALSIM_SetJoystickButton(stick, button, state);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickAxis
 * Signature: (IID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickAxis
  (JNIEnv*, jclass, jint stick, jint axis, jdouble value)
{
  HALSIM_SetJoystickAxis(stick, axis, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickPOV
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickPOV
  (JNIEnv*, jclass, jint stick, jint pov, jint value)
{
  HALSIM_SetJoystickPOV(stick, pov, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickButtonsValue
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickButtonsValue
  (JNIEnv*, jclass, jint stick, jint buttons)
{
  HALSIM_SetJoystickButtonsValue(stick, buttons);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickAxisCount
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickAxisCount
  (JNIEnv*, jclass, jint stick, jint count)
{
  HALSIM_SetJoystickAxisCount(stick, count);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickPOVCount
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickPOVCount
  (JNIEnv*, jclass, jint stick, jint count)
{
  HALSIM_SetJoystickPOVCount(stick, count);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickButtonCount
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickButtonCount
  (JNIEnv*, jclass, jint stick, jint count)
{
  HALSIM_SetJoystickButtonCount(stick, count);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickIsXbox
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickIsXbox
  (JNIEnv*, jclass, jint stick, jboolean isXbox)
{
  HALSIM_SetJoystickIsXbox(stick, isXbox);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickType
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickType
  (JNIEnv*, jclass, jint stick, jint type)
{
  HALSIM_SetJoystickType(stick, type);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickName
  (JNIEnv* env, jclass, jint stick, jstring name)
{
  JStringRef nameJString{env, name};
  auto str = wpi::make_string(nameJString);
  HALSIM_SetJoystickName(stick, &str);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickAxisType
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setJoystickAxisType
  (JNIEnv*, jclass, jint stick, jint axis, jint type)
{
  HALSIM_SetJoystickAxisType(stick, axis, type);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setGameSpecificMessage
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setGameSpecificMessage
  (JNIEnv* env, jclass, jstring message)
{
  JStringRef messageJString{env, message};
  auto str = wpi::make_string(messageJString);
  HALSIM_SetGameSpecificMessage(&str);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setEventName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setEventName
  (JNIEnv* env, jclass, jstring name)
{
  JStringRef nameJString{env, name};
  auto str = wpi::make_string(nameJString);
  HALSIM_SetEventName(&str);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setMatchType
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setMatchType
  (JNIEnv*, jclass, jint type)
{
  HALSIM_SetMatchType(static_cast<HAL_MatchType>(static_cast<int>(type)));
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setMatchNumber
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setMatchNumber
  (JNIEnv*, jclass, jint matchNumber)
{
  HALSIM_SetMatchNumber(matchNumber);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    setReplayNumber
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_setReplayNumber
  (JNIEnv*, jclass, jint replayNumber)
{
  HALSIM_SetReplayNumber(replayNumber);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DriverStationDataJNI
 * Method:    resetData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DriverStationDataJNI_resetData
  (JNIEnv*, jclass)
{
  HALSIM_ResetDriverStationData();
}

}  // extern "C"
