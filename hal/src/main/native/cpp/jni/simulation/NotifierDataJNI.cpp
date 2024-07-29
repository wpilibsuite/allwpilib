// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "edu_wpi_first_hal_simulation_NotifierDataJNI.h"
#include "hal/simulation/NotifierData.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_NotifierDataJNI
 * Method:    getNextTimeout
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_simulation_NotifierDataJNI_getNextTimeout
  (JNIEnv*, jclass)
{
  return HALSIM_GetNextNotifierTimeout();
}

/*
 * Class:     edu_wpi_first_hal_simulation_NotifierDataJNI
 * Method:    getNumNotifiers
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_NotifierDataJNI_getNumNotifiers
  (JNIEnv*, jclass)
{
  return HALSIM_GetNumNotifiers();
}

}  // extern "C"
