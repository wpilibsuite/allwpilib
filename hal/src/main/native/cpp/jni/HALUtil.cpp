// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALUtil.h"

#include <jni.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>

#include <fmt/format.h>
#include <wpi/jni_util.h>

#include "org_wpilib_hardware_hal_HALUtil.h"
#include "hal/CAN.h"
#include "hal/DriverStation.h"
#include "hal/Errors.h"
#include "hal/HAL.h"

using namespace wpi::java;

#define kRioStatusOffset -63000
#define kRioStatusSuccess 0
#define kRIOStatusBufferInvalidSize (kRioStatusOffset - 80)
#define kRIOStatusOperationTimedOut -52007
#define kRIOStatusFeatureNotSupported (kRioStatusOffset - 193)
#define kRIOStatusResourceNotInitialized -52010

static_assert(org_wpilib_hardware_hal_HALUtil_RUNTIME_ROBORIO == HAL_Runtime_RoboRIO);
static_assert(org_wpilib_hardware_hal_HALUtil_RUNTIME_ROBORIO2 ==
              HAL_Runtime_RoboRIO2);
static_assert(org_wpilib_hardware_hal_HALUtil_RUNTIME_SIMULATION ==
              HAL_Runtime_Simulation);
static_assert(org_wpilib_hardware_hal_HALUtil_RUNTIME_SYSTEMCORE ==
              HAL_Runtime_SystemCore);

static JavaVM* jvm = nullptr;
static JException illegalArgExCls;
static JException indexOobExCls;
static JException boundaryExCls;
static JException allocationExCls;
static JException halHandleExCls;
static JException uncleanStatusExCls;
static JException nullPointerEx;
static JClass powerDistributionVersionCls;
static JClass canStatusCls;
static JClass matchInfoDataCls;
static JClass canReceiveMessageCls;
static JClass canStreamMessageCls;
static JClass halValueCls;
static JClass revPHVersionCls;
static JClass canStreamOverflowExCls;

static const JClassInit classes[] = {
    {"org/wpilib/hardware/hal/PowerDistributionVersion",
     &powerDistributionVersionCls},
    {"org/wpilib/hardware/hal/can/CANStatus", &canStatusCls},
    {"org/wpilib/hardware/hal/MatchInfoData", &matchInfoDataCls},
    {"org/wpilib/hardware/hal/can/CANReceiveMessage", &canReceiveMessageCls},
    {"org/wpilib/hardware/hal/can/CANStreamMessage", &canStreamMessageCls},
    {"org/wpilib/hardware/hal/HALValue", &halValueCls},
    {"org/wpilib/hardware/hal/REVPHVersion", &revPHVersionCls},
    {"org/wpilib/hardware/hal/can/CANStreamOverflowException",
     &canStreamOverflowExCls}};

static const JExceptionInit exceptions[] = {
    {"java/lang/IllegalArgumentException", &illegalArgExCls},
    {"java/lang/IndexOutOfBoundsException", &indexOobExCls},
    {"org/wpilib/hardware/hal/util/BoundaryException", &boundaryExCls},
    {"org/wpilib/hardware/hal/util/AllocationException", &allocationExCls},
    {"org/wpilib/hardware/hal/util/HalHandleException", &halHandleExCls},
    {"org/wpilib/hardware/hal/util/UncleanStatusException", &uncleanStatusExCls},
    {"java/lang/NullPointerException", &nullPointerEx}};

