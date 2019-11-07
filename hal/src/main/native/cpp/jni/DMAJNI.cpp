/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_DMAJNI.h"
#include "hal/DMA.h"

using namespace frc;

extern "C" {
/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    initialize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_hal_DMAJNI_initialize
  (JNIEnv * env, jclass) {
  int32_t status = 0;
  auto handle = HAL_InitializeDMA(&status);
  CheckStatus(env, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_free
  (JNIEnv * env, jclass, jint handle) {
  HAL_FreeDMA(handle);
}

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    setPause
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_setPause
  (JNIEnv *env, jclass, jint handle, jboolean pause) {
    int32_t status = 0;
    HAL_SetDMAPause(handle, pause, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    setRate
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_setRate
  (JNIEnv *env, jclass, jint handle, jint rate) {
        int32_t status = 0;
    HAL_SetDMARate(handle, rate, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addEncoder
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addEncoder
  (JNIEnv *env, jclass, jint handle, jint encoderHandle) {
        int32_t status = 0;
    HAL_AddDMAEncoder(handle, encoderHandle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addEncoderRate
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addEncoderRate
  (JNIEnv *env, jclass, jint handle, jint encoderHandle) {
        int32_t status = 0;
    HAL_AddDMAEncoderRate(handle, encoderHandle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addCounter
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addCounter
  (JNIEnv *env, jclass, jint handle, jint counterHandle) {
        int32_t status = 0;
    HAL_AddDMACounter(handle, counterHandle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addCounterRate
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addCounterRate
  (JNIEnv *env, jclass, jint handle, jint counterHandle) {
        int32_t status = 0;
    HAL_AddDMACounterRate(handle, counterHandle, &status);
    CheckStatus(env, status);
  }

  /*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addDutyCycle
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addDutyCycle
  (JNIEnv *env, jclass, jint handle, jint dutyCycleHandle) {
        int32_t status = 0;
    HAL_AddDMADutyCycle(handle, dutyCycleHandle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addDigitalSource
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addDigitalSource
  (JNIEnv *env, jclass, jint handle, jint digitalSourceHandle) {
        int32_t status = 0;
    HAL_AddDMADigitalSource(handle, digitalSourceHandle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addAnalogInput
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addAnalogInput
  (JNIEnv *env, jclass, jint handle, jint analogInputHandle) {
        int32_t status = 0;
    HAL_AddDMAAnalogInput(handle, analogInputHandle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addAveragedAnalogInput
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addAveragedAnalogInput
  (JNIEnv *env, jclass, jint handle, jint analogInputHandle) {
        int32_t status = 0;
    HAL_AddDMAAveragedAnalogInput(handle, analogInputHandle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    addAnalogAccumulator
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_addAnalogAccumulator
  (JNIEnv *env, jclass, jint handle, jint analogInputHandle) {
        int32_t status = 0;
    HAL_AddDMAAnalogAccumulator(handle, analogInputHandle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    setExternalTrigger
 * Signature: (IIIZZ)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_setExternalTrigger
  (JNIEnv * env, jclass, jint handle, jint digitalSourceHandle, jint analogTriggerType, jboolean rising, jboolean falling) {
        int32_t status = 0;
    HAL_SetDMAExternalTrigger(handle, digitalSourceHandle, static_cast<HAL_AnalogTriggerType>(analogTriggerType), rising, falling, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    startDMA
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_startDMA
  (JNIEnv * env, jclass, jint handle, jint queueDepth) {
            int32_t status = 0;
    HAL_StartDMA(handle, queueDepth, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    stopDMA
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DMAJNI_stopDMA
  (JNIEnv * env, jclass, jint handle) {
                int32_t status = 0;
    HAL_StopDMA(handle, &status);
    CheckStatus(env, status);
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    readDMA
 * Signature: (II[I)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_hal_DMAJNI_readDMA
  (JNIEnv * env, jclass, jint handle, jint timeoutMs, jintArray arr) {
                    int32_t status = 0;
    HAL_DMASample dmaSample;
    int32_t remaining = 0;
    HAL_ReadDMA(handle, &dmaSample, timeoutMs, &remaining, &status);
    CheckStatus(env, status);

    static_assert(sizeof(uint32_t) == sizeof(jint), "Java ints must be 32 bits");

    uint32_t* nativeArr = static_cast<uint32_t*>(env->GetPrimitiveArrayCritical(arr, nullptr));

    env->ReleasePrimitiveArrayCritical(arr, nativeArr, JNI_ABORT);



    return dmaSample.timeStamp;
  }

/*
 * Class:     edu_wpi_first_hal_DMAJNI
 * Method:    getSensorReadData
 * Signature: (I)Ledu/wpi/first/hal/DMASample/BaseStore;
 */
JNIEXPORT jobject JNICALL Java_edu_wpi_first_hal_DMAJNI_getSensorReadData
  (JNIEnv *, jclass, jint) {
    return nullptr;
  }

}
