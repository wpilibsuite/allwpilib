/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>
#include <assert.h>

#include "edu_wpi_first_wpilibj_hal_CanTalonJNI.h"

#include "HAL/CanTalonSRX.h"

#include "HALUtil.h"

extern "C" {

inline bool CheckCTRStatus(JNIEnv *env, CTR_Code status) {
  if (status != CTR_OKAY) ReportError(env, (int32_t)status, 0, 0, 0, false);
  return status == CTR_OKAY;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CanTalonJNI
 * Method:    new_CanTalonSRX
 * Signature: (III)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_new_1CanTalonSRX__III
  (JNIEnv *env, jclass, jint deviceNumber, jint controlPeriodMs, jint enablePeriodMs)
{
  return (jlong)(new CanTalonSRX((int)deviceNumber, (int)controlPeriodMs, (int)enablePeriodMs));
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CanTalonJNI
 * Method:    new_CanTalonSRX
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_new_1CanTalonSRX__II
  (JNIEnv *env, jclass, jint deviceNumber, jint controlPeriodMs)
{
  return (jlong)(new CanTalonSRX((int)deviceNumber, (int)controlPeriodMs));
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CanTalonJNI
 * Method:    new_CanTalonSRX
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_new_1CanTalonSRX__I
  (JNIEnv *env, jclass, jint deviceNumber)
{
  return (jlong)(new CanTalonSRX((int)deviceNumber));
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CanTalonJNI
 * Method:    new_CanTalonSRX
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_new_1CanTalonSRX__
  (JNIEnv *env, jclass)
{
  return (jlong)(new CanTalonSRX);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CanTalonJNI
 * Method:    delete_CanTalonSRX
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_delete_1CanTalonSRX
  (JNIEnv *env, jclass, jlong handle)
{
  delete (CanTalonSRX*)handle;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CanTalonJNI
 * Method:    GetMotionProfileStatus
 * Signature: (JLedu/wpi/first/wpilibj/CANTalon;Ledu/wpi/first/wpilibj/CANTalon/MotionProfileStatus;)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetMotionProfileStatus
  (JNIEnv *env, jclass, jlong handle, jobject canTalon, jobject motionProfileStatus)
{
  static jmethodID setMotionProfileStatusFromJNI = nullptr;
  if (!setMotionProfileStatusFromJNI) {
    jclass cls = env->GetObjectClass(canTalon);
    setMotionProfileStatusFromJNI = env->GetMethodID(cls, "setMotionProfileStatusFromJNI", "(Ledu/wpi/first/wpilibj/CANTalon$MotionProfileStatus;IIIIIIII)V");
    if (!setMotionProfileStatusFromJNI) return;
  }

  uint32_t flags;
  uint32_t profileSlotSelect;
  int32_t targPos;
  int32_t targVel;
  uint32_t topBufferRem;
  uint32_t topBufferCnt;
  uint32_t btmBufferCnt;
  uint32_t outputEnable;
  CTR_Code status = ((CanTalonSRX*)handle)->GetMotionProfileStatus(flags, profileSlotSelect, targPos, targVel, topBufferRem, topBufferCnt, btmBufferCnt, outputEnable);
  if (!CheckCTRStatus(env, status)) return;

  env->CallVoidMethod(canTalon, setMotionProfileStatusFromJNI, motionProfileStatus, (jint)flags, (jint)profileSlotSelect, (jint)targPos, (jint)targVel, (jint)topBufferRem, (jint)topBufferCnt, (jint)btmBufferCnt, (jint)outputEnable);
}

JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_Set
  (JNIEnv *env, jclass, jlong handle, jdouble value)
{
  return ((CanTalonSRX*)handle)->Set((double)value);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetParam
  (JNIEnv *env, jclass, jlong handle, jint paramEnum, jdouble value)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetParam((CanTalonSRX::param_t)paramEnum, (double)value);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_RequestParam
  (JNIEnv *env, jclass, jlong handle, jint paramEnum)
{
  CTR_Code status = ((CanTalonSRX*)handle)->RequestParam((CanTalonSRX::param_t)paramEnum);
  CheckCTRStatus(env, status);
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetParamResponse
  (JNIEnv *env, jclass, jlong handle, jint paramEnum)
{
  double value;
  CTR_Code status = ((CanTalonSRX*)handle)->GetParamResponse((CanTalonSRX::param_t)paramEnum, value);
  CheckCTRStatus(env, status);
  return value;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetParamResponseInt32
  (JNIEnv *env, jclass, jlong handle, jint paramEnum)
{
  int value;
  CTR_Code status = ((CanTalonSRX*)handle)->GetParamResponseInt32((CanTalonSRX::param_t)paramEnum, value);
  CheckCTRStatus(env, status);
  return value;
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetPgain
  (JNIEnv *env, jclass, jlong handle, jint slotIdx, jdouble gain)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetPgain((unsigned)slotIdx, (double)gain);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetIgain
  (JNIEnv *env, jclass, jlong handle, jint slotIdx, jdouble gain)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetIgain((unsigned)slotIdx, (double)gain);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetDgain
  (JNIEnv *env, jclass, jlong handle, jint slotIdx, jdouble gain)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetDgain((unsigned)slotIdx, (double)gain);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetFgain
  (JNIEnv *env, jclass, jlong handle, jint slotIdx, jdouble gain)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetFgain((unsigned)slotIdx, (double)gain);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetIzone
  (JNIEnv *env, jclass, jlong handle, jint slotIdx, jint zone)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetIzone((unsigned)slotIdx, (int)zone);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetCloseLoopRampRate
  (JNIEnv *env, jclass, jlong handle, jint slotIdx, jint closeLoopRampRate)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetCloseLoopRampRate((unsigned)slotIdx, (int)closeLoopRampRate);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetVoltageCompensationRate
  (JNIEnv *env, jclass, jlong handle, jdouble voltagePerMs)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetVoltageCompensationRate((double)voltagePerMs);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetSensorPosition
  (JNIEnv *env, jclass, jlong handle, jint pos)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetSensorPosition((int)pos);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetForwardSoftLimit
  (JNIEnv *env, jclass, jlong handle, jint forwardLimit)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetForwardSoftLimit((int)forwardLimit);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetReverseSoftLimit
  (JNIEnv *env, jclass, jlong handle, jint reverseLimit)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetReverseSoftLimit((int)reverseLimit);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetForwardSoftEnable
  (JNIEnv *env, jclass, jlong handle, jint enable)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetForwardSoftEnable((int)enable);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetReverseSoftEnable
  (JNIEnv *env, jclass, jlong handle, jint enable)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetReverseSoftEnable((int)enable);
  CheckCTRStatus(env, status);
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetPgain
  (JNIEnv *env, jclass, jlong handle, jint slotIdx)
{
  double gain;
  CTR_Code status = ((CanTalonSRX*)handle)->GetPgain((unsigned)slotIdx, gain);
  CheckCTRStatus(env, status);
  return gain;
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetIgain
  (JNIEnv *env, jclass, jlong handle, jint slotIdx)
{
  double gain;
  CTR_Code status = ((CanTalonSRX*)handle)->GetIgain((unsigned)slotIdx, gain);
  CheckCTRStatus(env, status);
  return gain;
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetDgain
  (JNIEnv *env, jclass, jlong handle, jint slotIdx)
{
  double gain;
  CTR_Code status = ((CanTalonSRX*)handle)->GetDgain((unsigned)slotIdx, gain);
  CheckCTRStatus(env, status);
  return gain;
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFgain
  (JNIEnv *env, jclass, jlong handle, jint slotIdx)
{
  double gain;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFgain((unsigned)slotIdx, gain);
  CheckCTRStatus(env, status);
  return gain;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetIzone
  (JNIEnv *env, jclass, jlong handle, jint slotIdx)
{
  int zone;
  CTR_Code status = ((CanTalonSRX*)handle)->GetIzone((unsigned)slotIdx, zone);
  CheckCTRStatus(env, status);
  return zone;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetCloseLoopRampRate
  (JNIEnv *env, jclass, jlong handle, jint slotIdx)
{
  int closeLoopRampRate;
  CTR_Code status = ((CanTalonSRX*)handle)->GetCloseLoopRampRate((unsigned)slotIdx, closeLoopRampRate);
  CheckCTRStatus(env, status);
  return closeLoopRampRate;
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetVoltageCompensationRate
  (JNIEnv *env, jclass, jlong handle)
{
  double voltagePerMs;
  CTR_Code status = ((CanTalonSRX*)handle)->GetVoltageCompensationRate(voltagePerMs);
  CheckCTRStatus(env, status);
  return voltagePerMs;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetForwardSoftLimit
  (JNIEnv *env, jclass, jlong handle)
{
  int forwardLimit;
  CTR_Code status = ((CanTalonSRX*)handle)->GetForwardSoftLimit(forwardLimit);
  CheckCTRStatus(env, status);
  return forwardLimit;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetReverseSoftLimit
  (JNIEnv *env, jclass, jlong handle)
{
  int reverseLimit;
  CTR_Code status = ((CanTalonSRX*)handle)->GetReverseSoftLimit(reverseLimit);
  CheckCTRStatus(env, status);
  return reverseLimit;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetForwardSoftEnable
  (JNIEnv *env, jclass, jlong handle)
{
  int enable;
  CTR_Code status = ((CanTalonSRX*)handle)->GetForwardSoftEnable(enable);
  CheckCTRStatus(env, status);
  return enable;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetReverseSoftEnable
  (JNIEnv *env, jclass, jlong handle)
{
  int enable;
  CTR_Code status = ((CanTalonSRX*)handle)->GetReverseSoftEnable(enable);
  CheckCTRStatus(env, status);
  return enable;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetPulseWidthRiseToFallUs
  (JNIEnv *env, jclass, jlong handle)
{
  int param;
  CTR_Code status = ((CanTalonSRX*)handle)->GetPulseWidthRiseToFallUs(param);
  CheckCTRStatus(env, status);
  return param;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_IsPulseWidthSensorPresent
  (JNIEnv *env, jclass, jlong handle)
{
  int param;
  CTR_Code status = ((CanTalonSRX*)handle)->IsPulseWidthSensorPresent(param);
  CheckCTRStatus(env, status);
  return param;
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetModeSelect2
  (JNIEnv *env, jclass, jlong handle, jint modeSelect, jint demand)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetModeSelect((int)modeSelect, (int)demand);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetStatusFrameRate
  (JNIEnv *env, jclass, jlong handle, jint frameEnum, jint periodMs)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetStatusFrameRate((unsigned)frameEnum, (unsigned)periodMs);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_ClearStickyFaults
  (JNIEnv *env, jclass, jlong handle)
{
  CTR_Code status = ((CanTalonSRX*)handle)->ClearStickyFaults();
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_ChangeMotionControlFramePeriod
  (JNIEnv *env, jclass, jlong handle, jint periodMs)
{
  return ((CanTalonSRX*)handle)->ChangeMotionControlFramePeriod((uint32_t)periodMs);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_ClearMotionProfileTrajectories
  (JNIEnv *env, jclass, jlong handle)
{
  return ((CanTalonSRX*)handle)->ClearMotionProfileTrajectories();
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetMotionProfileTopLevelBufferCount
  (JNIEnv *env, jclass, jlong handle)
{
  return ((CanTalonSRX*)handle)->GetMotionProfileTopLevelBufferCount();
}
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_IsMotionProfileTopLevelBufferFull
  (JNIEnv *env, jclass, jlong handle)
{
  return ((CanTalonSRX*)handle)->IsMotionProfileTopLevelBufferFull();
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_PushMotionProfileTrajectory
  (JNIEnv *env, jclass, jlong handle, jint targPos, jint targVel, jint profileSlotSelect, jint timeDurMs, jint velOnly, jint isLastPoint, jint zeroPos)
{
  CTR_Code status = ((CanTalonSRX*)handle)->PushMotionProfileTrajectory((int)targPos, (int)targVel, (int)profileSlotSelect, (int)timeDurMs, (int)velOnly, (int)isLastPoint, (int)zeroPos);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_ProcessMotionProfileBuffer
  (JNIEnv *env, jclass, jlong handle)
{
  return ((CanTalonSRX*)handle)->ProcessMotionProfileBuffer();
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFault_1OverTemp
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFault_OverTemp(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFault_1UnderVoltage
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFault_UnderVoltage(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFault_1ForLim
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFault_ForLim(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFault_1RevLim
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFault_RevLim(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFault_1HardwareFailure
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFault_HardwareFailure(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFault_1ForSoftLim
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFault_ForSoftLim(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFault_1RevSoftLim
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFault_RevSoftLim(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetStckyFault_1OverTemp
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetStckyFault_OverTemp(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetStckyFault_1UnderVoltage
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetStckyFault_UnderVoltage(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetStckyFault_1ForLim
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetStckyFault_ForLim(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetStckyFault_1RevLim
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetStckyFault_RevLim(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetStckyFault_1ForSoftLim
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetStckyFault_ForSoftLim(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetStckyFault_1RevSoftLim
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetStckyFault_RevSoftLim(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetAppliedThrottle
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetAppliedThrottle(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetCloseLoopErr
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetCloseLoopErr(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFeedbackDeviceSelect
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFeedbackDeviceSelect(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetModeSelect
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetModeSelect(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetLimitSwitchEn
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetLimitSwitchEn(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetLimitSwitchClosedFor
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetLimitSwitchClosedFor(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetLimitSwitchClosedRev
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetLimitSwitchClosedRev(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetSensorPosition
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetSensorPosition(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetSensorVelocity
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetSensorVelocity(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetCurrent
  (JNIEnv * env, jclass, jlong handle)
{
  double retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetCurrent(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetBrakeIsEnabled
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetBrakeIsEnabled(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetEncPosition
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetEncPosition(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetEncVel
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetEncVel(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetEncIndexRiseEvents
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetEncIndexRiseEvents(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetQuadApin
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetQuadApin(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetQuadBpin
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetQuadBpin(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetQuadIdxpin
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetQuadIdxpin(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetAnalogInWithOv
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetAnalogInWithOv(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetAnalogInVel
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetAnalogInVel(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetTemp
  (JNIEnv * env, jclass, jlong handle)
{
  double retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetTemp(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jdouble JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetBatteryV
  (JNIEnv * env, jclass, jlong handle)
{
  double retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetBatteryV(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetResetCount
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetResetCount(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetResetFlags
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetResetFlags(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetFirmVers
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetFirmVers(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetPulseWidthPosition
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetPulseWidthPosition(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetPulseWidthVelocity
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetPulseWidthVelocity(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetPulseWidthRiseToRiseUs
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetPulseWidthRiseToRiseUs(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetActTraj_1IsValid
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetActTraj_IsValid(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetActTraj_1ProfileSlotSelect
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetActTraj_ProfileSlotSelect(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetActTraj_1VelOnly
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetActTraj_VelOnly(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetActTraj_1IsLast
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetActTraj_IsLast(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetOutputType
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetOutputType(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetHasUnderrun
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetHasUnderrun(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetIsUnderrun
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetIsUnderrun(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetNextID
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetNextID(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetBufferIsFull
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetBufferIsFull(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetCount
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetCount(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetActTraj_1Velocity
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetActTraj_Velocity(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_GetActTraj_1Position
  (JNIEnv * env, jclass, jlong handle)
{
  int retval;
  CTR_Code status = ((CanTalonSRX*)handle)->GetActTraj_Position(retval);
  CheckCTRStatus(env, status);
  return retval;
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetDemand
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetDemand(param);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetOverrideLimitSwitchEn
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetOverrideLimitSwitchEn(param);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetFeedbackDeviceSelect
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetFeedbackDeviceSelect(param);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetRevMotDuringCloseLoopEn
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetRevMotDuringCloseLoopEn(param);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetOverrideBrakeType
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetOverrideBrakeType(param);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetModeSelect
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetModeSelect(param);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetProfileSlotSelect
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetProfileSlotSelect(param);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetRampThrottle
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetRampThrottle(param);
  CheckCTRStatus(env, status);
}
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CanTalonJNI_SetRevFeedbackSensor
  (JNIEnv * env, jclass, jlong handle, jint param)
{
  CTR_Code status = ((CanTalonSRX*)handle)->SetRevFeedbackSensor(param);
  CheckCTRStatus(env, status);
}
}  // extern "C"
