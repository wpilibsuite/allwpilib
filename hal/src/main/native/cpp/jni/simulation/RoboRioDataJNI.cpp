// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "wpi/util/jni_util.hpp"

#include "CallbackStore.h"
#include "org_wpilib_hardware_hal_simulation_RoboRioDataJNI.h"
#include "wpi/hal/simulation/RoboRioData.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    registerVInVoltageCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_registerVInVoltageCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterRoboRioVInVoltageCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    cancelVInVoltageCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_cancelVInVoltageCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioVInVoltageCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getVInVoltage
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getVInVoltage
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioVInVoltage();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setVInVoltage
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setVInVoltage
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioVInVoltage(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    registerUserVoltage3V3Callback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_registerUserVoltage3V3Callback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserVoltage3V3Callback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserVoltage3V3Callback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_cancelUserVoltage3V3Callback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserVoltage3V3Callback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getUserVoltage3V3
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getUserVoltage3V3
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserVoltage3V3();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setUserVoltage3V3
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setUserVoltage3V3
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioUserVoltage3V3(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    registerUserCurrent3V3Callback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_registerUserCurrent3V3Callback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserCurrent3V3Callback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserCurrent3V3Callback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_cancelUserCurrent3V3Callback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserCurrent3V3Callback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getUserCurrent3V3
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getUserCurrent3V3
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserCurrent3V3();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setUserCurrent3V3
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setUserCurrent3V3
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioUserCurrent3V3(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    registerUserActive3V3Callback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_registerUserActive3V3Callback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserActive3V3Callback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserActive3V3Callback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_cancelUserActive3V3Callback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserActive3V3Callback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getUserActive3V3
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getUserActive3V3
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserActive3V3();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setUserActive3V3
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setUserActive3V3
  (JNIEnv*, jclass, jboolean value)
{
  HALSIM_SetRoboRioUserActive3V3(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    registerUserFaults3V3Callback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_registerUserFaults3V3Callback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioUserFaults3V3Callback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    cancelUserFaults3V3Callback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_cancelUserFaults3V3Callback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioUserFaults3V3Callback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getUserFaults3V3
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getUserFaults3V3
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioUserFaults3V3();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setUserFaults3V3
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setUserFaults3V3
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetRoboRioUserFaults3V3(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    registerBrownoutVoltageCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_registerBrownoutVoltageCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify,
      &HALSIM_RegisterRoboRioBrownoutVoltageCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    cancelBrownoutVoltageCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_cancelBrownoutVoltageCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioBrownoutVoltageCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getBrownoutVoltage
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getBrownoutVoltage
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioBrownoutVoltage();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setBrownoutVoltage
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setBrownoutVoltage
  (JNIEnv*, jclass, jdouble value)
{
  HALSIM_SetRoboRioBrownoutVoltage(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    registerCPUTempCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_registerCPUTempCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(env, callback, initialNotify,
                                      &HALSIM_RegisterRoboRioCPUTempCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    cancelCPUTempCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_cancelCPUTempCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioCPUTempCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getCPUTemp
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getCPUTemp
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioCPUTemp();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setCPUTemp
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setCPUTemp
  (JNIEnv*, jclass, jdouble cpuTemp)
{
  HALSIM_SetRoboRioCPUTemp(cpuTemp);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    registerTeamNumberCallback
 * Signature: (Ljava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_registerTeamNumberCallback
  (JNIEnv* env, jclass, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallbackNoIndex(
      env, callback, initialNotify, &HALSIM_RegisterRoboRioTeamNumberCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    cancelTeamNumberCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_cancelTeamNumberCallback
  (JNIEnv* env, jclass, jint handle)
{
  return sim::FreeCallbackNoIndex(env, handle,
                                  &HALSIM_CancelRoboRioTeamNumberCallback);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getTeamNumber
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getTeamNumber
  (JNIEnv*, jclass)
{
  return HALSIM_GetRoboRioTeamNumber();
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setTeamNumber
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setTeamNumber
  (JNIEnv*, jclass, jint value)
{
  HALSIM_SetRoboRioTeamNumber(value);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getSerialNumber
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getSerialNumber
  (JNIEnv* env, jclass)
{
  WPI_String str;
  HALSIM_GetRoboRioSerialNumber(&str);
  auto jstr = MakeJString(env, wpi::to_string_view(&str));
  WPI_FreeString(&str);
  return jstr;
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setSerialNumber
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setSerialNumber
  (JNIEnv* env, jclass, jstring serialNumber)
{
  JStringRef serialNumberJString{env, serialNumber};
  auto str = wpi::make_string(serialNumberJString);
  HALSIM_SetRoboRioSerialNumber(&str);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    getComments
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_getComments
  (JNIEnv* env, jclass)
{
  WPI_String str;
  HALSIM_GetRoboRioComments(&str);
  auto jstr = MakeJString(env, wpi::to_string_view(&str));
  WPI_FreeString(&str);
  return jstr;
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    setComments
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_setComments
  (JNIEnv* env, jclass, jstring comments)
{
  JStringRef commentsJString{env, comments};
  auto str = wpi::make_string(commentsJString);
  HALSIM_SetRoboRioComments(&str);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_RoboRioDataJNI
 * Method:    resetData
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_RoboRioDataJNI_resetData
  (JNIEnv*, jclass)
{
  HALSIM_ResetRoboRioData();
}

}  // extern "C"
