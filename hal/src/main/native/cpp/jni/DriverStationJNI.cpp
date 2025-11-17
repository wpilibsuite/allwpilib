// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <fmt/format.h>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_DriverStationJNI.h"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/HALBase.h"
#include "wpi/util/jni_util.hpp"

static_assert(
    org_wpilib_hardware_hal_DriverStationJNI_kUnknownAllianceStation ==
    HAL_AllianceStationID_kUnknown);
static_assert(org_wpilib_hardware_hal_DriverStationJNI_kRed1AllianceStation ==
              HAL_AllianceStationID_kRed1);
static_assert(org_wpilib_hardware_hal_DriverStationJNI_kRed2AllianceStation ==
              HAL_AllianceStationID_kRed2);
static_assert(org_wpilib_hardware_hal_DriverStationJNI_kRed3AllianceStation ==
              HAL_AllianceStationID_kRed3);
static_assert(org_wpilib_hardware_hal_DriverStationJNI_kBlue1AllianceStation ==
              HAL_AllianceStationID_kBlue1);
static_assert(org_wpilib_hardware_hal_DriverStationJNI_kBlue2AllianceStation ==
              HAL_AllianceStationID_kBlue2);
static_assert(org_wpilib_hardware_hal_DriverStationJNI_kBlue3AllianceStation ==
              HAL_AllianceStationID_kBlue3);

static_assert(org_wpilib_hardware_hal_DriverStationJNI_kMaxJoystickAxes ==
              HAL_kMaxJoystickAxes);
static_assert(org_wpilib_hardware_hal_DriverStationJNI_kMaxJoystickPOVs ==
              HAL_kMaxJoystickPOVs);
static_assert(org_wpilib_hardware_hal_DriverStationJNI_kMaxJoysticks ==
              HAL_kMaxJoysticks);

