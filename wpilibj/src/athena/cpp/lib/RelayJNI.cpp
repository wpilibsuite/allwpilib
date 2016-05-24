/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_RelayJNI.h"

#include "HAL/Relay.h"
#include "HALUtil.h"

// set the logging level
TLogLevel relayJNILogLevel = logWARNING;

#define RELAYJNI_LOG(level)     \
  if (level > relayJNILogLevel) \
    ;                           \
  else                          \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_RelayJNI
 * Method:    setRelayForward
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_RelayJNI_setRelayForward(
    JNIEnv* env, jclass, jlong id, jboolean value) {
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI setRelayForward";
  RELAYJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  RELAYJNI_LOG(logDEBUG) << "Flag = " << (jint)value;
  int32_t status = 0;
  setRelayForward((void*)id, value, &status);
  RELAYJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_RelayJNI
 * Method:    setRelayReverse
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_RelayJNI_setRelayReverse(
    JNIEnv* env, jclass, jlong id, jboolean value) {
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI setRelayReverse";
  RELAYJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  RELAYJNI_LOG(logDEBUG) << "Flag = " << (jint)value;
  int32_t status = 0;
  setRelayReverse((void*)id, value, &status);
  RELAYJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_RelayJNI
 * Method:    getRelayForward
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_RelayJNI_getRelayForward(
    JNIEnv* env, jclass, jlong id) {
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI getRelayForward";
  RELAYJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = getRelayForward((void*)id, &status);
  RELAYJNI_LOG(logDEBUG) << "Status = " << status;
  RELAYJNI_LOG(logDEBUG) << "getRelayForwardResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_RelayJNI
 * Method:    getRelayReverse
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_RelayJNI_getRelayReverse(
    JNIEnv* env, jclass, jlong id) {
  RELAYJNI_LOG(logDEBUG) << "Calling RELAYJNI getRelayReverse";
  RELAYJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = getRelayReverse((void*)id, &status);
  RELAYJNI_LOG(logDEBUG) << "Status = " << status;
  RELAYJNI_LOG(logDEBUG) << "getRelayReverseResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
