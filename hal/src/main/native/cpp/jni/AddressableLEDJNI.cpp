// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_AddressableLEDJNI.h"
#include "hal/AddressableLED.h"

using namespace hal;
using namespace wpi::java;

static_assert(sizeof(jbyte) * 3 == sizeof(HAL_AddressableLEDData));

static_assert(org_wpilib_hardware_hal_AddressableLEDJNI_COLOR_ORDER_RGB ==
              HAL_ALED_RGB);
static_assert(org_wpilib_hardware_hal_AddressableLEDJNI_COLOR_ORDER_RBG ==
              HAL_ALED_RBG);
static_assert(org_wpilib_hardware_hal_AddressableLEDJNI_COLOR_ORDER_BGR ==
              HAL_ALED_BGR);
static_assert(org_wpilib_hardware_hal_AddressableLEDJNI_COLOR_ORDER_BRG ==
              HAL_ALED_BRG);
static_assert(org_wpilib_hardware_hal_AddressableLEDJNI_COLOR_ORDER_GBR ==
              HAL_ALED_GBR);
static_assert(org_wpilib_hardware_hal_AddressableLEDJNI_COLOR_ORDER_GRB ==
              HAL_ALED_GRB);

extern "C" {
/*
 * Class:     org_wpilib_hardware_hal_AddressableLEDJNI
 * Method:    initialize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AddressableLEDJNI_initialize
  (JNIEnv* env, jclass, jint channel)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto ret = HAL_InitializeAddressableLED(channel, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return ret;
}

/*
 * Class:     org_wpilib_hardware_hal_AddressableLEDJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AddressableLEDJNI_free
  (JNIEnv* env, jclass, jint handle)
{
  if (handle != HAL_kInvalidHandle) {
    HAL_FreeAddressableLED(static_cast<HAL_AddressableLEDHandle>(handle));
  }
}

/*
 * Class:     org_wpilib_hardware_hal_AddressableLEDJNI
 * Method:    setStart
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AddressableLEDJNI_setStart
  (JNIEnv* env, jclass, jint handle, jint start)
{
  int32_t status = 0;
  HAL_SetAddressableLEDStart(static_cast<HAL_AddressableLEDHandle>(handle),
                             start, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AddressableLEDJNI
 * Method:    setLength
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AddressableLEDJNI_setLength
  (JNIEnv* env, jclass, jint handle, jint length)
{
  int32_t status = 0;
  HAL_SetAddressableLEDLength(static_cast<HAL_AddressableLEDHandle>(handle),
                              length, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AddressableLEDJNI
 * Method:    setData
 * Signature: (II[BII)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AddressableLEDJNI_setData
  (JNIEnv* env, jclass, jint outStart, jint colorOrder, jbyteArray data,
   jint start, jint len)
{
  if (!data) {
    ThrowNullPointerException(env, "data is null");
    return;
  }
  if (start < 0) {
    ThrowIndexOutOfBoundsException(env, "start must be >= 0");
    return;
  }
  if (len < 0) {
    ThrowIndexOutOfBoundsException(env, "len must be >= 0");
    return;
  }
  JSpan<const jbyte> cdata{env, data};
  if (static_cast<unsigned int>(start + len) > cdata.size()) {
    ThrowIndexOutOfBoundsException(
        env, "start + len must be smaller than array length");
    return;
  }
  if ((len % 3) != 0) {
    ThrowIndexOutOfBoundsException(env, "len must be a multiple of 3");
    return;
  }
  auto rawdata = cdata.uarray().subspan(start, len);
  int32_t status = 0;
  HAL_SetAddressableLEDData(
      outStart, rawdata.size() / 3,
      static_cast<HAL_AddressableLEDColorOrder>(colorOrder),
      reinterpret_cast<const HAL_AddressableLEDData*>(rawdata.data()), &status);
}

/*
 * Class:     org_wpilib_hardware_hal_AddressableLEDJNI
 * Method:    setDataFromBuffer
 * Signature: (IILjava/lang/Object;II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AddressableLEDJNI_setDataFromBuffer
  (JNIEnv* env, jclass, jint outStart, jint colorOrder, jobject data,
   jint start, jint len)
{
  if (!data) {
    ThrowNullPointerException(env, "data is null");
    return;
  }
  if (start < 0) {
    ThrowIndexOutOfBoundsException(env, "start must be >= 0");
    return;
  }
  if (len < 0) {
    ThrowIndexOutOfBoundsException(env, "len must be >= 0");
    return;
  }
  JSpan<const jbyte> cdata{env, data, static_cast<size_t>(start + len)};
  if (!cdata) {
    ThrowIllegalArgumentException(env, "data must be a native ByteBuffer");
    return;
  }
  if ((len % 3) != 0) {
    ThrowIndexOutOfBoundsException(env, "len must be a multiple of 3");
    return;
  }
  auto rawdata = cdata.uarray().subspan(start, len);
  int32_t status = 0;
  HAL_SetAddressableLEDData(
      outStart, rawdata.size() / 3,
      static_cast<HAL_AddressableLEDColorOrder>(colorOrder),
      reinterpret_cast<const HAL_AddressableLEDData*>(rawdata.data()), &status);
}
}  // extern "C"
