/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <wpi/jni_util.h>

#include "CallbackStore.h"
#include "edu_wpi_first_hal_simulation_DIODataJNI.h"
#include "hal/simulation/DIOData.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    getDisplayName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_getDisplayName
  (JNIEnv* env, jclass, jint index)
{
  const char* displayName = HALSIM_GetDIODisplayName(index);
  return wpi::java::MakeJString(env, displayName);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    setDisplayName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_setDisplayName
  (JNIEnv* env, jclass, jint index, jstring displayName)
{
  wpi::java::JStringRef displayNameRef{env, displayName};
  HALSIM_SetDIODisplayName(index, displayNameRef.c_str());
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    registerInitializedCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_registerInitializedCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDIOInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    cancelInitializedCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_cancelInitializedCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDIOInitializedCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    getInitialized
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_getInitialized
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDIOInitialized(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    setInitialized
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_setInitialized
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetDIOInitialized(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    registerValueCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_registerValueCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDIOValueCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    cancelValueCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_cancelValueCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index, &HALSIM_CancelDIOValueCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    getValue
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_getValue
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDIOValue(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    setValue
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_setValue
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetDIOValue(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    registerPulseLengthCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_registerPulseLengthCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDIOPulseLengthCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    cancelPulseLengthCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_cancelPulseLengthCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDIOPulseLengthCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    getPulseLength
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_getPulseLength
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDIOPulseLength(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    setPulseLength
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_setPulseLength
  (JNIEnv*, jclass, jint index, jdouble value)
{
  HALSIM_SetDIOPulseLength(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    registerIsInputCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_registerIsInputCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDIOIsInputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    cancelIsInputCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_cancelIsInputCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDIOIsInputCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    getIsInput
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_getIsInput
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDIOIsInput(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    setIsInput
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_setIsInput
  (JNIEnv*, jclass, jint index, jboolean value)
{
  HALSIM_SetDIOIsInput(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    registerFilterIndexCallback
 * Signature: (ILjava/lang/Object;Z)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_registerFilterIndexCallback
  (JNIEnv* env, jclass, jint index, jobject callback, jboolean initialNotify)
{
  return sim::AllocateCallback(env, index, callback, initialNotify,
                               &HALSIM_RegisterDIOFilterIndexCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    cancelFilterIndexCallback
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_cancelFilterIndexCallback
  (JNIEnv* env, jclass, jint index, jint handle)
{
  return sim::FreeCallback(env, handle, index,
                           &HALSIM_CancelDIOFilterIndexCallback);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    getFilterIndex
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_getFilterIndex
  (JNIEnv*, jclass, jint index)
{
  return HALSIM_GetDIOFilterIndex(index);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    setFilterIndex
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_setFilterIndex
  (JNIEnv*, jclass, jint index, jint value)
{
  HALSIM_SetDIOFilterIndex(index, value);
}

/*
 * Class:     edu_wpi_first_hal_simulation_DIODataJNI
 * Method:    resetData
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_simulation_DIODataJNI_resetData
  (JNIEnv*, jclass, jint index)
{
  HALSIM_ResetDIOData(index);
}

}  // extern "C"
