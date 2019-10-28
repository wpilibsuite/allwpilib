/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "edu_wpi_first_hal_AccelerometerJNI.h"
#include "hal/Accelerometer.h"

extern "C" {

/*
 * Class:     edu_wpi_first_hal_AccelerometerJNI
 * Method:    setAccelerometerActive
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AccelerometerJNI_setAccelerometerActive
  (JNIEnv*, jclass, jboolean active)
{
  HAL_SetAccelerometerActive(active);
}

/*
 * Class:     edu_wpi_first_hal_AccelerometerJNI
 * Method:    setAccelerometerRange
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AccelerometerJNI_setAccelerometerRange
  (JNIEnv*, jclass, jint range)
{
  HAL_SetAccelerometerRange((HAL_AccelerometerRange)range);
}

/*
 * Class:     edu_wpi_first_hal_AccelerometerJNI
 * Method:    getAccelerometerX
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AccelerometerJNI_getAccelerometerX
  (JNIEnv*, jclass)
{
  return HAL_GetAccelerometerX();
}

/*
 * Class:     edu_wpi_first_hal_AccelerometerJNI
 * Method:    getAccelerometerY
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AccelerometerJNI_getAccelerometerY
  (JNIEnv*, jclass)
{
  return HAL_GetAccelerometerY();
}

/*
 * Class:     edu_wpi_first_hal_AccelerometerJNI
 * Method:    getAccelerometerZ
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AccelerometerJNI_getAccelerometerZ
  (JNIEnv*, jclass)
{
  return HAL_GetAccelerometerZ();
}

}  // extern "C"
