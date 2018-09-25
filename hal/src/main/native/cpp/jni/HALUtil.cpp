/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALUtil.h"

#include <jni.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>

#include <wpi/SmallString.h>
#include <wpi/jni_util.h>
#include <wpi/raw_ostream.h>

#include "edu_wpi_first_hal_HALUtil.h"
#include "hal/CAN.h"
#include "hal/DriverStation.h"
#include "hal/Errors.h"
#include "hal/HAL.h"
#include "hal/cpp/Log.h"

using namespace wpi::java;

// set the logging level
TLogLevel halUtilLogLevel = logWARNING;

#define HALUTIL_LOG(level)     \
  if (level > halUtilLogLevel) \
    ;                          \
  else                         \
    Log().Get(level)

#define kRioStatusOffset -63000
#define kRioStatusSuccess 0
#define kRIOStatusBufferInvalidSize (kRioStatusOffset - 80)
#define kRIOStatusOperationTimedOut -52007
#define kRIOStatusFeatureNotSupported (kRioStatusOffset - 193)
#define kRIOStatusResourceNotInitialized -52010

static JavaVM* jvm = nullptr;
static JException illegalArgExCls;
static JException boundaryExCls;
static JException allocationExCls;
static JException halHandleExCls;
static JException canInvalidBufferExCls;
static JException canMessageNotFoundExCls;
static JException canMessageNotAllowedExCls;
static JException canNotInitializedExCls;
static JException uncleanStatusExCls;
static JClass pwmConfigDataResultCls;
static JClass canStatusCls;
static JClass matchInfoDataCls;
static JClass accumulatorResultCls;
static JClass canDataCls;

static const JClassInit classes[] = {
    {"edu/wpi/first/hal/PWMConfigDataResult", &pwmConfigDataResultCls},
    {"edu/wpi/first/hal/can/CANStatus", &canStatusCls},
    {"edu/wpi/first/hal/MatchInfoData", &matchInfoDataCls},
    {"edu/wpi/first/hal/AccumulatorResult", &accumulatorResultCls},
    {"edu/wpi/first/hal/CANData", &canDataCls}};

static const JExceptionInit exceptions[] = {
    {"java/lang/IllegalArgumentException", &illegalArgExCls},
    {"edu/wpi/first/hal/util/BoundaryException", &boundaryExCls},
    {"edu/wpi/first/hal/util/AllocationException", &allocationExCls},
    {"edu/wpi/first/hal/util/HalHandleException", &halHandleExCls},
    {"edu/wpi/first/hal/can/CANInvalidBufferException", &canInvalidBufferExCls},
    {"edu/wpi/first/hal/can/CANMessageNotFoundException",
     &canMessageNotFoundExCls},
    {"edu/wpi/first/hal/can/CANMessageNotAllowedException",
     &canMessageNotAllowedExCls},
    {"edu/wpi/first/hal/can/CANNotInitializedException",
     &canNotInitializedExCls},
    {"edu/wpi/first/hal/util/UncleanStatusException", &uncleanStatusExCls}};

