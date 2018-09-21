/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALUtil.h"
#include "edu_wpi_first_hal_PDPJNI.h"
#include "hal/PDP.h"
#include "hal/Ports.h"

using namespace frc;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    initializePDP
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PDPJNI_initializePDP
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  auto handle = HAL_InitializePDP(module, &status);
  CheckStatusRange(env, status, 0, HAL_GetNumPDPModules(), module);
  return static_cast<jint>(handle);
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    checkPDPChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PDPJNI_checkPDPChannel
  (JNIEnv* env, jclass, jint channel)
{
  return HAL_CheckPDPChannel(channel);
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    checkPDPModule
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PDPJNI_checkPDPModule
  (JNIEnv* env, jclass, jint module)
{
  return HAL_CheckPDPModule(module);
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    getPDPTemperature
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PDPJNI_getPDPTemperature
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  double temperature = HAL_GetPDPTemperature(handle, &status);
  CheckStatus(env, status, false);
  return temperature;
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    getPDPVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PDPJNI_getPDPVoltage
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  double voltage = HAL_GetPDPVoltage(handle, &status);
  CheckStatus(env, status, false);
  return voltage;
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    getPDPChannelCurrent
 * Signature: (BI)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PDPJNI_getPDPChannelCurrent
  (JNIEnv* env, jclass, jbyte channel, jint handle)
{
  int32_t status = 0;
  double current = HAL_GetPDPChannelCurrent(handle, channel, &status);
  CheckStatus(env, status, false);
  return current;
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    getPDPTotalCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PDPJNI_getPDPTotalCurrent
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  double current = HAL_GetPDPTotalCurrent(handle, &status);
  CheckStatus(env, status, false);
  return current;
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    getPDPTotalPower
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PDPJNI_getPDPTotalPower
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  double power = HAL_GetPDPTotalPower(handle, &status);
  CheckStatus(env, status, false);
  return power;
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    getPDPTotalEnergy
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PDPJNI_getPDPTotalEnergy
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  double energy = HAL_GetPDPTotalEnergy(handle, &status);
  CheckStatus(env, status, false);
  return energy;
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    resetPDPTotalEnergy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PDPJNI_resetPDPTotalEnergy
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_ResetPDPTotalEnergy(handle, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_PDPJNI
 * Method:    clearPDPStickyFaults
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PDPJNI_clearPDPStickyFaults
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_ClearPDPStickyFaults(handle, &status);
  CheckStatus(env, status, false);
}

}  // extern "C"
