/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "HAL/cpp/Log.h"

#include "edu_wpi_first_wpilibj_hal_JNIWrapper.h"

#include "HAL/HAL.h"

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_JNIWrapper
 * Method:    getPortWithModule
 * Signature: (BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_JNIWrapper_getPortWithModule(
    JNIEnv* env, jclass, jbyte module, jbyte channel) {
  // FILE_LOG(logDEBUG) << "Calling JNIWrapper getPortWithModlue";
  // FILE_LOG(logDEBUG) << "Module = " << (jint)module;
  // FILE_LOG(logDEBUG) << "Channel = " << (jint)channel;
  HAL_PortHandle port = HAL_GetPortWithModule(module, channel);
  // FILE_LOG(logDEBUG) << "Port Handle = " << port;
  return (jint)port;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_JNIWrapper
 * Method:    getPort
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_JNIWrapper_getPort(
    JNIEnv* env, jclass, jbyte channel) {
  // FILE_LOG(logDEBUG) << "Calling JNIWrapper getPortWithModlue";
  // FILE_LOG(logDEBUG) << "Module = " << (jint)module;
  // FILE_LOG(logDEBUG) << "Channel = " << (jint)channel;
  HAL_PortHandle port = HAL_GetPort(channel);
  // FILE_LOG(logDEBUG) << "Port Handle = " << port;
  return (jint)port;
}
}  // extern "C"
