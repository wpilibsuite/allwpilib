/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>
#include "HAL/HAL.h"
#include "HALUtil.h"

#include "edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI.h"

/*
 * Class:     edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI
 * Method:    setFilterSelect
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI_setFilterSelect(
    JNIEnv* env, jclass, jlong port_pointer, jint filter_index) {
  int32_t status = 0;
  void* digital_port_pointer = reinterpret_cast<void*>(port_pointer);

  setFilterSelect(digital_port_pointer, filter_index, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI
 * Method:    getFilterSelect
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI_getFilterSelect(
    JNIEnv* env, jclass, jlong port_pointer) {
  int32_t status = 0;
  void* digital_port_pointer = reinterpret_cast<void*>(port_pointer);

  jint result = getFilterSelect(digital_port_pointer, &status);
  CheckStatus(env, status);
  return result;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI
 * Method:    setFilterPeriod
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI_setFilterPeriod(
    JNIEnv* env, jclass, jint filter_index, jint fpga_cycles) {
  int32_t status = 0;

  setFilterPeriod(filter_index, fpga_cycles, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI
 * Method:    getFilterPeriod
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_DigitalGlitchFilterJNI_getFilterPeriod(
    JNIEnv* env, jclass, jint filter_index) {
  int32_t status = 0;

  jint result = getFilterPeriod(filter_index, &status);
  CheckStatus(env, status);
  return result;
}
