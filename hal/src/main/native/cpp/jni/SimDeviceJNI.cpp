/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_SimDeviceJNI.h"
#include "hal/SimDevice.h"

using namespace wpi::java;

static HAL_Value ValueFromJava(jint type, jlong value1, jdouble value2) {
  HAL_Value value;
  value.type = static_cast<HAL_Type>(type);
  switch (value.type) {
    case HAL_BOOLEAN:
      value.data.v_boolean = value1;
      break;
    case HAL_DOUBLE:
      value.data.v_double = value2;
      break;
    case HAL_ENUM:
      value.data.v_enum = value1;
      break;
    case HAL_INT:
      value.data.v_int = value1;
      break;
    case HAL_LONG:
      value.data.v_long = value1;
      break;
    default:
      break;
  }
  return value;
}

extern "C" {

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    createSimDevice
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_createSimDevice
  (JNIEnv* env, jclass, jstring name)
{
  return HAL_CreateSimDevice(JStringRef{env, name}.c_str());
}

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    freeSimDevice
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_freeSimDevice
  (JNIEnv*, jclass, jint handle)
{
  HAL_FreeSimDevice(handle);
}

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    createSimValueNative
 * Signature: (ILjava/lang/String;ZIJD)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_createSimValueNative
  (JNIEnv* env, jclass, jint device, jstring name, jboolean readonly, jint type,
   jlong value1, jdouble value2)
{
  return HAL_CreateSimValue(device, JStringRef{env, name}.c_str(), readonly,
                            ValueFromJava(type, value1, value2));
}

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    createSimValueEnum
 * Signature: (ILjava/lang/String;Z[Ljava/lang/Object;I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_createSimValueEnum
  (JNIEnv* env, jclass, jint device, jstring name, jboolean readonly,
   jobjectArray options, jint initialValue)
{
  size_t len = env->GetArrayLength(options);
  std::vector<std::string> arr;
  arr.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(options, i))};
    if (!elem) return 0;
    arr.push_back(JStringRef{env, elem}.str());
  }
  wpi::SmallVector<const char*, 8> carr;
  for (auto&& val : arr) carr.push_back(val.c_str());
  return HAL_CreateSimValueEnum(device, JStringRef{env, name}.c_str(), readonly,
                                len, carr.data(), initialValue);
}

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    getSimValue
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_getSimValue
  (JNIEnv* env, jclass, jint handle)
{
  return hal::CreateHALValue(env, HAL_GetSimValue(handle));
}

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    getSimValueDouble
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_getSimValueDouble
  (JNIEnv*, jclass, jint handle)
{
  return HAL_GetSimValueDouble(handle);
}

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    getSimValueEnum
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_getSimValueEnum
  (JNIEnv*, jclass, jint handle)
{
  return HAL_GetSimValueEnum(handle);
}

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    getSimValueBoolean
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_getSimValueBoolean
  (JNIEnv*, jclass, jint handle)
{
  return HAL_GetSimValueBoolean(handle);
}

/*
 * Class:     edu_wpi_first_hal_SimDeviceJNI
 * Method:    setSimValueNative
 * Signature: (IIJD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_SimDeviceJNI_setSimValueNative
  (JNIEnv*, jclass, jint handle, jint type, jlong value1, jdouble value2)
{
  HAL_SetSimValue(handle, ValueFromJava(type, value1, value2));
}

}  // extern "C"
