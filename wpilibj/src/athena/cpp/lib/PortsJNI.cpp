/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_PortsJNI.h"

#include "HAL/Ports.h"
#include "HALUtil.h"

// set the logging level
TLogLevel portsJNILogLevel = logWARNING;

#define PORTSJNI_LOG(level)     \
  if (level > portsJNILogLevel) \
    ;                         \
  else                        \
  Log().Get(level)

extern "C" {
/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumAccumulators
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumAccumulators(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumAccumulators";
  jint value = HAL_GetNumAccumulators();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumAnalogTriggers
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumAnalogTriggers(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumAnalogTriggers";
  jint value = HAL_GetNumAnalogTriggers();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumAnalogInputs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumAnalogInputs(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumAnalogInputs";
  jint value = HAL_GetNumAnalogInputs();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumAnalogOutputs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumAnalogOutputs(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumAnalogOutputs";
  jint value = HAL_GetNumAnalogOutputs();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumCounters
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumCounters(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumCounters";
  jint value = HAL_GetNumCounters();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumDigitalHeaders
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumDigitalHeaders(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumDigitalHeaders";
  jint value = HAL_GetNumDigitalHeaders();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumPWMHeaders
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumPWMHeaders(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPWMHeaders";
  jint value = HAL_GetNumPWMHeaders();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumDigitalPins
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumDigitalPins(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumDigitalPins";
  jint value = HAL_GetNumDigitalPins();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumPWMPins
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumPWMPins(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPWMPins";
  jint value = HAL_GetNumPWMPins();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumDigitalPWMOutputs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumDigitalPWMOutputs(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumDigitalPWMOutputs";
  jint value = HAL_GetNumDigitalPWMOutputs();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumEncoders
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumEncoders(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumEncoders";
  jint value = HAL_GetNumEncoders();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumInterrupts
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumInterrupts(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumInterrupts";
  jint value = HAL_GetNumInterrupts();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumRelayPins
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumRelayPins(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumRelayPins";
  jint value = HAL_GetNumRelayPins();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumRelayHeaders
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumRelayHeaders(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumRelayHeaders";
  jint value = HAL_GetNumRelayHeaders();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumPCMModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumPCMModules(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPCMModules";
  jint value = HAL_GetNumPCMModules();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumSolenoidPins
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumSolenoidPins(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumSolenoidPins";
  jint value = HAL_GetNumSolenoidPins();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumPDPModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumPDPModules(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPDPModules";
  jint value = HAL_GetNumPDPModules();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumPDPChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumPDPChannels(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumPDPChannels";
  jint value = HAL_GetNumPDPChannels();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PortsJNI
 * Method:    getNumCanTalons
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PortsJNI_getNumCanTalons(
    JNIEnv *env, jclass) {
  PORTSJNI_LOG(logDEBUG) << "Calling PortsJNI getNumCanTalons";
  jint value = HAL_GetNumCanTalons();
  PORTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}
}
