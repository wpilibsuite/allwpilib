// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "../HALUtil.h"
#include "CallbackStore.h"
#include "OpModeOptionsCallbackStore.h"
#include "SimulatorJNI.h"
#include "org_wpilib_hardware_hal_simulation_DriverStationDataJNI.h"
#include "wpi/hal/simulation/DriverStationData.h"
#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/jni_util.hpp"

using namespace wpi::hal;
using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerEnabledCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerEnabledCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationEnabledCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelEnabledCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelEnabledCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationEnabledCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getEnabled
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getEnabled
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationEnabled();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setEnabled
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setEnabled
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationEnabled(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerRobotModeCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerRobotModeCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationRobotModeCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelRobotModeCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelRobotModeCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationRobotModeCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    nativeGetRobotMode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_nativeGetRobotMode
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationRobotMode();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    nativeSetRobotMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_nativeSetRobotMode
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetDriverStationRobotMode(static_cast<HAL_RobotMode>(value));
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerEStopCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerEStopCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterDriverStationEStopCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelEStopCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelEStopCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationEStopCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getEStop
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getEStop
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationEStop();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setEStop
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setEStop
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationEStop(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerFmsAttachedCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerFmsAttachedCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationFmsAttachedCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelFmsAttachedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelFmsAttachedCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationFmsAttachedCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getFmsAttached
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getFmsAttached
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationFmsAttached();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setFmsAttached
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setFmsAttached
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationFmsAttached(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerDsAttachedCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerDsAttachedCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationDsAttachedCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelDsAttachedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelDsAttachedCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationDsAttachedCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getDsAttached
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getDsAttached
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationDsAttached();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setDsAttached
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setDsAttached
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetDriverStationDsAttached(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerAllianceStationIdCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerAllianceStationIdCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationAllianceStationIdCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelAllianceStationIdCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelAllianceStationIdCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(
      env, handle, &HALSIM_CancelDriverStationAllianceStationIdCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getAllianceStationId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getAllianceStationId
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationAllianceStationId();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setAllianceStationId
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setAllianceStationId
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetDriverStationAllianceStationId(
      static_cast<HAL_AllianceStationID>(value));
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerMatchTimeCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerMatchTimeCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationMatchTimeCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelMatchTimeCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelMatchTimeCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationMatchTimeCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getMatchTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getMatchTime
  (JNIEnv*, jclass)
{
  return HALSIM_GetDriverStationMatchTime();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setMatchTime
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setMatchTime
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetDriverStationMatchTime(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerOpModeCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerOpModeCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterDriverStationOpModeCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelOpModeCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelOpModeCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelDriverStationOpModeCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getOpMode
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getOpMode
  (JNIEnv* env, jclass)
{
  return HALSIM_GetDriverStationOpMode();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setOpMode
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setOpMode
  (JNIEnv* env, jclass, jlong value)
{
  HALSIM_SetDriverStationOpMode(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerOpModeOptionsCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerOpModeOptionsCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateOpModeOptionsCallback(
      env, callback, initialNotify, &HALSIM_RegisterOpModeOptionsCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    cancelOpModeOptionsCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_cancelOpModeOptionsCallback
  (JNIEnv* env, jclass, jint handle)
{
  sim::FreeOpModeOptionsCallback(env, handle,
                                 &HALSIM_CancelOpModeOptionsCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getOpModeOptions
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getOpModeOptions
  (JNIEnv* env, jclass)
{
  int32_t count;
  HAL_OpModeOption* options = HALSIM_GetOpModeOptions(&count);
  auto rv = CreateOpModeOptionArray(env, {options, options + count});
  HALSIM_FreeOpModeOptionsArray(options, count);
  return rv;
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickAxes
 * Signature: (B[FS)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickAxes
  (JNIEnv* env, jclass, jbyte joystickNum, jfloatArray axesArray,
   jshort availableAxes)
{
  HAL_JoystickAxes axes;
  {
    JSpan<const jfloat> jArrayRef(env, axesArray);
    auto arrayRef = jArrayRef.array();
    auto arraySize = arrayRef.size();
    int maxCount =
        arraySize < HAL_kMaxJoystickAxes ? arraySize : HAL_kMaxJoystickAxes;
    axes.available = availableAxes;
    for (int i = 0; i < maxCount; i++) {
      axes.axes[i] = arrayRef[i];
    }
  }
  HALSIM_SetJoystickAxes(joystickNum, &axes);
  return;
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickPOVs
 * Signature: (B[BS)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickPOVs
  (JNIEnv* env, jclass, jbyte joystickNum, jbyteArray povsArray,
   jshort availablePovs)
{
  HAL_JoystickPOVs povs;
  {
    JSpan<const jbyte> jArrayRef(env, povsArray);
    auto arrayRef = jArrayRef.array();
    auto arraySize = arrayRef.size();
    int maxCount =
        arraySize < HAL_kMaxJoystickPOVs ? arraySize : HAL_kMaxJoystickPOVs;
    povs.available = availablePovs;
    for (int i = 0; i < maxCount; i++) {
      povs.povs[i] = static_cast<HAL_JoystickPOV>(arrayRef[i]);
    }
  }
  HALSIM_SetJoystickPOVs(joystickNum, &povs);
  return;
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickButtons
 * Signature: (BJJ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickButtons
  (JNIEnv* env, jclass, jbyte joystickNum, jlong buttons,
   jlong availableButtons)
{
  HAL_JoystickButtons joystickButtons;
  joystickButtons.available = availableButtons;
  joystickButtons.buttons = buttons;
  HALSIM_SetJoystickButtons(joystickNum, &joystickButtons);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getJoystickLeds
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getJoystickLeds
  (JNIEnv* env, jclass, jint stick)
{
  int32_t leds = 0;
  HALSIM_GetJoystickLeds(stick, &leds);
  return leds;
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    getJoystickRumble
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_getJoystickRumble
  (JNIEnv* env, jclass, jint stick, jint rumbleNum)
{
  int32_t leftRumble = 0;
  int32_t rightRumble = 0;
  int32_t leftTriggerRumble = 0;
  int32_t rightTriggerRumble = 0;
  HALSIM_GetJoystickRumbles(stick, &leftRumble, &rightRumble,
                            &leftTriggerRumble, &rightTriggerRumble);
  switch (rumbleNum) {
    case 0:
      return leftRumble;
    case 1:
      return rightRumble;
    case 2:
      return leftTriggerRumble;
    case 3:
      return rightTriggerRumble;
    default:
      return 0;
  }
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setMatchInfo
 * Signature: (Ljava/lang/String;Ljava/lang/String;III)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setMatchInfo
  (JNIEnv* env, jclass, jstring eventName, jstring gameSpecificMessage,
   jint matchNumber, jint replayNumber, jint matchType)
{
  JStringRef eventNameRef{env, eventName};
  JStringRef gameSpecificMessageRef{env, gameSpecificMessage};

  HAL_MatchInfo halMatchInfo;
  wpi::util::format_to_n_c_str(halMatchInfo.eventName,
                               sizeof(halMatchInfo.eventName), "{}",
                               eventNameRef.str());
  wpi::util::format_to_n_c_str(
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
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    registerAllCallbacks
 * Signature: (Ljava/lang/Object;Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_registerAllCallbacks
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
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    notifyNewData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_notifyNewData
  (JNIEnv*, jclass)
{
  HALSIM_NotifyDriverStationNewData();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setSendError
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setSendError
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
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setSendConsoleLine
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setSendConsoleLine
  (JNIEnv*, jclass, jboolean shouldSend)
{
  if (shouldSend) {
    HALSIM_SetSendConsoleLine(nullptr);
  } else {
    HALSIM_SetSendConsoleLine([](const char* line) { return 0; });
  }
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickButton
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickButton
  (JNIEnv*, jclass, jint stick, jint button, jboolean state)
{
  HALSIM_SetJoystickButton(stick, button, state);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickAxis
 * Signature: (IID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickAxis
  (JNIEnv*, jclass, jint stick, jint axis, jdouble value)
{
  HALSIM_SetJoystickAxis(stick, axis, value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickPOV
 * Signature: (IIB)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickPOV
  (JNIEnv*, jclass, jint stick, jint pov, jbyte value)
{
  HALSIM_SetJoystickPOV(stick, pov, static_cast<HAL_JoystickPOV>(value));
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickButtonsValue
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickButtonsValue
  (JNIEnv*, jclass, jint stick, jlong buttons)
{
  HALSIM_SetJoystickButtonsValue(stick, buttons);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickAxesAvailable
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickAxesAvailable
  (JNIEnv*, jclass, jint stick, jint available)
{
  HALSIM_SetJoystickAxesAvailable(stick, available);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickPOVsAvailable
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickPOVsAvailable
  (JNIEnv*, jclass, jint stick, jint available)
{
  HALSIM_SetJoystickPOVsAvailable(stick, available);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickButtonsAvailable
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickButtonsAvailable
  (JNIEnv*, jclass, jint stick, jlong available)
{
  HALSIM_SetJoystickButtonsAvailable(stick, available);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setTouchpadCounts
 * Signature: (II[I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setTouchpadCounts
  (JNIEnv* env, jclass, jint stick, jint touchpadCount, jintArray fingerCounts)
{
  JSpan<const jint> jArrayRef(env, fingerCounts);
  uint8_t fingerCountsLocal[2];
  size_t toCopy = (std::min)(jArrayRef.size(), static_cast<size_t>(2));
  for (size_t i = 0; i < toCopy; ++i) {
    fingerCountsLocal[i] = static_cast<uint8_t>(jArrayRef[i]);
  }
  HALSIM_SetJoystickTouchpadCounts(stick, touchpadCount, fingerCountsLocal);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setTouchpadFinger
 * Signature: (IIIZFF)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setTouchpadFinger
  (JNIEnv*, jclass, jint stick, jint touchpadIndex, jint fingerIndex,
   jboolean down, jfloat x, jfloat y)
{
  HALSIM_SetJoystickTouchpadFinger(stick, touchpadIndex, fingerIndex, down, x,
                                   y);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickIsGamepad
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickIsGamepad
  (JNIEnv*, jclass, jint stick, jboolean isGamepad)
{
  HALSIM_SetJoystickIsGamepad(stick, isGamepad);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickGamepadType
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickGamepadType
  (JNIEnv*, jclass, jint stick, jint type)
{
  HALSIM_SetJoystickGamepadType(stick, type);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickSupportedOutputs
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickSupportedOutputs
  (JNIEnv*, jclass, jint stick, jint supportedOutputs)
{
  HALSIM_SetJoystickSupportedOutputs(stick, supportedOutputs);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setJoystickName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setJoystickName
  (JNIEnv* env, jclass, jint stick, jstring name)
{
  JStringRef nameJString{env, name};
  auto str = wpi::util::make_string(nameJString);
  HALSIM_SetJoystickName(stick, &str);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setGameSpecificMessage
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setGameSpecificMessage
  (JNIEnv* env, jclass, jstring message)
{
  JStringRef messageJString{env, message};
  auto str = wpi::util::make_string(messageJString);
  HALSIM_SetGameSpecificMessage(&str);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setEventName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setEventName
  (JNIEnv* env, jclass, jstring name)
{
  JStringRef nameJString{env, name};
  auto str = wpi::util::make_string(nameJString);
  HALSIM_SetEventName(&str);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setMatchType
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setMatchType
  (JNIEnv*, jclass, jint type)
{
  HALSIM_SetMatchType(static_cast<HAL_MatchType>(static_cast<int>(type)));
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setMatchNumber
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setMatchNumber
  (JNIEnv*, jclass, jint matchNumber)
{
  HALSIM_SetMatchNumber(matchNumber);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    setReplayNumber
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_setReplayNumber
  (JNIEnv*, jclass, jint replayNumber)
{
  HALSIM_SetReplayNumber(replayNumber);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_DriverStationDataJNI
 * Method:    resetData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_DriverStationDataJNI_resetData
  (JNIEnv*, jclass)
{
  HALSIM_ResetDriverStationData();
}

}  // extern "C"
