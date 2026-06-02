// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "org_wpilib_hardware_hal_simulation_IMUDataJNI.h"
#include "wpi/hal/simulation/IMUData.h"

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setAngleX
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setAngleX
  (JNIEnv*, jclass, jdouble angle)
{
  return HALSIM_SetIMUAngleX(angle);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setAngleY
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setAngleY
  (JNIEnv*, jclass, jdouble angle)
{
  return HALSIM_SetIMUAngleY(angle);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setAngleZ
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setAngleZ
  (JNIEnv*, jclass, jdouble angle)
{
  return HALSIM_SetIMUAngleZ(angle);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setGyroRateX
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setGyroRateX
  (JNIEnv*, jclass, jdouble rate)
{
  return HALSIM_SetIMUGyroRateX(rate);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setGyroRateY
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setGyroRateY
  (JNIEnv*, jclass, jdouble rate)
{
  return HALSIM_SetIMUGyroRateY(rate);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setGyroRateZ
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setGyroRateZ
  (JNIEnv*, jclass, jdouble rate)
{
  return HALSIM_SetIMUGyroRateZ(rate);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setAccelX
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setAccelX
  (JNIEnv*, jclass, jdouble accel)
{
  return HALSIM_SetIMUAccelX(accel);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setAccelY
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setAccelY
  (JNIEnv*, jclass, jdouble accel)
{
  return HALSIM_SetIMUAccelY(accel);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setAccelZ
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setAccelZ
  (JNIEnv*, jclass, jdouble accel)
{
  return HALSIM_SetIMUAccelZ(accel);
}

/*
 * Class:     org_wpilib_hardware_hal_simulation_IMUDataJNI
 * Method:    setYaw
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_simulation_IMUDataJNI_setYaw
  (JNIEnv*, jclass, jdouble angle)
{
  return HALSIM_SetIMUYaw(angle);
}

}  // extern "C"
