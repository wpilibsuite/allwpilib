/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_PortsJNI.h"
#include "hal/Ports.h"
#include "hal/cpp/Log.h"

using namespace frc;

// set the logging level
TLogLevel portsJNILogLevel = logWARNING;

#define PORTSJNI_LOG(level)     \
  if (level > portsJNILogLevel) \
    ;                           \
  else                          \
    Log().Get(level)

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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumAccumulators";
  jint value = HAL_GetNumAccumulators();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumAnalogTriggers";
  jint value = HAL_GetNumAnalogTriggers();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumAnalogInputs";
  jint value = HAL_GetNumAnalogInputs();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumAnalogOutputs";
  jint value = HAL_GetNumAnalogOutputs();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumCounters";
  jint value = HAL_GetNumCounters();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumDigitalHeaders";
  jint value = HAL_GetNumDigitalHeaders();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPWMHeaders";
  jint value = HAL_GetNumPWMHeaders();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumDigitalChannels";
  jint value = HAL_GetNumDigitalChannels();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPWMChannels";
  jint value = HAL_GetNumPWMChannels();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumDigitalPWMOutputs";
  jint value = HAL_GetNumDigitalPWMOutputs();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumEncoders";
  jint value = HAL_GetNumEncoders();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumInterrupts";
  jint value = HAL_GetNumInterrupts();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumRelayChannels";
  jint value = HAL_GetNumRelayChannels();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumRelayHeaders";
  jint value = HAL_GetNumRelayHeaders();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPCMModules";
  jint value = HAL_GetNumPCMModules();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumSolenoidChannels";
  jint value = HAL_GetNumSolenoidChannels();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPDPModules";
  jint value = HAL_GetNumPDPModules();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
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
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPDPChannels";
  jint value = HAL_GetNumPDPChannels();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}
}  // extern "C"
