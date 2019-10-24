#include <jni.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_DutyCycleJNI.h"
#include "hal/DutyCycle.h"

using namespace frc;

extern "C" {
/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    initialize
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_hal_DutyCycleJNI_initialize
  (JNIEnv * env, jclass, jint digitalSoruceHandle, jint analogTriggerType) {
  int32_t status = 0;
  auto handle = HAL_InitializeDutyCycle(static_cast<HAL_Handle>(digitalSourceHandle), static_cast<HAL_AnalogTriggerType>(analogTriggerType), &status);
  CheckStatus(env, status);
  return handle;  
}

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_hal_DutyCycleJNI_free
  (JNIEnv *, jclass, jint handle) {
    HAL_FreeDutyCycle(static_cast<HAL_DutyCycleHandle>(handle));
  }

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    getFrequency
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_hal_DutyCycleJNI_getFrequency
  (JNIEnv * env, jclass, jint handle) {
    int32_t status = 0;
    auto retVal = HAL_GetDutyCycleFrequency(static_cast<HAL_DutyCycleHandle>(handle), &status);
    CheckStatus(env, status);
    return retVal;
  }

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    getOutputRaw
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_hal_DutyCycleJNI_getOutputRaw
  (JNIEnv * env, jclass, jint handle) {
    int32_t status = 0;
    auto retVal = HAL_GetDutyCycleOutputRaw(static_cast<HAL_DutyCycleHandle>(handle), &status);
    CheckStatus(env, status);
    return retVal;
  }


/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    getOutputScaled
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_hal_DutyCycleJNI_getOutputScaled
  (JNIEnv * env, jclass, jint handle) {
    int32_t status = 0;
    auto retVal = HAL_GetDutyCycleOutputScaled(static_cast<HAL_DutyCycleHandle>(handle), &status);
    CheckStatus(env, status);
    return retVal;
  }

}