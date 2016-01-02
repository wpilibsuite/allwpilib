/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class CanTalonJNI extends JNIWrapper {
  // Motion Profile status bits
  public static final int kMotionProfileFlag_ActTraj_IsValid = 0x1;
  public static final int kMotionProfileFlag_HasUnderrun     = 0x2;
  public static final int kMotionProfileFlag_IsUnderrun      = 0x4;
  public static final int kMotionProfileFlag_ActTraj_IsLast  = 0x8;
  public static final int kMotionProfileFlag_ActTraj_VelOnly = 0x10;

  /**
   * Signal enumeration for generic signal access.
   * Although every signal is enumerated, only use this for traffic that must be solicited.
   * Use the auto generated getters/setters at bottom of this header as much as possible.
   */
  public enum param_t {
    eProfileParamSlot0_P(1),
    eProfileParamSlot0_I(2),
    eProfileParamSlot0_D(3),
    eProfileParamSlot0_F(4),
    eProfileParamSlot0_IZone(5),
    eProfileParamSlot0_CloseLoopRampRate(6),
    eProfileParamSlot1_P(11),
    eProfileParamSlot1_I(12),
    eProfileParamSlot1_D(13),
    eProfileParamSlot1_F(14),
    eProfileParamSlot1_IZone(15),
    eProfileParamSlot1_CloseLoopRampRate(16),
    eProfileParamSoftLimitForThreshold(21),
    eProfileParamSoftLimitRevThreshold(22),
    eProfileParamSoftLimitForEnable(23),
    eProfileParamSoftLimitRevEnable(24),
    eOnBoot_BrakeMode(31),
    eOnBoot_LimitSwitch_Forward_NormallyClosed(32),
    eOnBoot_LimitSwitch_Reverse_NormallyClosed(33),
    eOnBoot_LimitSwitch_Forward_Disable(34),
    eOnBoot_LimitSwitch_Reverse_Disable(35),
    eFault_OverTemp(41),
    eFault_UnderVoltage(42),
    eFault_ForLim(43),
    eFault_RevLim(44),
    eFault_HardwareFailure(45),
    eFault_ForSoftLim(46),
    eFault_RevSoftLim(47),
    eStckyFault_OverTemp(48),
    eStckyFault_UnderVoltage(49),
    eStckyFault_ForLim(50),
    eStckyFault_RevLim(51),
    eStckyFault_ForSoftLim(52),
    eStckyFault_RevSoftLim(53),
    eAppliedThrottle(61),
    eCloseLoopErr(62),
    eFeedbackDeviceSelect(63),
    eRevMotDuringCloseLoopEn(64),
    eModeSelect(65),
    eProfileSlotSelect(66),
    eRampThrottle(67),
    eRevFeedbackSensor(68),
    eLimitSwitchEn(69),
    eLimitSwitchClosedFor(70),
    eLimitSwitchClosedRev(71),
    eSensorPosition(73),
    eSensorVelocity(74),
    eCurrent(75),
    eBrakeIsEnabled(76),
    eEncPosition(77),
    eEncVel(78),
    eEncIndexRiseEvents(79),
    eQuadApin(80),
    eQuadBpin(81),
    eQuadIdxpin(82),
    eAnalogInWithOv(83),
    eAnalogInVel(84),
    eTemp(85),
    eBatteryV(86),
    eResetCount(87),
    eResetFlags(88),
    eFirmVers(89),
    eSettingsChanged(90),
    eQuadFilterEn(91),
    ePidIaccum(93),
    eStatus1FrameRate(94), // TALON_Status_1_General_10ms_t
    eStatus2FrameRate(95), // TALON_Status_2_Feedback_20ms_t
    eStatus3FrameRate(96), // TALON_Status_3_Enc_100ms_t
    eStatus4FrameRate(97), // TALON_Status_4_AinTempVbat_100ms_t
    eStatus6FrameRate(98), // TALON_Status_6_Eol_t
    eStatus7FrameRate(99), // TALON_Status_7_Debug_200ms_t
    eClearPositionOnIdx(100),
    //reserved
    //reserved
    //reserved
    ePeakPosOutput(104),
    eNominalPosOutput(105),
    ePeakNegOutput(106),
    eNominalNegOutput(107),
    eQuadIdxPolarity(108),
    eStatus8FrameRate(109), // TALON_Status_8_PulseWid_100ms_t
    eAllowPosOverflow(110),
    eProfileParamSlot0_AllowableClosedLoopErr(111),
    eNumberPotTurns(112),
    eNumberEncoderCPR(113),
    ePwdPosition(114),
    eAinPosition(115),
    eProfileParamVcompRate(116),
    eProfileParamSlot1_AllowableClosedLoopErr(117),
    eStatus9FrameRate(118), // TALON_Status_9_MotProfBuffer_100ms_t
    eMotionProfileHasUnderrunErr(119),
    eReserved120(120),
    eLegacyControlMode(121);

    public final int value;
    private param_t(int value) {
      this.value = value;
    }
  }

  public static native long new_CanTalonSRX(int deviceNumber, int controlPeriodMs, int enablePeriodMs);
  public static native long new_CanTalonSRX(int deviceNumber, int controlPeriodMs);
  public static native long new_CanTalonSRX(int deviceNumber);
  public static native long new_CanTalonSRX();
  public static native void delete_CanTalonSRX(long handle);

  public static native void GetMotionProfileStatus(long handle, Object canTalon, Object motionProfileStatus);

  public static native void Set(long handle, double value);
  public static native void SetParam(long handle, int paramEnum, double value);
  public static native void RequestParam(long handle, int paramEnum);
  public static native double GetParamResponse(long handle, int paramEnum);
  public static native int GetParamResponseInt32(long handle, int paramEnum);
  public static native void SetPgain(long handle, int slotIdx, double gain);
  public static native void SetIgain(long handle, int slotIdx, double gain);
  public static native void SetDgain(long handle, int slotIdx, double gain);
  public static native void SetFgain(long handle, int slotIdx, double gain);
  public static native void SetIzone(long handle, int slotIdx, int zone);
  public static native void SetCloseLoopRampRate(long handle, int slotIdx, int closeLoopRampRate);
  public static native void SetVoltageCompensationRate(long handle, double voltagePerMs);
  public static native void SetSensorPosition(long handle, int pos);
  public static native void SetForwardSoftLimit(long handle, int forwardLimit);
  public static native void SetReverseSoftLimit(long handle, int reverseLimit);
  public static native void SetForwardSoftEnable(long handle, int enable);
  public static native void SetReverseSoftEnable(long handle, int enable);
  public static native double GetPgain(long handle, int slotIdx);
  public static native double GetIgain(long handle, int slotIdx);
  public static native double GetDgain(long handle, int slotIdx);
  public static native double GetFgain(long handle, int slotIdx);
  public static native int GetIzone(long handle, int slotIdx);
  public static native int GetCloseLoopRampRate(long handle, int slotIdx);
  public static native double GetVoltageCompensationRate(long handle);
  public static native int GetForwardSoftLimit(long handle);
  public static native int GetReverseSoftLimit(long handle);
  public static native int GetForwardSoftEnable(long handle);
  public static native int GetReverseSoftEnable(long handle);
  public static native int GetPulseWidthRiseToFallUs(long handle);
  public static native int IsPulseWidthSensorPresent(long handle);
  public static native void SetModeSelect2(long handle, int modeSelect, int demand);
  public static native void SetStatusFrameRate(long handle, int frameEnum, int periodMs);
  public static native void ClearStickyFaults(long handle);
  public static native void ChangeMotionControlFramePeriod(long handle, int periodMs);
  public static native void ClearMotionProfileTrajectories(long handle);
  public static native int GetMotionProfileTopLevelBufferCount(long handle);
  public static native boolean IsMotionProfileTopLevelBufferFull(long handle);
  public static native void PushMotionProfileTrajectory(long handle, int targPos, int targVel, int profileSlotSelect, int timeDurMs, int velOnly, int isLastPoint, int zeroPos);
  public static native void ProcessMotionProfileBuffer(long handle);
  public static native int GetFault_OverTemp(long handle);
  public static native int GetFault_UnderVoltage(long handle);
  public static native int GetFault_ForLim(long handle);
  public static native int GetFault_RevLim(long handle);
  public static native int GetFault_HardwareFailure(long handle);
  public static native int GetFault_ForSoftLim(long handle);
  public static native int GetFault_RevSoftLim(long handle);
  public static native int GetStckyFault_OverTemp(long handle);
  public static native int GetStckyFault_UnderVoltage(long handle);
  public static native int GetStckyFault_ForLim(long handle);
  public static native int GetStckyFault_RevLim(long handle);
  public static native int GetStckyFault_ForSoftLim(long handle);
  public static native int GetStckyFault_RevSoftLim(long handle);
  public static native int GetAppliedThrottle(long handle);
  public static native int GetCloseLoopErr(long handle);
  public static native int GetFeedbackDeviceSelect(long handle);
  public static native int GetModeSelect(long handle);
  public static native int GetLimitSwitchEn(long handle);
  public static native int GetLimitSwitchClosedFor(long handle);
  public static native int GetLimitSwitchClosedRev(long handle);
  public static native int GetSensorPosition(long handle);
  public static native int GetSensorVelocity(long handle);
  public static native double GetCurrent(long handle);
  public static native int GetBrakeIsEnabled(long handle);
  public static native int GetEncPosition(long handle);
  public static native int GetEncVel(long handle);
  public static native int GetEncIndexRiseEvents(long handle);
  public static native int GetQuadApin(long handle);
  public static native int GetQuadBpin(long handle);
  public static native int GetQuadIdxpin(long handle);
  public static native int GetAnalogInWithOv(long handle);
  public static native int GetAnalogInVel(long handle);
  public static native double GetTemp(long handle);
  public static native double GetBatteryV(long handle);
  public static native int GetResetCount(long handle);
  public static native int GetResetFlags(long handle);
  public static native int GetFirmVers(long handle);
  public static native int GetPulseWidthPosition(long handle);
  public static native int GetPulseWidthVelocity(long handle);
  public static native int GetPulseWidthRiseToRiseUs(long handle);
  public static native int GetActTraj_IsValid(long handle);
  public static native int GetActTraj_ProfileSlotSelect(long handle);
  public static native int GetActTraj_VelOnly(long handle);
  public static native int GetActTraj_IsLast(long handle);
  public static native int GetOutputType(long handle);
  public static native int GetHasUnderrun(long handle);
  public static native int GetIsUnderrun(long handle);
  public static native int GetNextID(long handle);
  public static native int GetBufferIsFull(long handle);
  public static native int GetCount(long handle);
  public static native int GetActTraj_Velocity(long handle);
  public static native int GetActTraj_Position(long handle);
  public static native void SetDemand(long handle, int param);
  public static native void SetOverrideLimitSwitchEn(long handle, int param);
  public static native void SetFeedbackDeviceSelect(long handle, int param);
  public static native void SetRevMotDuringCloseLoopEn(long handle, int param);
  public static native void SetOverrideBrakeType(long handle, int param);
  public static native void SetModeSelect(long handle, int param);
  public static native void SetProfileSlotSelect(long handle, int param);
  public static native void SetRampThrottle(long handle, int param);
  public static native void SetRevFeedbackSensor(long handle, int param);
}
