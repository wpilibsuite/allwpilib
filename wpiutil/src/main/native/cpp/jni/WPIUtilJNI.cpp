// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "edu_wpi_first_util_WPIUtilJNI.h"
#include "wpi/PortForwarder.h"
#include "wpi/jni_util.h"
#include "wpi/timestamp.h"

using namespace wpi::java;

static bool mockTimeEnabled = false;
static uint64_t mockNow = 0;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    enableMockTime
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_enableMockTime
  (JNIEnv*, jclass)
{
  mockTimeEnabled = true;
  wpi::SetNowImpl([] { return mockNow; });
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    setMockTime
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_setMockTime
  (JNIEnv*, jclass, jlong time)
{
  mockNow = time;
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    now
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_now
  (JNIEnv*, jclass)
{
  if (mockTimeEnabled) {
    return mockNow;
  } else {
    return wpi::Now();
  }
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    addPortForwarder
 * Signature: (ILjava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_addPortForwarder
  (JNIEnv* env, jclass, jint port, jstring remoteHost, jint remotePort)
{
  wpi::PortForwarder::GetInstance().Add(static_cast<unsigned int>(port),
                                        JStringRef{env, remoteHost}.str(),
                                        static_cast<unsigned int>(remotePort));
}

/*
 * Class:     edu_wpi_first_util_WPIUtilJNI
 * Method:    removePortForwarder
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_WPIUtilJNI_removePortForwarder
  (JNIEnv* env, jclass, jint port)
{
  wpi::PortForwarder::GetInstance().Remove(port);
}

}  // extern "C"