namespace frc {

void ThrowUncleanStatusException(JNIEnv* env, wpi::StringRef msg,
                                 int32_t status) {
  static jmethodID func =
      env->GetMethodID(uncleanStatusExCls, "<init>", "(ILjava/lang/String;)V");

  jobject exception =
      env->NewObject(uncleanStatusExCls, func, static_cast<jint>(status),
                     MakeJString(env, msg));
  env->Throw(static_cast<jthrowable>(exception));
}

void ThrowAllocationException(JNIEnv* env, int32_t minRange, int32_t maxRange,
                              int32_t requestedValue, int32_t status) {
  const char* message = HAL_GetErrorMessage(status);
  wpi::SmallString<1024> buf;
  wpi::raw_svector_ostream oss(buf);
  oss << " Code: " << status << ". " << message
      << ", Minimum Value: " << minRange << ", Maximum Value: " << maxRange
      << ", Requested Value: " << requestedValue;
  env->ThrowNew(allocationExCls, buf.c_str());
  allocationExCls.Throw(env, buf.c_str());
}

void ThrowHalHandleException(JNIEnv* env, int32_t status) {
  const char* message = HAL_GetErrorMessage(status);
  wpi::SmallString<1024> buf;
  wpi::raw_svector_ostream oss(buf);
  oss << " Code: " << status << ". " << message;
  halHandleExCls.Throw(env, buf.c_str());
}

void ReportError(JNIEnv* env, int32_t status, bool doThrow) {
  if (status == 0) return;
  if (status == HAL_HANDLE_ERROR) {
    ThrowHalHandleException(env, status);
  }
  const char* message = HAL_GetErrorMessage(status);
  if (doThrow && status < 0) {
    wpi::SmallString<1024> buf;
    wpi::raw_svector_ostream oss(buf);
    oss << " Code: " << status << ". " << message;
    ThrowUncleanStatusException(env, buf.c_str(), status);
  } else {
    std::string func;
    auto stack = GetJavaStackTrace(env, &func, "edu.wpi.first.wpilibj");
    HAL_SendError(1, status, 0, message, func.c_str(), stack.c_str(), 1);
  }
}

void ThrowError(JNIEnv* env, int32_t status, int32_t minRange, int32_t maxRange,
                int32_t requestedValue) {
  if (status == 0) return;
  if (status == NO_AVAILABLE_RESOURCES || status == RESOURCE_IS_ALLOCATED ||
      status == RESOURCE_OUT_OF_RANGE) {
    ThrowAllocationException(env, minRange, maxRange, requestedValue, status);
  }
  if (status == HAL_HANDLE_ERROR) {
    ThrowHalHandleException(env, status);
  }
  const char* message = HAL_GetErrorMessage(status);
  wpi::SmallString<1024> buf;
  wpi::raw_svector_ostream oss(buf);
  oss << " Code: " << status << ". " << message;
  ThrowUncleanStatusException(env, buf.c_str(), status);
}

void ReportCANError(JNIEnv* env, int32_t status, int message_id) {
  if (status >= 0) return;
  switch (status) {
    case kRioStatusSuccess:
      // Everything is ok... don't throw.
      break;
    case HAL_ERR_CANSessionMux_InvalidBuffer:
    case kRIOStatusBufferInvalidSize: {
      static jmethodID invalidBufConstruct = nullptr;
      if (!invalidBufConstruct)
        invalidBufConstruct =
            env->GetMethodID(canInvalidBufferExCls, "<init>", "()V");
      jobject exception =
          env->NewObject(canInvalidBufferExCls, invalidBufConstruct);
      env->Throw(static_cast<jthrowable>(exception));
      break;
    }
    case HAL_ERR_CANSessionMux_MessageNotFound:
    case kRIOStatusOperationTimedOut: {
      static jmethodID messageNotFoundConstruct = nullptr;
      if (!messageNotFoundConstruct)
        messageNotFoundConstruct =
            env->GetMethodID(canMessageNotFoundExCls, "<init>", "()V");
      jobject exception =
          env->NewObject(canMessageNotFoundExCls, messageNotFoundConstruct);
      env->Throw(static_cast<jthrowable>(exception));
      break;
    }
    case HAL_ERR_CANSessionMux_NotAllowed:
    case kRIOStatusFeatureNotSupported: {
      wpi::SmallString<100> buf;
      wpi::raw_svector_ostream oss(buf);
      oss << "MessageID = " << message_id;
      canMessageNotAllowedExCls.Throw(env, buf.c_str());
      break;
    }
    case HAL_ERR_CANSessionMux_NotInitialized:
    case kRIOStatusResourceNotInitialized: {
      static jmethodID notInitConstruct = nullptr;
      if (!notInitConstruct)
        notInitConstruct =
            env->GetMethodID(canNotInitializedExCls, "<init>", "()V");
      jobject exception =
          env->NewObject(canNotInitializedExCls, notInitConstruct);
      env->Throw(static_cast<jthrowable>(exception));
      break;
    }
    default: {
      wpi::SmallString<100> buf;
      wpi::raw_svector_ostream oss(buf);
      oss << "Fatal status code detected: " << status;
      uncleanStatusExCls.Throw(env, buf.c_str());
      break;
    }
  }
}

void ThrowIllegalArgumentException(JNIEnv* env, wpi::StringRef msg) {
  illegalArgExCls.Throw(env, msg);
}

void ThrowBoundaryException(JNIEnv* env, double value, double lower,
                            double upper) {
  static jmethodID getMessage = nullptr;
  if (!getMessage)
    getMessage = env->GetStaticMethodID(boundaryExCls, "getMessage",
                                        "(DDD)Ljava/lang/String;");

  static jmethodID constructor = nullptr;
  if (!constructor)
    constructor =
        env->GetMethodID(boundaryExCls, "<init>", "(Ljava/lang/String;)V");

  jobject msg = env->CallStaticObjectMethod(
      boundaryExCls, getMessage, static_cast<jdouble>(value),
      static_cast<jdouble>(lower), static_cast<jdouble>(upper));
  jobject ex = env->NewObject(boundaryExCls, constructor, msg);
  env->Throw(static_cast<jthrowable>(ex));
}

jobject CreatePWMConfigDataResult(JNIEnv* env, int32_t maxPwm,
                                  int32_t deadbandMaxPwm, int32_t centerPwm,
                                  int32_t deadbandMinPwm, int32_t minPwm) {
  static jmethodID constructor =
      env->GetMethodID(pwmConfigDataResultCls, "<init>", "(IIIII)V");
  return env->NewObject(pwmConfigDataResultCls, constructor, maxPwm,
                        deadbandMaxPwm, centerPwm, deadbandMinPwm, minPwm);
}

void SetCanStatusObject(JNIEnv* env, jobject canStatus,
                        float percentBusUtilization, uint32_t busOffCount,
                        uint32_t txFullCount, uint32_t receiveErrorCount,
                        uint32_t transmitErrorCount) {
  static jmethodID func =
      env->GetMethodID(canStatusCls, "setStatus", "(DIIII)V");
  env->CallVoidMethod(canStatus, func, (jdouble)percentBusUtilization,
                      (jint)busOffCount, (jint)txFullCount,
                      (jint)receiveErrorCount, (jint)transmitErrorCount);
}

void SetMatchInfoObject(JNIEnv* env, jobject matchStatus,
                        const HAL_MatchInfo& matchInfo) {
  static jmethodID func =
      env->GetMethodID(matchInfoDataCls, "setData",
                       "(Ljava/lang/String;Ljava/lang/String;III)V");

  env->CallVoidMethod(
      matchStatus, func, MakeJString(env, matchInfo.eventName),
      MakeJString(env, wpi::StringRef{reinterpret_cast<const char*>(
                                          matchInfo.gameSpecificMessage),
                                      matchInfo.gameSpecificMessageSize}),
      (jint)matchInfo.matchNumber, (jint)matchInfo.replayNumber,
      (jint)matchInfo.matchType);
}

void SetAccumulatorResultObject(JNIEnv* env, jobject accumulatorResult,
                                int64_t value, int64_t count) {
  static jmethodID func =
      env->GetMethodID(accumulatorResultCls, "set", "(JJ)V");

  env->CallVoidMethod(accumulatorResult, func, (jlong)value, (jlong)count);
}

jbyteArray SetCANDataObject(JNIEnv* env, jobject canData, int32_t length,
                            uint64_t timestamp) {
  static jmethodID func = env->GetMethodID(canDataCls, "setData", "(IJ)[B");

  jbyteArray retVal = static_cast<jbyteArray>(
      env->CallObjectMethod(canData, func, (jint)length, (jlong)timestamp));
  return retVal;
}

JavaVM* GetJVM() { return jvm; }

}  // namespace frc

