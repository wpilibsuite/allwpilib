/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "edu_wpi_first_wpilibj_hal_PDPJNI.h"
#include "HAL/PDP.hpp"
#include "HALUtil.h"

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTemperature
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_initializePDP
  (JNIEnv *, jclass, jint module)
{
	initializePDP(module);
}
  
/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTemperature
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTemperature
  (JNIEnv *env, jclass, jint module)
{
	int32_t status = 0;
	double temperature = getPDPTemperature(module, &status);
  CheckStatus(env, status, false);
  return temperature;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPVoltage
  (JNIEnv *env, jclass, jint module)
{
	int32_t status = 0;
	double voltage = getPDPVoltage(module, &status);
  CheckStatus(env, status, false);
  return voltage;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPChannelCurrent
 * Signature: (BI)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPChannelCurrent
  (JNIEnv *env, jclass, jbyte channel, jint module)
{
	int32_t status = 0;
	double current = getPDPChannelCurrent(module, channel, &status);
  CheckStatus(env, status, false);
  return current;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTotalCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTotalCurrent
  (JNIEnv *env, jclass, jint module)
{
	int32_t status = 0;
	double current = getPDPTotalCurrent(module, &status);
  CheckStatus(env, status, false);
  return current;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    getPDPTotalPower
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTotalPower
  (JNIEnv *env, jclass, jint module)
{
	int32_t status = 0;
	double power = getPDPTotalPower(module, &status);
  CheckStatus(env, status, false);
  return power;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    resetPDPTotalEnergy
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_getPDPTotalEnergy
  (JNIEnv *env, jclass, jint module)
{
	int32_t status = 0;
	double energy = getPDPTotalEnergy(module, &status);
  CheckStatus(env, status, false);
  return energy;
}


/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    resetPDPTotalEnergy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_resetPDPTotalEnergy
  (JNIEnv *env, jclass, jint module)
{
	int32_t status = 0;
	resetPDPTotalEnergy(module, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PDPJNI
 * Method:    clearStickyFaults
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PDPJNI_clearPDPStickyFaults
  (JNIEnv *env, jclass, jint module)
{
	int32_t status = 0;
	clearPDPStickyFaults(module, &status);
  CheckStatus(env, status, false);
}

}  // extern "C"
