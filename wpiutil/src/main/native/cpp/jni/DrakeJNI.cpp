/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <Eigen/Core>

#include <iostream>

#include <unsupported/Eigen/MatrixFunctions>

#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "edu_wpi_first_wpiutil_math_DrakeJNI.h"
#include "wpi/jni_util.h"

using namespace wpi::java;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    // Check to ensure the JNI version is valid

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;

    // In here is also where you store things like class references
    // if they are ever needed

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {}

/*
 * Class:     edu_wpi_first_wpiutil_math_DrakeJNI
 * Method:    discreteAlgebraicRiccatiEquation
 * Signature: ([D[D[D[DII[D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpiutil_math_DrakeJNI_discreteAlgebraicRiccatiEquation
  (JNIEnv* env, jclass, jdoubleArray A, jdoubleArray B, jdoubleArray Q,
   jdoubleArray R, jint states, jint inputs, jdoubleArray S)
{

  jdouble* nativeA = env->GetDoubleArrayElements(A, nullptr);
  jdouble* nativeB = env->GetDoubleArrayElements(B, nullptr);
  jdouble* nativeQ = env->GetDoubleArrayElements(Q, nullptr);
  jdouble* nativeR = env->GetDoubleArrayElements(R, nullptr);

  Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                  Eigen::RowMajor>> Amat{nativeA,
                                  states, states};
  Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                  Eigen::RowMajor>> Bmat{nativeB,                                
                                  states, inputs};
  Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                  Eigen::RowMajor>> Qmat{nativeQ,                                
                                  states, states};
  Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                                  Eigen::RowMajor>> Rmat{nativeR,                                
                                  inputs, inputs};

  Eigen::MatrixXd result =
      drake::math::DiscreteAlgebraicRiccatiEquation(Amat, Bmat, Qmat, Rmat);

  env->ReleaseDoubleArrayElements(A, nativeA, 0);
  env->ReleaseDoubleArrayElements(B, nativeB, 0);
  env->ReleaseDoubleArrayElements(Q, nativeQ, 0);
  env->ReleaseDoubleArrayElements(R, nativeR, 0);

  env->SetDoubleArrayRegion(S, 0, states * states, result.data());
}

}  // extern "C"
