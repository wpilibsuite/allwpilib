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
	double temperature = getPDPTemperature(&status, module);
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
	double voltage = getPDPVoltage(&status, module);
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
	double current = getPDPChannelCurrent(channel, &status, module);
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
	double current = getPDPTotalCurrent(&status, module);
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
	double power = getPDPTotalPower(&status, module);
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
	double energy = getPDPTotalEnergy(&status, module);
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
	resetPDPTotalEnergy(&status, module);
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
	clearPDPStickyFaults(&status, module);
  CheckStatus(env, status, false);
}

}  // extern "C"
