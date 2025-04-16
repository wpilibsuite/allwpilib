// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef HAL_HAL_SRC_MAIN_NATIVE_CPP_JNI_HALUTIL_H_
#define HAL_HAL_SRC_MAIN_NATIVE_CPP_JNI_HALUTIL_H_

#include <jni.h>
#include <stdint.h>

#include <string_view>

struct HAL_MatchInfo;
struct HAL_Value;

namespace hal {

void ReportError(JNIEnv* env, int32_t status, bool doThrow = true);

void ThrowError(JNIEnv* env, int32_t status, int32_t minRange, int32_t maxRange,
                int32_t requestedValue);

inline bool CheckStatus(JNIEnv* env, int32_t status, bool doThrow = true) {
  if (status != 0) {
    ReportError(env, status, doThrow);
  }
  return status == 0;
}

inline bool CheckStatusRange(JNIEnv* env, int32_t status, int32_t minRange,
                             int32_t maxRange, int32_t requestedValue) {
  if (status != 0) {
    ThrowError(env, status, minRange, maxRange, requestedValue);
  }
  return status == 0;
}

inline bool CheckStatusForceThrow(JNIEnv* env, int32_t status) {
  if (status != 0) {
    ThrowError(env, status, 0, 0, 0);
  }
  return status == 0;
}

void ThrowNullPointerException(JNIEnv* env, std::string_view msg);
void ThrowCANStreamOverflowException(JNIEnv* env, jobjectArray messages,
                                     jint length);
void ThrowIllegalArgumentException(JNIEnv* env, std::string_view msg);
void ThrowBoundaryException(JNIEnv* env, double value, double lower,
                            double upper);

jobject CreatePWMConfigDataResult(JNIEnv* env, int32_t maxPwm,
                                  int32_t deadbandMaxPwm, int32_t centerPwm,
                                  int32_t deadbandMinPwm, int32_t minPwm);

jobject CreateREVPHVersion(JNIEnv* env, uint32_t firmwareMajor,
                           uint32_t firmwareMinor, uint32_t firmwareFix,
                           uint32_t hardwareMinor, uint32_t hardwareMajor,
                           uint32_t uniqueId);

void SetCanStatusObject(JNIEnv* env, jobject canStatus,
                        float percentBusUtilization, uint32_t busOffCount,
                        uint32_t txFullCount, uint32_t receiveErrorCount,
                        uint32_t transmitErrorCount);

void SetMatchInfoObject(JNIEnv* env, jobject matchStatus,
                        const HAL_MatchInfo& matchInfo);

jbyteArray SetCANReceiveMessageObject(JNIEnv* env, jobject canData,
                                      int32_t length, int32_t flags,
                                      uint64_t timestamp);

jbyteArray SetCANStreamObject(JNIEnv* env, jobject canStreamData,
                              int32_t length, uint32_t messageId,
                              uint64_t timestamp);

jobject CreateHALValue(JNIEnv* env, const HAL_Value& value);

jobject CreatePowerDistributionVersion(JNIEnv* env, uint32_t firmwareMajor,
                                       uint32_t firmwareMinor,
                                       uint32_t firmwareFix,
                                       uint32_t hardwareMinor,
                                       uint32_t hardwareMajor,
                                       uint32_t uniqueId);

jobject CreateCANStreamMessage(JNIEnv* env);

JavaVM* GetJVM();

}  // namespace hal

#endif  // HAL_HAL_SRC_MAIN_NATIVE_CPP_JNI_HALUTIL_H_
