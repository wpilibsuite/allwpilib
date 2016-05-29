/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef HALUTIL_H
#define HALUTIL_H

#include <stdint.h>

#include <jni.h>

extern JavaVM* jvm;

void ReportError(JNIEnv* env, int32_t status, bool do_throw = true);

inline bool CheckStatus(JNIEnv* env, int32_t status, bool do_throw = true) {
  if (status != 0) ReportError(env, status, do_throw);
  return status == 0;
}

void ReportCANError(JNIEnv* env, int32_t status, int message_id);

inline bool CheckCANStatus(JNIEnv* env, int32_t status, int message_id) {
  if (status != 0) ReportCANError(env, status, message_id);
  return status == 0;
}

void ThrowIllegalArgumentException(JNIEnv* env, const char* msg);
void ThrowBoundaryException(JNIEnv* env, double value, double lower,
                            double upper);

#endif  // HALUTIL_H
