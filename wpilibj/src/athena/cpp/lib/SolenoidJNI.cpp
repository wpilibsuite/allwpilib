/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>
#include "HAL/HAL.h"
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_SolenoidJNI.h"

#include "HALUtil.h"

TLogLevel solenoidJNILogLevel = logERROR;

#define SOLENOIDJNI_LOG(level)     \
  if (level > solenoidJNILogLevel) \
    ;                              \
  else                             \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    initializeSolenoidPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_initializeSolenoidPort(
    JNIEnv *env, jclass, jint port_handle) {
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI initializeSolenoidPort";

  SOLENOIDJNI_LOG(logDEBUG) << "Port Handle = " << (HalPortHandle)port_handle;

  int32_t status = 0;
  HalSolenoidHandle handle =
      initializeSolenoidPort((HalPortHandle)port_handle, &status);

  SOLENOIDJNI_LOG(logDEBUG) << "Status = " << status;
  SOLENOIDJNI_LOG(logDEBUG) << "Solenoid Port Handle = " << handle;

  CheckStatus(env, status);
  return (jint)handle;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    freeSolenoidPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_freeSolenoidPort(
    JNIEnv *env, jclass, jint id) {
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI initializeSolenoidPort";

  SOLENOIDJNI_LOG(logDEBUG) << "Port Handle = " << (HalSolenoidHandle)id;
  freeSolenoidPort((HalSolenoidHandle)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    setSolenoid
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_setSolenoid(
    JNIEnv *env, jclass, jint solenoid_port, jboolean value) {
  SOLENOIDJNI_LOG(logDEBUG) << "Calling SolenoidJNI SetSolenoid";

  SOLENOIDJNI_LOG(logDEBUG) << "Solenoid Port Handle = "
                            << (HalSolenoidHandle)solenoid_port;

  int32_t status = 0;
  setSolenoid((HalSolenoidHandle)solenoid_port, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getSolenoid
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getSolenoid(
    JNIEnv *env, jclass, jint solenoid_port) {
  int32_t status = 0;
  jboolean val = getSolenoid((HalSolenoidHandle)solenoid_port, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getAllSolenoids
 * Signature: (B)Z
 */
JNIEXPORT jbyte JNICALL
Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getAllSolenoids(
    JNIEnv *env, jclass, jbyte module) {
  int32_t status = 0;
  jbyte val = getAllSolenoids(module, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidBlackList
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidBlackList(
    JNIEnv *env, jclass, jbyte module) {
  int32_t status = 0;
  jint val = getPCMSolenoidBlackList(module, &status);
  CheckStatus(env, status);
  return val;
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidVoltageStickyFault
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidVoltageStickyFault(
    JNIEnv *env, jclass, jbyte module) {
  int32_t status = 0;
  bool val = getPCMSolenoidVoltageStickyFault(module, &status);
  CheckStatus(env, status);
  return val;
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    getPCMSolenoidVoltageFault
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_getPCMSolenoidVoltageFault(
    JNIEnv *env, jclass, jbyte module) {
  int32_t status = 0;
  bool val = getPCMSolenoidVoltageFault(module, &status);
  CheckStatus(env, status);
  return val;
}
/*
 * Class:     edu_wpi_first_wpilibj_hal_SolenoidJNI
 * Method:    clearAllPCMStickyFaults
 * Signature: (B)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_SolenoidJNI_clearAllPCMStickyFaults(
    JNIEnv *env, jclass, jbyte module) {
  int32_t status = 0;
  clearAllPCMStickyFaults_sol(module, &status);
  CheckStatus(env, status);
}

}  // extern "C"