namespace sim {
jint SimOnLoad(JavaVM* vm, void* reserved);
void SimOnUnload(JavaVM* vm, void* reserved);
}  // namespace sim

using namespace frc;

extern "C" {

//
// indicate JNI version support desired and load classes
//
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  jvm = vm;

  // set our logging level
  Log::ReportingLevel() = logDEBUG;

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  for (auto& c : classes) {
    *c.cls = JClass(env, c.name);
    if (!*c.cls) return JNI_ERR;
  }

  for (auto& c : exceptions) {
    *c.cls = JException(env, c.name);
    if (!*c.cls) return JNI_ERR;
  }

  return sim::SimOnLoad(vm, reserved);
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
  sim::SimOnUnload(vm, reserved);

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    return;
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
 * Class:     edu_wpi_first_hal_HALUtil
 * Method:    getFPGAVersion
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_hal_HALUtil_getFPGAVersion
  (JNIEnv* env, jclass)
{
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGAVersion";
  int32_t status = 0;
  jshort returnValue = HAL_GetFPGAVersion(&status);
  HALUTIL_LOG(logDEBUG) << "Status = " << status;
  HALUTIL_LOG(logDEBUG) << "FPGAVersion = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_HALUtil
 * Method:    getFPGARevision
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HALUtil_getFPGARevision
  (JNIEnv* env, jclass)
{
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGARevision";
  int32_t status = 0;
  jint returnValue = HAL_GetFPGARevision(&status);
  HALUTIL_LOG(logDEBUG) << "Status = " << status;
  HALUTIL_LOG(logDEBUG) << "FPGARevision = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_HALUtil
 * Method:    getFPGATime
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_HALUtil_getFPGATime
  (JNIEnv* env, jclass)
{
  // HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGATime";
  int32_t status = 0;
  jlong returnValue = HAL_GetFPGATime(&status);
  // HALUTIL_LOG(logDEBUG) << "Status = " << status;
  // HALUTIL_LOG(logDEBUG) << "FPGATime = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_HALUtil
 * Method:    getHALRuntimeType
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HALUtil_getHALRuntimeType
  (JNIEnv* env, jclass)
{
  // HALUTIL_LOG(logDEBUG) << "Calling HALUtil getHALRuntimeType";
  jint returnValue = HAL_GetRuntimeType();
  // HALUTIL_LOG(logDEBUG) << "RuntimeType = " << returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_HALUtil
 * Method:    getFPGAButton
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HALUtil_getFPGAButton
  (JNIEnv* env, jclass)
{
  // HALUTIL_LOG(logDEBUG) << "Calling HALUtil getFPGATime";
  int32_t status = 0;
  jboolean returnValue = HAL_GetFPGAButton(&status);
  // HALUTIL_LOG(logDEBUG) << "Status = " << status;
  // HALUTIL_LOG(logDEBUG) << "FPGATime = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_HALUtil
 * Method:    getHALErrorMessage
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_HALUtil_getHALErrorMessage
  (JNIEnv* paramEnv, jclass, jint paramId)
{
  const char* msg = HAL_GetErrorMessage(paramId);
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil HAL_GetErrorMessage id=" << paramId
                        << " msg=" << msg;
  return MakeJString(paramEnv, msg);
}

/*
 * Class:     edu_wpi_first_hal_HALUtil
 * Method:    getHALErrno
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HALUtil_getHALErrno
  (JNIEnv*, jclass)
{
  return errno;
}

/*
 * Class:     edu_wpi_first_hal_HALUtil
 * Method:    getHALstrerror
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_hal_HALUtil_getHALstrerror
  (JNIEnv* env, jclass, jint errorCode)
{
  const char* msg = std::strerror(errno);
  HALUTIL_LOG(logDEBUG) << "Calling HALUtil getHALstrerror errorCode="
                        << errorCode << " msg=" << msg;
  return MakeJString(env, msg);
}

}  // extern "C"
