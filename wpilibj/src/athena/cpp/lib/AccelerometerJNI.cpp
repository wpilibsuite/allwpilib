/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/Accelerometer.h"
#include <jni.h>
#include "edu_wpi_first_wpilibj_hal_AccelerometerJNI.h"

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    setAccelerometerActive
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_setAccelerometerActive(
    JNIEnv *, jclass, jboolean active) {
  setAccelerometerActive(active);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    setAccelerometerRange
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_setAccelerometerRange(
    JNIEnv *, jclass, jint range) {
  setAccelerometerRange((AccelerometerRange)range);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    getAccelerometerX
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_getAccelerometerX(
    JNIEnv *, jclass) {
  return getAccelerometerX();
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    getAccelerometerY
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_getAccelerometerY(
    JNIEnv *, jclass) {
  return getAccelerometerY();
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    getAccelerometerZ
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_getAccelerometerZ(
    JNIEnv *, jclass) {
  return getAccelerometerZ();
}

}  // extern "C"
