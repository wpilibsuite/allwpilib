// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_REVPHJNI.h"
#include "hal/Ports.h"
#include "hal/REVPH.h"
#include "hal/handles/HandlesInternal.h"

static_assert(
    edu_wpi_first_hal_REVPHJNI_COMPRESSOR_CONFIG_TYPE_DISABLED ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_kDisabled);
static_assert(
    edu_wpi_first_hal_REVPHJNI_COMPRESSOR_CONFIG_TYPE_DIGITAL ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_kDigital);
static_assert(
    edu_wpi_first_hal_REVPHJNI_COMPRESSOR_CONFIG_TYPE_ANALOG ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_kAnalog);
static_assert(
    edu_wpi_first_hal_REVPHJNI_COMPRESSOR_CONFIG_TYPE_HYBRID ==
    HAL_REVPHCompressorConfigType::HAL_REVPHCompressorConfigType_kHybrid);

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    initialize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_REVPHJNI_initialize
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto handle = HAL_InitializeREVPH(module, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_free
  (JNIEnv* env, jclass, jint handle)
{
  HAL_FreeREVPH(handle);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    checkSolenoidChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_REVPHJNI_checkSolenoidChannel
  (JNIEnv*, jclass, jint channel)
{
  return HAL_CheckREVPHSolenoidChannel(channel);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getCompressor
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getCompressor
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHCompressor(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    setCompressorConfig
 * Signature: (IDDZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_setCompressorConfig
  (JNIEnv* env, jclass, jint handle, jdouble minAnalogVoltage,
   jdouble maxAnalogVoltage, jboolean forceDisable, jboolean useDigital)
{
  int32_t status = 0;
  HAL_REVPHCompressorConfig config;
  config.minAnalogVoltage = minAnalogVoltage;
  config.maxAnalogVoltage = maxAnalogVoltage;
  config.useDigital = useDigital;
  config.forceDisable = forceDisable;
  HAL_SetREVPHCompressorConfig(handle, &config, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    setClosedLoopControlDisabled
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_setClosedLoopControlDisabled
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_SetREVPHClosedLoopControlDisabled(handle, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    setClosedLoopControlDigital
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_setClosedLoopControlDigital
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_SetREVPHClosedLoopControlDigital(handle, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    setClosedLoopControlAnalog
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_setClosedLoopControlAnalog
  (JNIEnv* env, jclass, jint handle, jdouble minAnalogVoltage,
   jdouble maxAnalogVoltage)
{
  int32_t status = 0;
  HAL_SetREVPHClosedLoopControlAnalog(handle, minAnalogVoltage,
                                      maxAnalogVoltage, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    setClosedLoopControlHybrid
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_setClosedLoopControlHybrid
  (JNIEnv* env, jclass, jint handle, jdouble minAnalogVoltage,
   jdouble maxAnalogVoltage)
{
  int32_t status = 0;
  HAL_SetREVPHClosedLoopControlHybrid(handle, minAnalogVoltage,
                                      maxAnalogVoltage, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getCompressorConfig
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getCompressorConfig
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto config = HAL_GetREVPHCompressorConfig(handle, &status);
  CheckStatus(env, status, false);
  return static_cast<jint>(config);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getPressureSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getPressureSwitch
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHPressureSwitch(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getAnalogVoltage
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getAnalogVoltage
  (JNIEnv* env, jclass, jint handle, jint channel)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHAnalogVoltage(handle, channel, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getCompressorCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getCompressorCurrent
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHCompressorCurrent(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getSolenoids
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getSolenoids
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHSolenoids(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    setSolenoids
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_setSolenoids
  (JNIEnv* env, jclass, jint handle, jint mask, jint value)
{
  int32_t status = 0;
  HAL_SetREVPHSolenoids(handle, mask, value, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    fireOneShot
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_fireOneShot
  (JNIEnv* env, jclass, jint handle, jint index, jint durMs)
{
  int32_t status = 0;
  HAL_FireREVPHOneShot(handle, index, durMs, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    clearStickyFaults
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_clearStickyFaults
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_ClearREVPHStickyFaults(handle, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getInputVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getInputVoltage
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto voltage = HAL_GetREVPHVoltage(handle, &status);
  CheckStatus(env, status, false);
  return voltage;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    get5VVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_REVPHJNI_get5VVoltage
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto voltage = HAL_GetREVPH5VVoltage(handle, &status);
  CheckStatus(env, status, false);
  return voltage;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getSolenoidCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getSolenoidCurrent
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto voltage = HAL_GetREVPHSolenoidCurrent(handle, &status);
  CheckStatus(env, status, false);
  return voltage;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getSolenoidVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getSolenoidVoltage
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto voltage = HAL_GetREVPHSolenoidVoltage(handle, &status);
  CheckStatus(env, status, false);
  return voltage;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getStickyFaultsNative
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getStickyFaultsNative
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_REVPHStickyFaults halFaults;
  std::memset(&halFaults, 0, sizeof(halFaults));
  HAL_GetREVPHStickyFaults(handle, &halFaults, &status);
  CheckStatus(env, status, false);
  jint faults;
  static_assert(sizeof(faults) == sizeof(halFaults));
  std::memcpy(&faults, &halFaults, sizeof(faults));
  return faults;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getFaultsNative
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getFaultsNative
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_REVPHFaults halFaults;
  std::memset(&halFaults, 0, sizeof(halFaults));
  HAL_GetREVPHFaults(handle, &halFaults, &status);
  CheckStatus(env, status, false);
  jint faults;
  static_assert(sizeof(faults) == sizeof(halFaults));
  std::memcpy(&faults, &halFaults, sizeof(faults));
  return faults;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getSolenoidDisabledList
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getSolenoidDisabledList
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHSolenoidDisabledList(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getVersion
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getVersion
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_REVPHVersion version;
  std::memset(&version, 0, sizeof(version));
  HAL_GetREVPHVersion(handle, &version, &status);
  CheckStatus(env, status, false);
  return CreateREVPHVersion(env, version.firmwareMajor, version.firmwareMinor,
                            version.firmwareFix, version.hardwareMinor,
                            version.hardwareMajor, version.uniqueId);
}

}  // extern "C"