using namespace wpi::hal;
using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    observeUserProgramStarting
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_observeUserProgramStarting
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramStarting();
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    observeUserProgramDisabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_observeUserProgramDisabled
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramDisabled();
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    observeUserProgramAutonomous
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_observeUserProgramAutonomous
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramAutonomous();
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    observeUserProgramTeleop
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_observeUserProgramTeleop
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramTeleop();
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    observeUserProgramTest
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_observeUserProgramTest
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramTest();
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    nativeGetControlWord
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_nativeGetControlWord
  (JNIEnv*, jclass)
{
  static_assert(sizeof(HAL_ControlWord) == sizeof(jint),
                "Java int must match the size of control word");
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  jint retVal = 0;
  std::memcpy(&retVal, &controlWord, sizeof(HAL_ControlWord));
  return retVal;
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    nativeGetAllianceStation
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_nativeGetAllianceStation
  (JNIEnv*, jclass)
{
  int32_t status = 0;
  auto allianceStation = HAL_GetAllianceStation(&status);
  return static_cast<jint>(allianceStation);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    getAllJoystickData
 * Signature: (I[F[S[B[J)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_getAllJoystickData
  (JNIEnv* env, jclass cls, jint stick, jfloatArray axesArray,
   jshortArray rawAxesArray, jbyteArray povsArray,
   jlongArray buttonsAndMetadataArray)
{
  HAL_JoystickAxes axes;
  HAL_JoystickPOVs povs;
  HAL_JoystickButtons buttons;

  HAL_GetAllJoystickData(stick, &axes, &povs, &buttons);

  CriticalJSpan<jfloat> jAxes(env, axesArray);
  CriticalJSpan<jshort> jRawAxes(env, rawAxesArray);
  CriticalJSpan<jbyte> jPovs(env, povsArray);
  CriticalJSpan<jlong> jButtons(env, buttonsAndMetadataArray);

  static_assert(sizeof(jAxes[0]) == sizeof(axes.axes[0]));
  static_assert(sizeof(jRawAxes[0]) == sizeof(axes.raw[0]));
  static_assert(sizeof(jPovs[0]) == sizeof(povs.povs[0]));

  std::memcpy(&jAxes[0], axes.axes, sizeof(axes.axes));
  std::memcpy(&jRawAxes[0], axes.raw, sizeof(axes.raw));
  std::memcpy(&jPovs[0], povs.povs, sizeof(povs.povs));
  jButtons[0] = axes.available;
  jButtons[1] = povs.available;
  jButtons[2] = buttons.available;
  jButtons[3] = buttons.buttons;
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    setJoystickRumble
 * Signature: (BIIII)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_setJoystickRumble
  (JNIEnv*, jclass, jbyte port, jint leftRumble, jint rightRumble,
   jint leftTriggerRumble, jint rightTriggerRumble)
{
  return HAL_SetJoystickRumble(port, leftRumble, rightRumble, leftTriggerRumble,
                               rightTriggerRumble);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    setJoystickLeds
 * Signature: (BI)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_setJoystickLeds
  (JNIEnv*, jclass, jbyte port, jint leds)
{
  return HAL_SetJoystickLeds(port, leds);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    getJoystickIsGamepad
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_getJoystickIsGamepad
  (JNIEnv*, jclass, jbyte port)
{
  return HAL_GetJoystickIsGamepad(port);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    getJoystickSupportedOutputs
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_getJoystickSupportedOutputs
  (JNIEnv*, jclass, jbyte port)
{
  return HAL_GetJoystickSupportedOutputs(port);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    getJoystickGamepadType
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_getJoystickGamepadType
  (JNIEnv*, jclass, jbyte port)
{
  return HAL_GetJoystickGamepadType(port);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    getJoystickName
 * Signature: (B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_getJoystickName
  (JNIEnv* env, jclass, jbyte port)
{
  WPI_String joystickName;
  HAL_GetJoystickName(&joystickName, port);
  jstring str = MakeJString(env, wpi::util::to_string_view(&joystickName));
  WPI_FreeString(&joystickName);
  return str;
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    getMatchTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_getMatchTime
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  return HAL_GetMatchTime(&status);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    getMatchInfo
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_getMatchInfo
  (JNIEnv* env, jclass, jobject info)
{
  HAL_MatchInfo matchInfo;
  auto status = HAL_GetMatchInfo(&matchInfo);
  if (status == 0) {
    SetMatchInfoObject(env, info, matchInfo);
  }
  return status;
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    sendError
 * Signature: (ZIZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_sendError
  (JNIEnv* env, jclass, jboolean isError, jint errorCode, jboolean isLVCode,
   jstring details, jstring location, jstring callStack, jboolean printMsg)
{
  JStringRef detailsStr{env, details};
  JStringRef locationStr{env, location};
  JStringRef callStackStr{env, callStack};

  jint returnValue =
      HAL_SendError(isError, errorCode, isLVCode, detailsStr.c_str(),
                    locationStr.c_str(), callStackStr.c_str(), printMsg);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    sendConsoleLine
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_sendConsoleLine
  (JNIEnv* env, jclass, jstring line)
{
  JStringRef lineStr{env, line};

  jint returnValue = HAL_SendConsoleLine(lineStr.c_str());
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    refreshDSData
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_refreshDSData
  (JNIEnv*, jclass)
{
  return HAL_RefreshDSData();
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    provideNewDataEventHandle
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_provideNewDataEventHandle
  (JNIEnv*, jclass, jint handle)
{
  HAL_ProvideNewDataEventHandle(handle);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    removeNewDataEventHandle
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_removeNewDataEventHandle
  (JNIEnv*, jclass, jint handle)
{
  HAL_RemoveNewDataEventHandle(handle);
}

/*
 * Class:     org_wpilib_hardware_hal_DriverStationJNI
 * Method:    getOutputsActive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_DriverStationJNI_getOutputsActive
  (JNIEnv*, jclass)
{
  return HAL_GetOutputsEnabled();
}
}  // extern "C"
