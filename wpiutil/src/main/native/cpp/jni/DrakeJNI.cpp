/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <Eigen/Core>

#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "edu_wpi_first_wpiutil_math_DrakeJNI.h"
#include "wpi/jni_util.h"

using namespace wpi::java;

extern "C" {

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
  Eigen::Map<Eigen::MatrixXd> Amat{env->GetDoubleArrayElements(A, nullptr),
                                   states, states};
  Eigen::Map<Eigen::MatrixXd> Bmat{env->GetDoubleArrayElements(B, nullptr),
                                   states, inputs};
  Eigen::Map<Eigen::MatrixXd> Qmat{env->GetDoubleArrayElements(Q, nullptr),
                                   states, states};
  Eigen::Map<Eigen::MatrixXd> Rmat{env->GetDoubleArrayElements(R, nullptr),
                                   inputs, inputs};
  Eigen::MatrixXd result =
      drake::math::DiscreteAlgebraicRiccatiEquation(Amat, Bmat, Qmat, Rmat);

  env->SetDoubleArrayRegion(S, 0, states * states, result.data());
}

}  // extern "C"
