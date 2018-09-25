/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
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
#include "hal/cpp/Log.h"

using namespace frc;
using namespace wpi::java;

// set the logging level
static TLogLevel netCommLogLevel = logWARNING;

#define NETCOMM_LOG(level)     \
  if (level > netCommLogLevel) \
    ;                          \
  else                         \
    Log().Get(level)

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
  NETCOMM_LOG(logDEBUG) << "Calling HAL report "
                        << "res:" << paramResource
                        << " instance:" << paramInstanceNumber
                        << " context:" << paramContext
                        << " feature:" << featureStr.c_str();
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
  NETCOMM_LOG(logDEBUG) << "Calling HAL Control Word";
  static_assert(sizeof(HAL_ControlWord) == sizeof(jint),
                "Java int must match the size of control word");
  HAL_ControlWord controlWord;
  std::memset(&controlWord, 0, sizeof(HAL_ControlWord));
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
  NETCOMM_LOG(logDEBUG) << "Calling HAL Alliance Station";
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
  NETCOMM_LOG(logDEBUG) << "Calling HALJoystickAxes";
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
  NETCOMM_LOG(logDEBUG) << "Calling HALJoystickPOVs";
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
  NETCOMM_LOG(logDEBUG) << "Calling HALJoystickButtons";
  HAL_JoystickButtons joystickButtons;
  HAL_GetJoystickButtons(joystickNum, &joystickButtons);
  jbyte* countPtr =
      reinterpret_cast<jbyte*>(env->GetDirectBufferAddress(count));
  NETCOMM_LOG(logDEBUG) << "Buttons = " << joystickButtons.buttons;
  NETCOMM_LOG(logDEBUG) << "Count = " << (jint)joystickButtons.count;
  *countPtr = joystickButtons.count;
  NETCOMM_LOG(logDEBUG) << "CountBuffer = " << (jint)*countPtr;
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
  NETCOMM_LOG(logDEBUG) << "Calling HAL_SetJoystickOutputs on port " << port;
  NETCOMM_LOG(logDEBUG) << "Outputs: " << outputs;
  NETCOMM_LOG(logDEBUG) << "Left Rumble: " << leftRumble
                        << " Right Rumble: " << rightRumble;
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
  NETCOMM_LOG(logDEBUG) << "Calling HAL_GetJoystickIsXbox";
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
  NETCOMM_LOG(logDEBUG) << "Calling HAL_GetJoystickType";
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
  NETCOMM_LOG(logDEBUG) << "Calling HAL_GetJoystickName";
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
  NETCOMM_LOG(logDEBUG) << "Calling HAL_GetJoystickAxisType";
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

  NETCOMM_LOG(logDEBUG) << "Send Error: " << detailsStr.c_str();
  NETCOMM_LOG(logDEBUG) << "Location: " << locationStr.c_str();
  NETCOMM_LOG(logDEBUG) << "Call Stack: " << callStackStr.c_str();
  jint returnValue =
      HAL_SendError(isError, errorCode, isLVCode, detailsStr.c_str(),
                    locationStr.c_str(), callStackStr.c_str(), printMsg);
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
  // FILE_LOG(logDEBUG) << "Calling HAL getPortWithModlue";
  // FILE_LOG(logDEBUG) << "Module = " << (jint)module;
  // FILE_LOG(logDEBUG) << "Channel = " << (jint)channel;
  HAL_PortHandle port = HAL_GetPortWithModule(module, channel);
  // FILE_LOG(logDEBUG) << "Port Handle = " << port;
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
  // FILE_LOG(logDEBUG) << "Calling HAL getPortWithModlue";
  // FILE_LOG(logDEBUG) << "Module = " << (jint)module;
  // FILE_LOG(logDEBUG) << "Channel = " << (jint)channel;
  HAL_PortHandle port = HAL_GetPort(channel);
  // FILE_LOG(logDEBUG) << "Port Handle = " << port;
  return (jint)port;
}

}  // extern "C"
