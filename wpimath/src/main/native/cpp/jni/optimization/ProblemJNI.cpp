// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <vector>

#include <sleipnir/optimization/problem.hpp>
#include <wpi/jni_util.h>

#include "../SleipnirJNIUtil.h"
#include "edu_wpi_first_math_optimization_ProblemJNI.h"

using namespace wpi::java;

extern "C" {

namespace {

// ProblemJNI_solve() sets these before calling Problem::solve() so the Java
// callback has a valid JNIEnv and object on which to call
// Problem.runCallbacks()
thread_local JNIEnv* callbackEnv;
thread_local jobject callbackObj;

}  // namespace

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_create
  (JNIEnv* env, jclass)
{
  auto problem = new slp::Problem;

  // Configure Java iteration callbacks
  problem->add_persistent_callback([](const slp::IterationInfo& info) -> bool {
    // Find Problem class
    static wpi::java::JClass cls{callbackEnv,
                                 "edu/wpi/first/math/optimization/Problem"};
    if (!cls) {
      return true;
    }

    // Find Problem.runCallbacks()
    static jmethodID runCallbacks = callbackEnv->GetMethodID(
        cls, "runCallbacks",
        "(III[DLedu/wpi/first/math/autodiff/NativeSparseTriplets;"
        "Ledu/wpi/first/math/autodiff/NativeSparseTriplets;"
        "Ledu/wpi/first/math/autodiff/NativeSparseTriplets;"
        "Ledu/wpi/first/math/autodiff/NativeSparseTriplets;)Z");
    if (!runCallbacks) {
      return true;
    }

    // Run Java callbacks
    return callbackEnv->CallBooleanMethod(
        callbackObj, runCallbacks, info.A_e.rows(), info.A_i.rows(),
        info.iteration, MakeJDoubleArray(callbackEnv, info.x),
        frc::detail::GetTriplets(callbackEnv, info.g),
        frc::detail::GetTriplets(callbackEnv, info.H),
        frc::detail::GetTriplets(callbackEnv, info.A_e),
        frc::detail::GetTriplets(callbackEnv, info.A_i));
  });

  return reinterpret_cast<jlong>(problem);
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    destroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_destroy
  (JNIEnv* env, jclass, jlong handle)
{
  delete reinterpret_cast<slp::Problem*>(handle);
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    decisionVariable
 * Signature: (JII)[J
 */
JNIEXPORT jlongArray JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_decisionVariable
  (JNIEnv* env, jclass, jlong handle, jint rows, jint cols)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  auto vars = problem.decision_variable(rows, cols);

  std::vector<jlong> varHandles;
  varHandles.reserve(vars.size());
  for (auto& var : vars) {
    varHandles.emplace_back(reinterpret_cast<jlong>(new slp::Variable{var}));
  }
  return MakeJLongArray(env, varHandles);
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    symmetricDecisionVariable
 * Signature: (JI)[J
 */
JNIEXPORT jlongArray JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_symmetricDecisionVariable
  (JNIEnv* env, jclass, jlong handle, jint rows)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  auto vars = problem.symmetric_decision_variable(rows);

  std::vector<jlong> varHandles;
  varHandles.reserve(vars.size());
  for (auto& var : vars) {
    varHandles.emplace_back(reinterpret_cast<jlong>(new slp::Variable{var}));
  }
  return MakeJLongArray(env, varHandles);
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    minimize
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_minimize
  (JNIEnv* env, jclass, jlong handle, jlong costHandle)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  auto& costVar = *reinterpret_cast<slp::Variable*>(costHandle);
  problem.minimize(costVar);
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    maximize
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_maximize
  (JNIEnv* env, jclass, jlong handle, jlong objectiveHandle)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  auto& objectiveVar = *reinterpret_cast<slp::Variable*>(objectiveHandle);
  problem.maximize(objectiveVar);
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    subjectToEq
 * Signature: (J[J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_subjectToEq
  (JNIEnv* env, jclass, jlong handle, jlongArray constraintHandles)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  JSpan<const jlong> constraintHandlesSpan{env, constraintHandles};

  for (const auto& constraintHandle : constraintHandlesSpan) {
    const auto& constraint =
        *reinterpret_cast<slp::Variable*>(constraintHandle);
    problem.subject_to(constraint == 0.0);
  }
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    subjectToIneq
 * Signature: (J[J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_subjectToIneq
  (JNIEnv* env, jclass, jlong handle, jlongArray constraintHandles)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  JSpan<const jlong> constraintHandlesSpan{env, constraintHandles};

  for (const auto& constraintHandle : constraintHandlesSpan) {
    const auto& constraint =
        *reinterpret_cast<slp::Variable*>(constraintHandle);
    problem.subject_to(constraint >= 0.0);
  }
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    costFunctionType
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_costFunctionType
  (JNIEnv* env, jclass, jlong handle)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  return static_cast<jint>(problem.cost_function_type());
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    equalityConstraintType
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_equalityConstraintType
  (JNIEnv* env, jclass, jlong handle)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  return static_cast<jint>(problem.equality_constraint_type());
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    inequalityConstraintType
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_inequalityConstraintType
  (JNIEnv* env, jclass, jlong handle)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);
  return static_cast<jint>(problem.inequality_constraint_type());
}

/*
 * Class:     edu_wpi_first_math_optimization_ProblemJNI
 * Method:    solve
 * Signature: (Ljava/lang/Object;JDIDZZZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_math_optimization_ProblemJNI_solve
  (JNIEnv* env, jclass, jobject obj, jlong handle, jdouble tolerance,
   jint maxIterations, jdouble timeout, jboolean feasibleIPM,
   jboolean diagnostics, jboolean spy)
{
  auto& problem = *reinterpret_cast<slp::Problem*>(handle);

  callbackEnv = env;
  callbackObj = obj;

  slp::Options options{
      tolerance, maxIterations, std::chrono::duration<double>{timeout},
      static_cast<bool>(feasibleIPM), static_cast<bool>(diagnostics)};
  return static_cast<int>(problem.solve(options, spy));
}

}  // extern "C"