namespace hal {

void ThrowUncleanStatusException(JNIEnv* env, std::string_view msg,
                                 int32_t status) {
  static jmethodID func =
      env->GetMethodID(uncleanStatusExCls, "<init>", "(ILjava/lang/String;)V");

  jobject exception =
      env->NewObject(uncleanStatusExCls, func, static_cast<jint>(status),
                     MakeJString(env, msg));
  env->Throw(static_cast<jthrowable>(exception));
}

void ThrowAllocationException(JNIEnv* env, const char* lastError,
                              int32_t status) {
  allocationExCls.Throw(env,
                        fmt::format("Code: {}\n{}", status, lastError).c_str());
}

void ThrowHalHandleException(JNIEnv* env, int32_t status) {
  const char* message = HAL_GetLastError(&status);
  halHandleExCls.Throw(env,
                       fmt::format(" Code: {}. {}", status, message).c_str());
}

void ReportError(JNIEnv* env, int32_t status, bool doThrow) {
  if (status == 0) {
    return;
  }
  const char* message = HAL_GetLastError(&status);
  if (status == HAL_HANDLE_ERROR) {
    ThrowHalHandleException(env, status);
    return;
  }
  if (doThrow && status < 0) {
    ThrowUncleanStatusException(
        env, fmt::format(" Code: {}. {}", status, message).c_str(), status);
  } else {
    std::string func;
    auto stack = GetJavaStackTrace(env, &func, "edu.wpi.first");
    // Make a copy of message for safety, calling back into the HAL might
    // invalidate the string.
    std::string lastMessage{message};
    HAL_SendError(1, status, 0, lastMessage.c_str(), func.c_str(),
                  stack.c_str(), 1);
  }
}

void ThrowError(JNIEnv* env, int32_t status, int32_t minRange, int32_t maxRange,
                int32_t requestedValue) {
  if (status == 0) {
    return;
  }
  const char* lastError = HAL_GetLastError(&status);
  if (status == NO_AVAILABLE_RESOURCES || status == RESOURCE_IS_ALLOCATED ||
      status == RESOURCE_OUT_OF_RANGE) {
    ThrowAllocationException(env, lastError, status);
    return;
  }
  if (status == HAL_HANDLE_ERROR) {
    ThrowHalHandleException(env, status);
    return;
  }
  ThrowUncleanStatusException(
      env, fmt::format(" Code: {}. {}", status, lastError).c_str(), status);
}

void ThrowNullPointerException(JNIEnv* env, std::string_view msg) {
  nullPointerEx.Throw(env, msg);
}

void ThrowCANStreamOverflowException(JNIEnv* env, jobjectArray messages,
                                     jint length) {
  static jmethodID constructor =
      env->GetMethodID(canStreamOverflowExCls, "<init>",
                       "([Lorg/wpilib/hardware/hal/CANStreamMessage;I)V");
  jobject exception =
      env->NewObject(canStreamOverflowExCls, constructor, messages, length);
  env->Throw(static_cast<jthrowable>(exception));
}

void ThrowIllegalArgumentException(JNIEnv* env, std::string_view msg) {
  illegalArgExCls.Throw(env, msg);
}

void ThrowIndexOutOfBoundsException(JNIEnv* env, std::string_view msg) {
  indexOobExCls.Throw(env, msg);
}

void ThrowBoundaryException(JNIEnv* env, double value, double lower,
                            double upper) {
  static jmethodID getMessage = nullptr;
  if (!getMessage) {
    getMessage = env->GetStaticMethodID(boundaryExCls, "getMessage",
                                        "(DDD)Ljava/lang/String;");
  }

  static jmethodID constructor = nullptr;
  if (!constructor) {
    constructor =
        env->GetMethodID(boundaryExCls, "<init>", "(Ljava/lang/String;)V");
  }

  jobject msg = env->CallStaticObjectMethod(
      boundaryExCls, getMessage, static_cast<jdouble>(value),
      static_cast<jdouble>(lower), static_cast<jdouble>(upper));
  jobject ex = env->NewObject(boundaryExCls, constructor, msg);
  env->Throw(static_cast<jthrowable>(ex));
}

jobject CreateREVPHVersion(JNIEnv* env, uint32_t firmwareMajor,
                           uint32_t firmwareMinor, uint32_t firmwareFix,
                           uint32_t hardwareMinor, uint32_t hardwareMajor,
                           uint32_t uniqueId) {
  static jmethodID constructor =
      env->GetMethodID(revPHVersionCls, "<init>", "(IIIIII)V");
  return env->NewObject(
      revPHVersionCls, constructor, static_cast<jint>(firmwareMajor),
      static_cast<jint>(firmwareMinor), static_cast<jint>(firmwareFix),
      static_cast<jint>(hardwareMinor), static_cast<jint>(hardwareMajor),
      static_cast<jint>(uniqueId));
}

void SetCanStatusObject(JNIEnv* env, jobject canStatus,
                        float percentBusUtilization, uint32_t busOffCount,
                        uint32_t txFullCount, uint32_t receiveErrorCount,
                        uint32_t transmitErrorCount) {
  static jmethodID func =
      env->GetMethodID(canStatusCls, "setStatus", "(DIIII)V");
  env->CallVoidMethod(
      canStatus, func, static_cast<jdouble>(percentBusUtilization),
      static_cast<jint>(busOffCount), static_cast<jint>(txFullCount),
      static_cast<jint>(receiveErrorCount),
      static_cast<jint>(transmitErrorCount));
}

void SetMatchInfoObject(JNIEnv* env, jobject matchStatus,
                        const HAL_MatchInfo& matchInfo) {
  static jmethodID func =
      env->GetMethodID(matchInfoDataCls, "setData",
                       "(Ljava/lang/String;Ljava/lang/String;III)V");

  env->CallVoidMethod(
      matchStatus, func, MakeJString(env, matchInfo.eventName),
      MakeJString(env,
                  {reinterpret_cast<const char*>(matchInfo.gameSpecificMessage),
                   matchInfo.gameSpecificMessageSize}),
      static_cast<jint>(matchInfo.matchNumber),
      static_cast<jint>(matchInfo.replayNumber),
      static_cast<jint>(matchInfo.matchType));
}

jbyteArray SetCANReceiveMessageObject(JNIEnv* env, jobject canData,
                                      int32_t length, int32_t flags,
                                      uint64_t timestamp) {
  static jmethodID func =
      env->GetMethodID(canReceiveMessageCls, "setReceiveData", "(IIJ)[B");

  jbyteArray retVal = static_cast<jbyteArray>(env->CallObjectMethod(
      canData, func, static_cast<jint>(length), static_cast<jint>(flags),
      static_cast<jlong>(timestamp)));
  return retVal;
}

jbyteArray SetCANStreamObject(JNIEnv* env, jobject canStreamData,
                              int32_t length, int32_t flags, uint32_t messageId,
                              uint64_t timestamp) {
  static jmethodID func =
      env->GetMethodID(canStreamMessageCls, "setStreamData", "(IIIJ)[B");

  jbyteArray retVal = static_cast<jbyteArray>(env->CallObjectMethod(
      canStreamData, func, static_cast<jint>(length), static_cast<jint>(flags),
      static_cast<jint>(messageId), static_cast<jlong>(timestamp)));
  return retVal;
}

jobject CreateHALValue(JNIEnv* env, const HAL_Value& value) {
  static jmethodID fromNative = env->GetStaticMethodID(
      halValueCls, "fromNative", "(IJD)Lorg/wpilib/hardware/hal/HALValue;");
  jlong value1 = 0;
  jdouble value2 = 0.0;
  switch (value.type) {
    case HAL_BOOLEAN:
      value1 = value.data.v_boolean;
      break;
    case HAL_DOUBLE:
      value2 = value.data.v_double;
      break;
    case HAL_ENUM:
      value1 = value.data.v_enum;
      break;
    case HAL_INT:
      value1 = value.data.v_int;
      break;
    case HAL_LONG:
      value1 = value.data.v_long;
      break;
    default:
      break;
  }
  return env->CallStaticObjectMethod(
      halValueCls, fromNative, static_cast<jint>(value.type), value1, value2);
}

jobject CreatePowerDistributionVersion(JNIEnv* env, uint32_t firmwareMajor,
                                       uint32_t firmwareMinor,
                                       uint32_t firmwareFix,
                                       uint32_t hardwareMinor,
                                       uint32_t hardwareMajor,
                                       uint32_t uniqueId) {
  static jmethodID constructor =
      env->GetMethodID(powerDistributionVersionCls, "<init>", "(IIIIII)V");
  return env->NewObject(
      powerDistributionVersionCls, constructor,
      static_cast<jint>(firmwareMajor), static_cast<jint>(firmwareMinor),
      static_cast<jint>(firmwareFix), static_cast<jint>(hardwareMinor),
      static_cast<jint>(hardwareMajor), static_cast<jint>(uniqueId));
}

jobject CreateCANStreamMessage(JNIEnv* env) {
  static jmethodID constructor =
      env->GetMethodID(canStreamMessageCls, "<init>", "()V");
  return env->NewObject(canStreamMessageCls, constructor);
}

JavaVM* GetJVM() {
  return jvm;
}

namespace sim {
jint SimOnLoad(JavaVM* vm, void* reserved);
void SimOnUnload(JavaVM* vm, void* reserved);
}  // namespace sim

}  // namespace hal

