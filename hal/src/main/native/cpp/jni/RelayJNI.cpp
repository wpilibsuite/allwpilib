/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_RelayJNI.h"
#include "hal/Ports.h"
#include "hal/Relay.h"
#include "hal/cpp/Log.h"
#include "hal/handles/HandlesInternal.h"

using namespace frc;

// set the logging level
TLogLevel relayJNILogLevel = logWARNING;

#define RELAYJNI_LOG(level)     \
  if (level > relayJNILogLevel) \
    ;                           \
  else                          \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_RelayJNI
 * Method:    initializeRelayPort
 * Signature: (IZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_RelayJNI_initializeRelayPort
  (JNIEnv* env, jclass, jint id, jboolean fwd)
{
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI initializeRelayPort";
  RELAYJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_PortHandle)id;
  RELAYJNI_LOG(logDEBUG) << "Forward = " << (jint)fwd;
  int32_t status = 0;
  HAL_RelayHandle handle = HAL_InitializeRelayPort(
      (HAL_PortHandle)id, static_cast<uint8_t>(fwd), &status);
  RELAYJNI_LOG(logDEBUG) << "Status = " << status;
  RELAYJNI_LOG(logDEBUG) << "Relay Handle = " << handle;
  CheckStatusRange(env, status, 0, HAL_GetNumRelayChannels(),
                   hal::getPortHandleChannel((HAL_PortHandle)id));
  return (jint)handle;
}

/*
 * Class:     edu_wpi_first_hal_RelayJNI
 * Method:    freeRelayPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_RelayJNI_freeRelayPort
  (JNIEnv* env, jclass, jint id)
{
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI freeRelayPort";
  RELAYJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_RelayHandle)id;
  HAL_FreeRelayPort((HAL_RelayHandle)id);
}

/*
 * Class:     edu_wpi_first_hal_RelayJNI
 * Method:    checkRelayChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_RelayJNI_checkRelayChannel
  (JNIEnv* env, jclass, jint channel)
{
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI checkRelayChannel";
  RELAYJNI_LOG(logDEBUG) << "Channel = " << channel;
  return (jboolean)HAL_CheckRelayChannel(static_cast<uint8_t>(channel));
}

/*
 * Class:     edu_wpi_first_hal_RelayJNI
 * Method:    setRelay
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_RelayJNI_setRelay
  (JNIEnv* env, jclass, jint id, jboolean value)
{
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI setRelay";
  RELAYJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_RelayHandle)id;
  RELAYJNI_LOG(logDEBUG) << "Flag = " << (jint)value;
  int32_t status = 0;
  HAL_SetRelay((HAL_RelayHandle)id, value, &status);
  RELAYJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_RelayJNI
 * Method:    getRelay
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_RelayJNI_getRelay
  (JNIEnv* env, jclass, jint id)
{
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI getRelay";
  RELAYJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_RelayHandle)id;
  int32_t status = 0;
  jboolean returnValue = HAL_GetRelay((HAL_RelayHandle)id, &status);
  RELAYJNI_LOG(logDEBUG) << "Status = " << status;
  RELAYJNI_LOG(logDEBUG) << "getRelayResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
