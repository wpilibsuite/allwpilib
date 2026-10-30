// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "org_wpilib_networktables_AlertBackend.h"
#include "wpi/nt/AlertBackend.hpp"
#include "wpi/util/jni_util.hpp"

extern "C" {
/*
 * Class:     org_wpilib_networktables_AlertBackend
 * Method:    installImpl
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_networktables_AlertBackend_installImpl
  (JNIEnv* env, jclass, jint instance, jstring publicationRoot)
{
  wpi::nt::InstallAlertBackend(
      wpi::nt::NetworkTableInstance{instance},
      wpi::util::java::JStringRef{env, publicationRoot}.str());
}
}  // extern "C"