using namespace hal;

extern "C" {

//
// indicate JNI version support desired and load classes
//
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  }

  for (auto& c : classes) {
    *c.cls = JClass(env, c.name);
    if (!*c.cls) {
      return JNI_ERR;
    }
  }

  for (auto& c : exceptions) {
    *c.cls = JException(env, c.name);
    if (!*c.cls) {
      return JNI_ERR;
    }
  }

  return sim::SimOnLoad(vm, reserved);
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  sim::SimOnUnload(vm, reserved);

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return;
  }
  // Delete global references

  for (auto& c : classes) {
    c.cls->free(env);
  }
  for (auto& c : exceptions) {
    c.cls->free(env);
  }
  jvm = nullptr;
}

/*
 * Class:     org_wpilib_hardware_hal_HALUtil
 * Method:    getSerialNumber
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_hardware_hal_HALUtil_getSerialNumber
  (JNIEnv* env, jclass)
{
  WPI_String serialNum;
  HAL_GetSerialNumber(&serialNum);
  jstring ret = MakeJString(env, wpi::to_string_view(&serialNum));
  WPI_FreeString(&serialNum);
  return ret;
}

/*
 * Class:     org_wpilib_hardware_hal_HALUtil
 * Method:    getComments
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_hardware_hal_HALUtil_getComments
  (JNIEnv* env, jclass)
{
  WPI_String comments;
  HAL_GetComments(&comments);
  jstring ret = MakeJString(env, wpi::to_string_view(&comments));
  WPI_FreeString(&comments);
  return ret;
}

/*
 * Class:     org_wpilib_hardware_hal_HALUtil
 * Method:    getTeamNumber
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_HALUtil_getTeamNumber
  (JNIEnv* env, jclass)
{
  return HAL_GetTeamNumber();
}

/*
 * Class:     org_wpilib_hardware_hal_HALUtil
 * Method:    getFPGATime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_org_wpilib_hardware_hal_HALUtil_getFPGATime
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  jlong returnValue = HAL_GetFPGATime(&status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_HALUtil
 * Method:    getHALRuntimeType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_HALUtil_getHALRuntimeType
  (JNIEnv* env, jclass)
{
  jint returnValue = HAL_GetRuntimeType();
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_HALUtil
 * Method:    getHALErrorMessage
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_hardware_hal_HALUtil_getHALErrorMessage
  (JNIEnv* paramEnv, jclass, jint paramId)
{
  const char* msg = HAL_GetErrorMessage(paramId);
  return MakeJString(paramEnv, msg);
}

/*
 * Class:     org_wpilib_hardware_hal_HALUtil
 * Method:    getHALErrno
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_HALUtil_getHALErrno
  (JNIEnv*, jclass)
{
  return errno;
}

/*
 * Class:     org_wpilib_hardware_hal_HALUtil
 * Method:    getHALstrerror
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_hardware_hal_HALUtil_getHALstrerror
  (JNIEnv* env, jclass, jint errorCode)
{
  const char* msg = std::strerror(errno);
  return MakeJString(env, msg);
}

}  // extern "C"
