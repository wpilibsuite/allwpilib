/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "edu_wpi_first_wpiutil_WPIUtilJNI.h"
#include "wpi/PortForwarder.h"
#include "wpi/jni_util.h"
#include "wpi/timestamp.h"

using namespace wpi::java;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {}

/*
 * Class:     edu_wpi_first_wpiutil_WPIUtilJNI
 * Method:    now
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_WPIUtilJNI_now
  (JNIEnv*, jclass)
{
  return wpi::Now();
}

/*
 * Class:     edu_wpi_first_wpiutil_WPIUtilJNI
 * Method:    addPortForwarder
 * Signature: (ILjava/lang/String;I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpiutil_WPIUtilJNI_addPortForwarder
  (JNIEnv* env, jclass, jint port, jstring remoteHost, jint remotePort)
{
  wpi::PortForwarder::GetInstance().Add(static_cast<unsigned int>(port),
                                        JStringRef{env, remoteHost}.str(),
                                        static_cast<unsigned int>(remotePort));
}

/*
 * Class:     edu_wpi_first_wpiutil_WPIUtilJNI
 * Method:    removePortForwarder
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpiutil_WPIUtilJNI_removePortForwarder
  (JNIEnv* env, jclass, jint port)
{
  wpi::PortForwarder::GetInstance().Remove(port);
}

}  // extern "C"
