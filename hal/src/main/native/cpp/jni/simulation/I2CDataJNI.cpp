/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_I2CDataJNI.h"
#include "hal/simulation/I2CData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    registerBus
 * Signature: (ILedu/wpi/first/hal/sim/I2CBus;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_registerBus
  (JNIEnv *, jclass, jint bus, jobject impl)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    getSystemBus
 * Signature: (I)Ledu/wpi/first/hal/sim/mockdata/I2CDataJNI/NativeI2CBus;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_getSystemBus
  (JNIEnv *, jclass, jint bus)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    getSimBus
 * Signature: ()Ledu/wpi/first/hal/sim/mockdata/I2CDataJNI/NativeI2CBus;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_getSimBus
  (JNIEnv *, jclass)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    unregisterBus
 * Signature: (I)Ledu/wpi/first/hal/sim/I2CBus;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_unregisterBus
  (JNIEnv *, jclass, jint bus)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    registerDevice
 * Signature: (IILedu/wpi/first/hal/sim/I2CDevice;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_registerDevice
  (JNIEnv *, jclass, jint bus, jint deviceAddress, jobject device)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    unregisterDevice
 * Signature: (II)Ledu/wpi/first/hal/sim/I2CDevice;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_unregisterDevice
  (JNIEnv *, jclass, jint bus, jint deviceAddress)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    nativeOpenBus
 * Signature: (JJI)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_nativeOpenBus
  (JNIEnv *, jclass, jlong impl, jlong data, jint bus)
{
  int32_t status = 0;
  bool result = reinterpret_cast<HALSIM_I2CBus*>(impl)->OpenBus(
      reinterpret_cast<void*>(data), bus, &status);

  return result;
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    nativeCloseBus
 * Signature: (JJI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_nativeCloseBus
  (JNIEnv *, jclass, jlong impl, jlong data, jint bus)
{
  return reinterpret_cast<HALSIM_I2CBus*>(impl)->CloseBus(
      reinterpret_cast<void*>(data), bus);
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    nativeInitialize
 * Signature: (JJI)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_nativeInitialize
  (JNIEnv *, jclass, jlong impl, jlong data, jint deviceAddress)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    nativeTransaction
 * Signature: (JJI[BI[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_nativeTransaction
  (JNIEnv *, jclass, jlong impl, jlong data, jint deviceAddress,
   jbyteArray dataToSend, jint sendSize, jbyteArray dataReceived,
   jint receiveSize)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    nativeWrite
 * Signature: (JJI[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_nativeWrite
  (JNIEnv *, jclass, jlong impl, jlong data, jint deviceAddress,
   jbyteArray dataToSend, jint sendSize)
{
}

/*
 * Class:     edu_wpi_first_hal_simulation_I2CDataJNI
 * Method:    nativeRead
 * Signature: (JJI[BI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_I2CDataJNI_nativeRead
  (JNIEnv *, jclass, jlong impl, jlong data, jint deviceAddress,
   jbyteArray buffer, jint count)
{
}

}  // extern "C"
