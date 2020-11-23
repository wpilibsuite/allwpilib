/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/HAL.h"

#include <jni.h>

#include <cassert>
#include <cstring>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_HAL.h"
#include "hal/DriverStation.h"
#include "hal/Main.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    initialize
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_initialize
  (JNIEnv*, jclass, jint timeout, jint mode)
{
  return HAL_Initialize(timeout, mode);
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    shutdown
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_shutdown
  (JNIEnv*, jclass)
{
  return HAL_Shutdown();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    hasMain
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_hasMain
  (JNIEnv*, jclass)
{
  return HAL_HasMain();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    runMain
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_runMain
  (JNIEnv*, jclass)
{
  HAL_RunMain();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    exitMain
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_exitMain
  (JNIEnv*, jclass)
{
  HAL_ExitMain();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    simPeriodicBeforeNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_simPeriodicBeforeNative
  (JNIEnv*, jclass)
{
  HAL_SimPeriodicBefore();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    simPeriodicAfterNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_simPeriodicAfterNative
  (JNIEnv*, jclass)
{
  HAL_SimPeriodicAfter();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    observeUserProgramStarting
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_observeUserProgramStarting
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramStarting();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    observeUserProgramDisabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_observeUserProgramDisabled
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramDisabled();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    observeUserProgramAutonomous
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_observeUserProgramAutonomous
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramAutonomous();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    observeUserProgramTeleop
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_observeUserProgramTeleop
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramTeleop();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    observeUserProgramTest
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_observeUserProgramTest
  (JNIEnv*, jclass)
{
  HAL_ObserveUserProgramTest();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    report
 * Signature: (IIILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_report
  (JNIEnv* paramEnv, jclass, jint paramResource, jint paramInstanceNumber,
   jint paramContext, jstring paramFeature)
{
  JStringRef featureStr{paramEnv, paramFeature};
  jint returnValue = HAL_Report(paramResource, paramInstanceNumber,
                                paramContext, featureStr.c_str());
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    nativeGetControlWord
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_nativeGetControlWord
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
 * Class:     edu_wpi_first_hal_HAL
 * Method:    nativeGetAllianceStation
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_nativeGetAllianceStation
  (JNIEnv*, jclass)
{
  int32_t status = 0;
  auto allianceStation = HAL_GetAllianceStation(&status);
  return static_cast<jint>(allianceStation);
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getJoystickAxes
 * Signature: (B[F)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_hal_HAL_getJoystickAxes
  (JNIEnv* env, jclass, jbyte joystickNum, jfloatArray axesArray)
{
  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(joystickNum, &axes);

  jsize javaSize = env->GetArrayLength(axesArray);
  if (axes.count > javaSize) {
    wpi::SmallString<128> errStr;
    wpi::raw_svector_ostream oss{errStr};
    oss << "Native array size larger then passed in java array size "
        << "Native Size: " << static_cast<int>(axes.count)
        << " Java Size: " << static_cast<int>(javaSize);

    ThrowIllegalArgumentException(env, errStr.str());
    return 0;
  }

  env->SetFloatArrayRegion(axesArray, 0, axes.count, axes.axes);

  return axes.count;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getJoystickPOVs
 * Signature: (B[S)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_hal_HAL_getJoystickPOVs
  (JNIEnv* env, jclass, jbyte joystickNum, jshortArray povsArray)
{
  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(joystickNum, &povs);

  jsize javaSize = env->GetArrayLength(povsArray);
  if (povs.count > javaSize) {
    wpi::SmallString<128> errStr;
    wpi::raw_svector_ostream oss{errStr};
    oss << "Native array size larger then passed in java array size "
        << "Native Size: " << static_cast<int>(povs.count)
        << " Java Size: " << static_cast<int>(javaSize);

    ThrowIllegalArgumentException(env, errStr.str());
    return 0;
  }

  env->SetShortArrayRegion(povsArray, 0, povs.count, povs.povs);

  return povs.count;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getJoystickButtons
 * Signature: (BLjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getJoystickButtons
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
 * Class:     edu_wpi_first_hal_HAL
 * Method:    setJoystickOutputs
 * Signature: (BISS)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_setJoystickOutputs
  (JNIEnv*, jclass, jbyte port, jint outputs, jshort leftRumble,
   jshort rightRumble)
{
  return HAL_SetJoystickOutputs(port, outputs, leftRumble, rightRumble);
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getJoystickIsXbox
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getJoystickIsXbox
  (JNIEnv*, jclass, jbyte port)
{
  return HAL_GetJoystickIsXbox(port);
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getJoystickType
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getJoystickType
  (JNIEnv*, jclass, jbyte port)
{
  return HAL_GetJoystickType(port);
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getJoystickName
 * Signature: (B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_HAL_getJoystickName
  (JNIEnv* env, jclass, jbyte port)
{
  char* joystickName = HAL_GetJoystickName(port);
  jstring str = MakeJString(env, joystickName);
  HAL_FreeJoystickName(joystickName);
  return str;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getJoystickAxisType
 * Signature: (BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getJoystickAxisType
  (JNIEnv*, jclass, jbyte joystickNum, jbyte axis)
{
  return HAL_GetJoystickAxisType(joystickNum, axis);
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    isNewControlData
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_isNewControlData
  (JNIEnv*, jclass)
{
  return static_cast<jboolean>(HAL_IsNewControlData());
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    waitForDSData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_waitForDSData
  (JNIEnv* env, jclass)
{
  HAL_WaitForDSData();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    releaseDSMutex
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_releaseDSMutex
  (JNIEnv* env, jclass)
{
  HAL_ReleaseDSMutex();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    waitForDSDataTimeout
 * Signature: (D)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_waitForDSDataTimeout
  (JNIEnv*, jclass, jdouble timeout)
{
  return static_cast<jboolean>(HAL_WaitForDSDataTimeout(timeout));
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getMatchTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_HAL_getMatchTime
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  return HAL_GetMatchTime(&status);
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getSystemActive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_getSystemActive
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetSystemActive(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getBrownedOut
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_getBrownedOut
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetBrownedOut(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getMatchInfo
 * Signature: (Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getMatchInfo
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
 * Class:     edu_wpi_first_hal_HAL
 * Method:    sendError
 * Signature: (ZIZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_sendError
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
 * Class:     edu_wpi_first_hal_HAL
 * Method:    sendConsoleLine
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_sendConsoleLine
  (JNIEnv* env, jclass, jstring line)
{
  JStringRef lineStr{env, line};

  jint returnValue = HAL_SendConsoleLine(lineStr.c_str());
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getPortWithModule
 * Signature: (BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getPortWithModule
  (JNIEnv* env, jclass, jbyte module, jbyte channel)
{
  HAL_PortHandle port = HAL_GetPortWithModule(module, channel);
  return (jint)port;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getPort
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getPort
  (JNIEnv* env, jclass, jbyte channel)
{
  HAL_PortHandle port = HAL_GetPort(channel);
  return (jint)port;
}

}  // extern "C"
