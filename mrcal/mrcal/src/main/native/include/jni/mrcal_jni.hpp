// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

/*
 * Copyright (C) Photon Vision.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* DO NOT EDIT THIS std::FILE - it is machine generated */
#include <jni.h>

/* Header for class MrCalJNI */

#ifndef ALLWPILIB_MRCAL_MRCAL_SRC_MAIN_NATIVE_INCLUDE_JNI_MRCAL_JNI_HPP_
#define ALLWPILIB_MRCAL_MRCAL_SRC_MAIN_NATIVE_INCLUDE_JNI_MRCAL_JNI_HPP_
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     MrCalJNI
 * Method:    mrcal_calibrate_camera
 * Signature: ([D[DDDDDD)LMrCalJNI/CalResult;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_mrcal_MrCalJNI_mrcal_1calibrate_1camera(JNIEnv*, jclass,
                                                        jdoubleArray, jint,
                                                        jint, jdouble, jint,
                                                        jint, jdouble);

/*
 * Class:     org_wpilib_mrcal_MrCalJNI
 * Method:    undistort_mrcal
 * Signature: (JJJJI)Z
 */
JNIEXPORT jboolean JNICALL Java_org_wpilib_mrcal_MrCalJNI_undistort_1mrcal(
    JNIEnv*, jclass, jlong, jlong, jlong, jlong, jint, jint, jint, jint, jint);

/*
 * Class:     org_wpilib_mrcal_MrCalJNI
 * Method:    compute_uncertainty
 * Signature: ([D[D[DIIDIIII)[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_org_wpilib_mrcal_MrCalJNI_compute_1uncertainty(JNIEnv*, jclass,
                                                    jdoubleArray, jdoubleArray,
                                                    jdoubleArray, jint, jint,
                                                    jdouble, jint, jint, jint,
                                                    jint, jdouble, jdouble);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // ALLWPILIB_MRCAL_MRCAL_SRC_MAIN_NATIVE_INCLUDE_JNI_MRCAL_JNI_HPP_
