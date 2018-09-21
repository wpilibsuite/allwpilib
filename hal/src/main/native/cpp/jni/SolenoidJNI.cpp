/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_SolenoidJNI.h"
#include "hal/Ports.h"
#include "hal/Solenoid.h"
#include "hal/cpp/Log.h"
#include "hal/handles/HandlesInternal.h"

using namespace frc;

TLogLevel solenoidJNILogLevel = logERROR;

#define SOLENOIDJNI_LOG(level)     \
  if (level > solenoidJNILogLevel) \
    ;                              \
  else                             \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    initializeSolenoidPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_initializeSolenoidPort
  (JNIEnv* env, jclass, jint id)
{
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI initializeSolenoidPort";

  SOLENOIDJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_PortHandle)id;

  int32_t status = 0;
  HAL_SolenoidHandle handle =
      HAL_InitializeSolenoidPort((HAL_PortHandle)id, &status);

  SOLENOIDJNI_LOG(logDEBUG) << "Status = " << status;
  SOLENOIDJNI_LOG(logDEBUG) << "Solenoid Port Handle = " << handle;

  // Use solenoid channels, as we have to pick one.
  CheckStatusRange(env, status, 0, HAL_GetNumSolenoidChannels(),
                   hal::getPortHandleChannel((HAL_PortHandle)id));
  return (jint)handle;
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    checkSolenoidChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_checkSolenoidChannel
  (JNIEnv* env, jclass, jint channel)
{
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI checkSolenoidChannel";
  SOLENOIDJNI_LOG(logDEBUG) << "Channel = " << channel;
  return HAL_CheckSolenoidChannel(channel);
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    checkSolenoidModule
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_checkSolenoidModule
  (JNIEnv* env, jclass, jint module)
{
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI checkSolenoidModule";
  SOLENOIDJNI_LOG(logDEBUG) << "Module = " << module;
  return HAL_CheckSolenoidModule(module);
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    freeSolenoidPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_freeSolenoidPort
  (JNIEnv* env, jclass, jint id)
{
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI initializeSolenoidPort";

  SOLENOIDJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_SolenoidHandle)id;
  HAL_FreeSolenoidPort((HAL_SolenoidHandle)id);
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    setSolenoid
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_setSolenoid
  (JNIEnv* env, jclass, jint solenoid_port, jboolean value)
{
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI SetSolenoid";

  SOLENOIDJNI_LOG(logDEBUG)
      << "Solenoid Port Handle = " << (HAL_SolenoidHandle)solenoid_port;

  int32_t status = 0;
  HAL_SetSolenoid((HAL_SolenoidHandle)solenoid_port, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    getSolenoid
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_getSolenoid
  (JNIEnv* env, jclass, jint solenoid_port)
{
  int32_t status = 0;
  jboolean val = HAL_GetSolenoid((HAL_SolenoidHandle)solenoid_port, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    getAllSolenoids
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_getAllSolenoids
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  jint val = HAL_GetAllSolenoids(module, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    getPCMSolenoidBlackList
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_getPCMSolenoidBlackList
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  jint val = HAL_GetPCMSolenoidBlackList(module, &status);
  CheckStatus(env, status);
  return val;
}
/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    getPCMSolenoidVoltageStickyFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_getPCMSolenoidVoltageStickyFault
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  bool val = HAL_GetPCMSolenoidVoltageStickyFault(module, &status);
  CheckStatus(env, status);
  return val;
}
/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    getPCMSolenoidVoltageFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_getPCMSolenoidVoltageFault
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  bool val = HAL_GetPCMSolenoidVoltageFault(module, &status);
  CheckStatus(env, status);
  return val;
}
/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    clearAllPCMStickyFaults
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_clearAllPCMStickyFaults
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  HAL_ClearAllPCMStickyFaults(module, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    setOneShotDuration
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_setOneShotDuration
  (JNIEnv* env, jclass, jint solenoid_port, jlong durationMS)
{
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI SetOneShotDuration";

  SOLENOIDJNI_LOG(logDEBUG)
      << "Solenoid Port Handle = " << (HAL_SolenoidHandle)solenoid_port;
  SOLENOIDJNI_LOG(logDEBUG) << "Duration (MS) = " << durationMS;

  int32_t status = 0;
  HAL_SetOneShotDuration((HAL_SolenoidHandle)solenoid_port, durationMS,
                         &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_SolenoidJNI
 * Method:    fireOneShot
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SolenoidJNI_fireOneShot
  (JNIEnv* env, jclass, jint solenoid_port)
{
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI fireOneShot";

  SOLENOIDJNI_LOG(logDEBUG)
      << "Solenoid Port Handle = " << (HAL_SolenoidHandle)solenoid_port;

  int32_t status = 0;
  HAL_FireOneShot((HAL_SolenoidHandle)solenoid_port, &status);
  CheckStatus(env, status);
}
}  // extern "C"
