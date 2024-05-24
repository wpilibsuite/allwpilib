// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <fmt/format.h>
#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_DriverStationJNI.h"
#include "hal/DriverStation.h"
#include "hal/FRCUsageReporting.h"
#include "hal/HALBase.h"

static_assert(edu_wpi_first_hal_DriverStationJNI_kUnknownAllianceStation ==
              HAL_AllianceStationID_kUnknown);
static_assert(edu_wpi_first_hal_DriverStationJNI_kRed1AllianceStation ==
              HAL_AllianceStationID_kRed1);
static_assert(edu_wpi_first_hal_DriverStationJNI_kRed2AllianceStation ==
              HAL_AllianceStationID_kRed2);
static_assert(edu_wpi_first_hal_DriverStationJNI_kRed3AllianceStation ==
              HAL_AllianceStationID_kRed3);
static_assert(edu_wpi_first_hal_DriverStationJNI_kBlue1AllianceStation ==
              HAL_AllianceStationID_kBlue1);
static_assert(edu_wpi_first_hal_DriverStationJNI_kBlue2AllianceStation ==
              HAL_AllianceStationID_kBlue2);
static_assert(edu_wpi_first_hal_DriverStationJNI_kBlue3AllianceStation ==
              HAL_AllianceStationID_kBlue3);

static_assert(edu_wpi_first_hal_DriverStationJNI_kMaxJoystickAxes ==
              HAL_kMaxJoystickAxes);
static_assert(edu_wpi_first_hal_DriverStationJNI_kMaxJoystickPOVs ==
              HAL_kMaxJoystickPOVs);
