#include <jni.h>
#include "edu_wpi_first_wpilibj_hal_AccelerometerJNI.h"
#include "HAL/Accelerometer.hpp"

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    setAccelerometerActive
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_setAccelerometerActive
  (JNIEnv *, jclass, jboolean active)
{
	setAccelerometerActive(active);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    setAccelerometerRange
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_setAccelerometerRange
  (JNIEnv *, jclass, jint range)
{
	setAccelerometerRange((AccelerometerRange)range);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    getAccelerometerX
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_getAccelerometerX
  (JNIEnv *, jclass)
{
	return getAccelerometerX();
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    getAccelerometerY
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_getAccelerometerY
  (JNIEnv *, jclass)
{
	return getAccelerometerY();
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AccelerometerJNI
 * Method:    getAccelerometerZ
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_AccelerometerJNI_getAccelerometerZ
  (JNIEnv *, jclass)
{
	return getAccelerometerZ();
}
