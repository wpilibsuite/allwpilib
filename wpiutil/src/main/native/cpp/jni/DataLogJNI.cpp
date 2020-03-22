/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "edu_wpi_first_wpiutil_datalog_DataLogJNI.h"
#include "wpi/DataLog.h"
#include "wpi/jni_util.h"

using namespace wpi::java;
using namespace wpi::log;

extern "C" {

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    getDataType
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_getDataType
  (JNIEnv* env, jclass, jlong impl)
{
  return MakeJString(env,
                     reinterpret_cast<const DataLogImpl*>(impl)->m_dataType);
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    getDataLayout
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_getDataLayout
  (JNIEnv*, jclass, jlong)
{
  return nullptr;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    getRecordSize
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_getRecordSize
  (JNIEnv*, jclass, jlong)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    isFixedSize
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_isFixedSize
  (JNIEnv*, jclass, jlong)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    flush
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_flush
  (JNIEnv*, jclass, jlong)
{
  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    getSize
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_getSize
  (JNIEnv*, jclass, jlong)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    find
 * Signature: (JJI)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_find__JJI
  (JNIEnv*, jclass, jlong, jlong, jint)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    find
 * Signature: (JJII)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_find__JJII
  (JNIEnv*, jclass, jlong, jlong, jint, jint)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    check
 * Signature: (JLjava/lang/String;IZZZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_check
  (JNIEnv*, jclass, jlong, jstring, jint, jboolean, jboolean, jboolean)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_close
  (JNIEnv*, jclass, jlong)
{
  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    open
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IILjava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_open
  (JNIEnv*, jclass, jstring, jstring, jstring, jint, jint, jobject)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    openRaw
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_openRaw
  (JNIEnv*, jclass, jstring, jobject)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendRaw
 * Signature: (J[B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendRaw
  (JNIEnv*, jclass, jlong, jbyteArray)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendRawTime
 * Signature: (JJ[B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendRawTime
  (JNIEnv*, jclass, jlong, jlong, jbyteArray)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    readRaw
 * Signature: (JI)[B
 */
JNIEXPORT jbyteArray JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_readRaw
  (JNIEnv*, jclass, jlong, jint)
{
  return nullptr;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    openBoolean
 * Signature: (Ljava/lang/String;ILjava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_openBoolean
  (JNIEnv*, jclass, jstring, jint, jobject)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendBoolean
 * Signature: (JZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendBoolean
  (JNIEnv*, jclass, jlong, jboolean)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendBooleanTime
 * Signature: (JJZ)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendBooleanTime
  (JNIEnv*, jclass, jlong, jlong, jboolean)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    openDouble
 * Signature: (Ljava/lang/String;ILjava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_openDouble
  (JNIEnv*, jclass, jstring, jint, jobject)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendDouble
 * Signature: (JD)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendDouble
  (JNIEnv*, jclass, jlong, jdouble)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendDoubleTime
 * Signature: (JJD)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendDoubleTime
  (JNIEnv*, jclass, jlong, jlong, jdouble)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    openString
 * Signature: (Ljava/lang/String;ILjava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_openString
  (JNIEnv*, jclass, jstring, jint, jobject)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendString
 * Signature: (JLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendString
  (JNIEnv*, jclass, jlong, jstring)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendStringTime
 * Signature: (JJLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendStringTime
  (JNIEnv*, jclass, jlong, jlong, jstring)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    openBooleanArray
 * Signature: (Ljava/lang/String;ILjava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_openBooleanArray
  (JNIEnv*, jclass, jstring, jint, jobject)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendBooleanArray
 * Signature: (J[Z)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendBooleanArray
  (JNIEnv*, jclass, jlong, jbooleanArray)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendBooleanArrayTime
 * Signature: (JJ[Z)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendBooleanArrayTime
  (JNIEnv*, jclass, jlong, jlong, jbooleanArray)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    openDoubleArray
 * Signature: (Ljava/lang/String;ILjava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_openDoubleArray
  (JNIEnv*, jclass, jstring, jint, jobject)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendDoubleArray
 * Signature: (J[D)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendDoubleArray
  (JNIEnv*, jclass, jlong, jdoubleArray)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendDoubleArrayTime
 * Signature: (JJ[D)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendDoubleArrayTime
  (JNIEnv*, jclass, jlong, jlong, jdoubleArray)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    openStringArray
 * Signature: (Ljava/lang/String;ILjava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_openStringArray
  (JNIEnv*, jclass, jstring, jint, jobject)
{
  return 0;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendStringArray
 * Signature: (J[Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendStringArray
  (JNIEnv*, jclass, jlong, jobjectArray)
{
  return JNI_FALSE;  // TODO
}

/*
 * Class:     edu_wpi_first_wpiutil_datalog_DataLogJNI
 * Method:    appendStringArrayTime
 * Signature: (JJ[Ljava/lang/Object;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpiutil_datalog_DataLogJNI_appendStringArrayTime
  (JNIEnv*, jclass, jlong, jlong, jobjectArray)
{
  return JNI_FALSE;  // TODO
}

}  // extern "C"
