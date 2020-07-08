/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
