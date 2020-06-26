/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
 * Method:    getNumPCMModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumPCMModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumPCMModules();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumSolenoidChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumSolenoidChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumSolenoidChannels();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumPDPModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumPDPModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumPDPModules();
  return value;
}

/*
 * Class:     edu_wpi_first_hal_PortsJNI
 * Method:    getNumPDPChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PortsJNI_getNumPDPChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumPDPChannels();
  return value;
}
}  // extern "C"
