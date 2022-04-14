// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "edu_wpi_first_util_datalog_DataLogJNI.h"
#include "wpi/DataLog.h"
#include "wpi/jni_util.h"

using namespace wpi::java;
using namespace wpi::log;

extern "C" {

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    create
 * Signature: (Ljava/lang/String;Ljava/lang/String;DLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_create
  (JNIEnv* env, jclass, jstring dir, jstring filename, jdouble period,
   jstring extraHeader)
{
  return reinterpret_cast<jlong>(new DataLog{JStringRef{env, dir},
                                             JStringRef{env, filename}, period,
                                             JStringRef{env, extraHeader}});
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    setFilename
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_setFilename
  (JNIEnv* env, jclass, jlong impl, jstring filename)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->SetFilename(JStringRef{env, filename});
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    flush
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_flush
  (JNIEnv*, jclass, jlong impl)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->Flush();
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    pause
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_pause
  (JNIEnv*, jclass, jlong impl)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->Pause();
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    resume
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_resume
  (JNIEnv*, jclass, jlong impl)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->Resume();
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    start
 * Signature: (JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_start
  (JNIEnv* env, jclass, jlong impl, jstring name, jstring type,
   jstring metadata, jlong timestamp)
{
  if (impl == 0) {
    return 0;
  }
  return reinterpret_cast<DataLog*>(impl)->Start(
      JStringRef{env, name}, JStringRef{env, type}, JStringRef{env, metadata},
      timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    finish
 * Signature: (JIJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_finish
  (JNIEnv*, jclass, jlong impl, jint entry, jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->Finish(entry, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    setMetadata
 * Signature: (JILjava/lang/String;J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_setMetadata
  (JNIEnv* env, jclass, jlong impl, jint entry, jstring metadata,
   jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->SetMetadata(
      entry, JStringRef{env, metadata}, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_close
  (JNIEnv*, jclass, jlong impl)
{
  delete reinterpret_cast<DataLog*>(impl);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendRaw
 * Signature: (JI[BJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendRaw
  (JNIEnv* env, jclass, jlong impl, jint entry, jbyteArray value,
   jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  JByteArrayRef cvalue{env, value};
  reinterpret_cast<DataLog*>(impl)->AppendRaw(
      entry,
      {reinterpret_cast<const uint8_t*>(cvalue.array().data()), cvalue.size()},
      timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendBoolean
 * Signature: (JIZJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendBoolean
  (JNIEnv*, jclass, jlong impl, jint entry, jboolean value, jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->AppendBoolean(entry, value, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendInteger
 * Signature: (JIJJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendInteger
  (JNIEnv*, jclass, jlong impl, jint entry, jlong value, jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->AppendInteger(entry, value, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendFloat
 * Signature: (JIFJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendFloat
  (JNIEnv*, jclass, jlong impl, jint entry, jfloat value, jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->AppendFloat(entry, value, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendDouble
 * Signature: (JIDJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendDouble
  (JNIEnv*, jclass, jlong impl, jint entry, jdouble value, jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->AppendDouble(entry, value, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendString
 * Signature: (JILjava/lang/String;J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendString
  (JNIEnv* env, jclass, jlong impl, jint entry, jstring value, jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->AppendString(entry, JStringRef{env, value},
                                                 timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendBooleanArray
 * Signature: (JI[ZJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendBooleanArray
  (JNIEnv* env, jclass, jlong impl, jint entry, jbooleanArray value,
   jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->AppendBooleanArray(
      entry, JBooleanArrayRef{env, value}, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendIntegerArray
 * Signature: (JI[JJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendIntegerArray
  (JNIEnv* env, jclass, jlong impl, jint entry, jlongArray value,
   jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  JLongArrayRef jarr{env, value};
  if constexpr (sizeof(jlong) == sizeof(int64_t)) {
    reinterpret_cast<DataLog*>(impl)->AppendIntegerArray(
        entry,
        {reinterpret_cast<const int64_t*>(jarr.array().data()),
         jarr.array().size()},
        timestamp);
  } else {
    wpi::SmallVector<int64_t, 16> arr;
    arr.reserve(jarr.size());
    for (auto v : jarr.array()) {
      arr.push_back(v);
    }
    reinterpret_cast<DataLog*>(impl)->AppendIntegerArray(entry, arr, timestamp);
  }
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendFloatArray
 * Signature: (JI[FJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendFloatArray
  (JNIEnv* env, jclass, jlong impl, jint entry, jfloatArray value,
   jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->AppendFloatArray(
      entry, JFloatArrayRef{env, value}, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendDoubleArray
 * Signature: (JI[DJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendDoubleArray
  (JNIEnv* env, jclass, jlong impl, jint entry, jdoubleArray value,
   jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  reinterpret_cast<DataLog*>(impl)->AppendDoubleArray(
      entry, JDoubleArrayRef{env, value}, timestamp);
}

/*
 * Class:     edu_wpi_first_util_datalog_DataLogJNI
 * Method:    appendStringArray
 * Signature: (JI[Ljava/lang/Object;J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_util_datalog_DataLogJNI_appendStringArray
  (JNIEnv* env, jclass, jlong impl, jint entry, jobjectArray value,
   jlong timestamp)
{
  if (impl == 0) {
    return;
  }
  size_t len = env->GetArrayLength(value);
  std::vector<std::string> arr;
  arr.reserve(len);
  for (size_t i = 0; i < len; ++i) {
    JLocal<jstring> elem{
        env, static_cast<jstring>(env->GetObjectArrayElement(value, i))};
    if (!elem) {
      return;
    }
    arr.emplace_back(JStringRef{env, elem}.str());
  }
  reinterpret_cast<DataLog*>(impl)->AppendStringArray(entry, arr, timestamp);
}

}  // extern "C"