static_assert(edu_wpi_first_hal_DriverStationJNI_kMaxJoysticks ==
              HAL_kMaxJoysticks);

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    observeUserProgramStarting
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_observeUserProgramStarting
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramStarting();
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    observeUserProgramDisabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_observeUserProgramDisabled
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramDisabled();
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    observeUserProgramAutonomous
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_observeUserProgramAutonomous
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramAutonomous();
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    observeUserProgramTeleop
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_observeUserProgramTeleop
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramTeleop();
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    observeUserProgramTest
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_observeUserProgramTest
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramTest();
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    report
 * Signature: (IIILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_report
  (JNIEnv* paramEnv, jclass, jint paramResource, jint paramInstanceNumber,
   jint paramContext, jstring paramFeature)
{
  JStringRef featureStr{paramEnv, paramFeature};
  jint returnValue = HAL_Report(paramResource, paramInstanceNumber,
                                paramContext, featureStr.c_str());
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    nativeGetControlWord
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_nativeGetControlWord
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
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    nativeGetAllianceStation
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_nativeGetAllianceStation
  (JNIEnv*, jclass)
{
  int32_t status = 0;
  auto allianceStation = HAL_GetAllianceStation(&status);
  return static_cast<jint>(allianceStation);
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getJoystickAxesRaw
 * Signature: (B[I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getJoystickAxesRaw
  (JNIEnv* env, jclass, jbyte joystickNum, jintArray axesRawArray)
{
  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(joystickNum, &axes);

  jsize javaSize = env->GetArrayLength(axesRawArray);
  if (axes.count > javaSize) {
    ThrowIllegalArgumentException(
        env,
        fmt::format("Native array size larger then passed in java array "
                    "size\nNative Size: {} Java Size: {}",
                    static_cast<int>(axes.count), static_cast<int>(javaSize)));
    return 0;
  }

  jint raw[HAL_kMaxJoystickAxes];
  for (int16_t i = 0; i < axes.count; i++) {
    raw[i] = axes.raw[i];
  }
  env->SetIntArrayRegion(axesRawArray, 0, axes.count, raw);

  return axes.count;
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getJoystickAxes
 * Signature: (B[F)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getJoystickAxes
  (JNIEnv* env, jclass, jbyte joystickNum, jfloatArray axesArray)
{
  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(joystickNum, &axes);

  jsize javaSize = env->GetArrayLength(axesArray);
  if (axes.count > javaSize) {
    ThrowIllegalArgumentException(
        env,
        fmt::format("Native array size larger then passed in java array "
                    "size\nNative Size: {} Java Size: {}",
                    static_cast<int>(axes.count), static_cast<int>(javaSize)));
    return 0;
  }

  env->SetFloatArrayRegion(axesArray, 0, axes.count, axes.axes);

  return axes.count;
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getJoystickPOVs
 * Signature: (B[S)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getJoystickPOVs
  (JNIEnv* env, jclass, jbyte joystickNum, jshortArray povsArray)
{
  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(joystickNum, &povs);

  jsize javaSize = env->GetArrayLength(povsArray);
  if (povs.count > javaSize) {
    ThrowIllegalArgumentException(
        env,
        fmt::format("Native array size larger then passed in java array "
                    "size\nNative Size: {} Java Size: {}",
                    static_cast<int>(povs.count), static_cast<int>(javaSize)));
    return 0;
  }

  env->SetShortArrayRegion(povsArray, 0, povs.count, povs.povs);

  return povs.count;
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getAllJoystickData
 * Signature: ([F[B[S[J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getAllJoystickData
  (JNIEnv* env, jclass cls, jfloatArray axesArray, jbyteArray rawAxesArray,
   jshortArray povsArray, jlongArray buttonsAndMetadataArray)
{
  HAL_JoystickAxes axes[HAL_kMaxJoysticks];
  HAL_JoystickPOVs povs[HAL_kMaxJoysticks];
  HAL_JoystickButtons buttons[HAL_kMaxJoysticks];

  HAL_GetAllJoystickData(axes, povs, buttons);

  CriticalJSpan<jfloat> jAxes(env, axesArray);
  CriticalJSpan<jbyte> jRawAxes(env, rawAxesArray);
  CriticalJSpan<jshort> jPovs(env, povsArray);
  CriticalJSpan<jlong> jButtons(env, buttonsAndMetadataArray);

  static_assert(sizeof(jAxes[0]) == sizeof(axes[0].axes[0]));
  static_assert(sizeof(jRawAxes[0]) == sizeof(axes[0].raw[0]));
  static_assert(sizeof(jPovs[0]) == sizeof(povs[0].povs[0]));

  for (size_t i = 0; i < HAL_kMaxJoysticks; i++) {
    std::memcpy(&jAxes[i * HAL_kMaxJoystickAxes], axes[i].axes,
                sizeof(axes[i].axes));
    std::memcpy(&jRawAxes[i * HAL_kMaxJoystickAxes], axes[i].raw,
                sizeof(axes[i].raw));
    std::memcpy(&jPovs[i * HAL_kMaxJoystickPOVs], povs[i].povs,
                sizeof(povs[i].povs));
    jButtons[i * 4] = axes[i].count;
    jButtons[(i * 4) + 1] = povs[i].count;
    jButtons[(i * 4) + 2] = buttons[i].count;
    jButtons[(i * 4) + 3] = buttons[i].buttons;
  }
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getJoystickButtons
 * Signature: (BLjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getJoystickButtons
  (JNIEnv* env, jclass, jbyte joystickNum, jobject count)
{
  HAL_JoystickButtons joystickButtons;
  HAL_GetJoystickButtons(joystickNum, &joystickButtons);
  jbyte* countPtr =
      reinterpret_cast<jbyte*>(env->GetDirectBufferAddress(count));
  *countPtr = joystickButtons.count;
  return joystickButtons.buttons;
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    setJoystickOutputs
 * Signature: (BIII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_setJoystickOutputs
  (JNIEnv*, jclass, jbyte port, jint outputs, jint leftRumble, jint rightRumble)
{
  return HAL_SetJoystickOutputs(port, outputs, leftRumble, rightRumble);
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getJoystickIsXbox
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getJoystickIsXbox
  (JNIEnv*, jclass, jbyte port)
{
  return HAL_GetJoystickIsXbox(port);
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getJoystickType
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getJoystickType
  (JNIEnv*, jclass, jbyte port)
{
  return HAL_GetJoystickType(port);
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getJoystickName
 * Signature: (B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getJoystickName
  (JNIEnv* env, jclass, jbyte port)
{
  WPI_String joystickName;
  HAL_GetJoystickName(&joystickName, port);
  jstring str = MakeJString(env, wpi::to_string_view(&joystickName));
  WPI_FreeString(&joystickName);
  return str;
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getJoystickAxisType
 * Signature: (BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getJoystickAxisType
  (JNIEnv*, jclass, jbyte joystickNum, jbyte axis)
{
  return HAL_GetJoystickAxisType(joystickNum, axis);
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getMatchTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getMatchTime
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  return HAL_GetMatchTime(&status);
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getMatchInfo
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getMatchInfo
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
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    sendError
 * Signature: (ZIZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_sendError
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
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    sendConsoleLine
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_sendConsoleLine
  (JNIEnv* env, jclass, jstring line)
{
  JStringRef lineStr{env, line};

  jint returnValue = HAL_SendConsoleLine(lineStr.c_str());
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    refreshDSData
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_refreshDSData
  (JNIEnv*, jclass)
{
  return HAL_RefreshDSData();
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    provideNewDataEventHandle
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_provideNewDataEventHandle
  (JNIEnv*, jclass, jint handle)
{
  HAL_ProvideNewDataEventHandle(handle);
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    removeNewDataEventHandle
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_removeNewDataEventHandle
  (JNIEnv*, jclass, jint handle)
{
  HAL_RemoveNewDataEventHandle(handle);
}

/*
 * Class:     edu_wpi_first_hal_DriverStationJNI
 * Method:    getOutputsActive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_DriverStationJNI_getOutputsActive
  (JNIEnv*, jclass)
{
  return HAL_GetOutputsEnabled();
}
}  // extern "C"
