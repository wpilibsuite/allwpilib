/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_JNIWrapper.h"

#include "HAL/HAL.hpp"

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_JNIWrapper
 * Method:    getPortWithModule
 * Signature: (BB)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_JNIWrapper_getPortWithModule(
    JNIEnv* env, jclass, jbyte module, jbyte pin) {
  // FILE_LOG(logDEBUG) << "Calling JNIWrapper getPortWithModlue";
  // FILE_LOG(logDEBUG) << "Module = " << (jint)module;
  // FILE_LOG(logDEBUG) << "Pin = " << (jint)pin;
  void* port = getPortWithModule(module, pin);
  // FILE_LOG(logDEBUG) << "Port Ptr = " << port;
  return (jlong)port;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_JNIWrapper
 * Method:    getPort
 * Signature: (BB)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_JNIWrapper_getPort(
    JNIEnv* env, jclass, jbyte pin) {
  // FILE_LOG(logDEBUG) << "Calling JNIWrapper getPortWithModlue";
  // FILE_LOG(logDEBUG) << "Module = " << (jint)module;
  // FILE_LOG(logDEBUG) << "Pin = " << (jint)pin;
  void* port = getPort(pin);
  // FILE_LOG(logDEBUG) << "Port Ptr = " << port;
  return (jlong)port;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_JNIWrapper
 * Method:    freePort
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_JNIWrapper_freePort(
    JNIEnv* env, jclass, jlong id) {
  // FILE_LOG(logDEBUG) << "Calling JNIWrapper getPortWithModlue";
  // FILE_LOG(logDEBUG) << "Module = " << (jint)module;
  // FILE_LOG(logDEBUG) << "Pin = " << (jint)pin;
  freePort((void*)id);
  // FILE_LOG(logDEBUG) << "Port Ptr = " << port;
}

}  // extern "C"
