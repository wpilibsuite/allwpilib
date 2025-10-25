// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

/* Header for class MrCalJNI */

#ifndef WPILIB_THIRDPARTY_MRCAL_SRC_MAIN_NATIVE_INCLUDE_JNI_MRCAL_JNI_H_
#define WPILIB_THIRDPARTY_MRCAL_SRC_MAIN_NATIVE_INCLUDE_JNI_MRCAL_JNI_H_
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     MrCalJNI
 * Method:    mrcal_calibrate_camera
 * Signature: ([D[DDDDDD)LMrCalJNI/CalResult;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_mrcal_MrCalJNI_mrcal_1calibrate_1camera(JNIEnv*, jclass,
                                                           jdoubleArray, jint,
                                                           jint, jdouble, jint,
                                                           jint, jdouble);

/*
 * Class:     edu_wpi_first_mrcal_MrCalJNI
 * Method:    undistort_mrcal
 * Signature: (JJJJI)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_mrcal_MrCalJNI_undistort_1mrcal(
    JNIEnv*, jclass, jlong, jlong, jlong, jlong, jint, jint, jint, jint, jint);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // WPILIB_THIRDPARTY_MRCAL_SRC_MAIN_NATIVE_INCLUDE_JNI_MRCAL_JNI_H_
