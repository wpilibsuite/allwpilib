// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_PortsJNI.h"
#include "hal/Ports.h"

using namespace hal;

extern "C" {
/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumAccumulators
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumAccumulators
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumAccumulators();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumAnalogTriggers
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumAnalogTriggers
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumAnalogTriggers();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumAnalogInputs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumAnalogInputs
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumAnalogInputs();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumAnalogOutputs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumAnalogOutputs
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumAnalogOutputs();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumCounters
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumCounters
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCounters();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumDigitalHeaders
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumDigitalHeaders
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumDigitalHeaders();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumPWMHeaders
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumPWMHeaders
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumPWMHeaders();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumDigitalChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumDigitalChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumDigitalChannels();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumPWMChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumPWMChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumPWMChannels();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumDigitalPWMOutputs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumDigitalPWMOutputs
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumDigitalPWMOutputs();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumEncoders
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumEncoders
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumEncoders();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumInterrupts
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumInterrupts
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumInterrupts();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumRelayChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumRelayChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumRelayChannels();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumRelayHeaders
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumRelayHeaders
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumRelayHeaders();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumCTREPCMModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumCTREPCMModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCTREPCMModules();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumCTRESolenoidChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumCTRESolenoidChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCTRESolenoidChannels();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumCTREPDPModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumCTREPDPModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCTREPDPModules();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumCTREPDPChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumCTREPDPChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCTREPDPChannels();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumREVPDHModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumREVPDHModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumREVPDHModules();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumREVPDHChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumREVPDHChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumREVPDHChannels();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumREVPHModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumREVPHModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumREVPHModules();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumREVPHChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumREVPHChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumREVPHChannels();
  return value;
}
}  // extern "C"
