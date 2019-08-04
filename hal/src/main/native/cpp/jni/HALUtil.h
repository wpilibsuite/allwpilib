/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef HAL_HAL_SRC_MAIN_NATIVE_CPP_JNI_HALUTIL_H_
#define HAL_HAL_SRC_MAIN_NATIVE_CPP_JNI_HALUTIL_H_

#include <jni.h>
#include <stdint.h>

#include <wpi/StringRef.h>

struct HAL_MatchInfo;

namespace frc {

void ReportError(JNIEnv* env, int32_t status, bool doThrow = true);

void ThrowError(JNIEnv* env, int32_t status, int32_t minRange, int32_t maxRange,
                int32_t requestedValue);

inline bool CheckStatus(JNIEnv* env, int32_t status, bool doThrow = true) {
  if (status != 0) ReportError(env, status, doThrow);
  return status == 0;
}

inline bool CheckStatusRange(JNIEnv* env, int32_t status, int32_t minRange,
                             int32_t maxRange, int32_t requestedValue) {
  if (status != 0) ThrowError(env, status, minRange, maxRange, requestedValue);
  return status == 0;
}

inline bool CheckStatusForceThrow(JNIEnv* env, int32_t status) {
  if (status != 0) ThrowError(env, status, 0, 0, 0);
  return status == 0;
}

void ReportCANError(JNIEnv* env, int32_t status, int32_t message_id);

inline bool CheckCANStatus(JNIEnv* env, int32_t status, int32_t message_id) {
  if (status != 0) ReportCANError(env, status, message_id);
  return status == 0;
}

void ThrowIllegalArgumentException(JNIEnv* env, wpi::StringRef msg);
void ThrowBoundaryException(JNIEnv* env, double value, double lower,
                            double upper);

jobject CreatePWMConfigDataResult(JNIEnv* env, int32_t maxPwm,
                                  int32_t deadbandMaxPwm, int32_t centerPwm,
                                  int32_t deadbandMinPwm, int32_t minPwm);

void SetCanStatusObject(JNIEnv* env, jobject canStatus,
                        float percentBusUtilization, uint32_t busOffCount,
                        uint32_t txFullCount, uint32_t receiveErrorCount,
                        uint32_t transmitErrorCount);

void SetMatchInfoObject(JNIEnv* env, jobject matchStatus,
                        const HAL_MatchInfo& matchInfo);

void SetAccumulatorResultObject(JNIEnv* env, jobject accumulatorResult,
                                int64_t value, int64_t count);

jbyteArray SetCANDataObject(JNIEnv* env, jobject canData, int32_t length,
                            uint64_t timestamp);

JavaVM* GetJVM();

}  // namespace frc

#endif  // HAL_HAL_SRC_MAIN_NATIVE_CPP_JNI_HALUTIL_H_
