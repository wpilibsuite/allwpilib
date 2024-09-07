// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_RelayJNI.h"
#include "hal/Ports.h"
#include "hal/Relay.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

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
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  HAL_RelayHandle handle = HAL_InitializeRelayPort(
      (HAL_PortHandle)id, static_cast<uint8_t>(fwd), stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
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
  if (id != HAL_kInvalidHandle) {
    HAL_FreeRelayPort((HAL_RelayHandle)id);
  }
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
  int32_t status = 0;
  HAL_SetRelay((HAL_RelayHandle)id, value, &status);
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
  int32_t status = 0;
  jboolean returnValue = HAL_GetRelay((HAL_RelayHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
