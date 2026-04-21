// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.hpp"
#include "org_wpilib_hardware_hal_PortsJNI.h"
#include "wpi/hal/Ports.h"

using namespace wpi::hal;

extern "C" {
/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumCanBuses
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumCanBuses
  (JNIEnv*, jclass)
{
  jint value = HAL_GetNumCanBuses();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumSmartIo
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumSmartIo
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumSmartIo();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumCTREPCMModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumCTREPCMModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCTREPCMModules();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumCTRESolenoidChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumCTRESolenoidChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCTRESolenoidChannels();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumCTREPDPModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumCTREPDPModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCTREPDPModules();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumCTREPDPChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumCTREPDPChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumCTREPDPChannels();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumREVPDHModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumREVPDHModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumREVPDHModules();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumREVPDHChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumREVPDHChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumREVPDHChannels();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumREVPHModules
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumREVPHModules
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumREVPHModules();
  return value;
}

/*
 * Class:     org_wpilib_hardware_hal_PortsJNI
 * Method:    getNumREVPHChannels
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PortsJNI_getNumREVPHChannels
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetNumREVPHChannels();
  return value;
}
}  // extern "C"
