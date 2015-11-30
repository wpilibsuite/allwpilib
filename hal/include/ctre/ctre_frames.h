/**
 * @file ctre_frames.h
 * CAN Encoder/Decoder Structures for CTRE devices.
 */
#ifndef CTRE_FRAMES_H
#define CTRE_FRAMES_H

/** control */
typedef struct _TALON_Control_1_General_10ms_t {
	unsigned TokenH:8;
	unsigned TokenL:8;
	unsigned DemandH:8;
	unsigned DemandM:8;
	unsigned DemandL:8;
	unsigned ProfileSlotSelect:1;
	unsigned FeedbackDeviceSelect:4;
	unsigned OverrideLimitSwitchEn:3;
	unsigned RevFeedbackSensor:1;
	unsigned RevMotDuringCloseLoopEn:1;
	unsigned OverrideBrakeType:2;
	unsigned ModeSelect:4;
	unsigned RampThrottle:8;
} TALON_Control_1_General_10ms_t ;

/* TALON_Control_2_Rates_OneShot_t removed since it has been deprecated */

typedef struct _TALON_Control_3_ClearFlags_OneShot_t {
	unsigned ZeroFeedbackSensor:1;
	unsigned ClearStickyFaults:1;
} TALON_Control_3_ClearFlags_OneShot_t ;

typedef struct _TALON_Control_5_General_10ms_t {
	unsigned ThrottleBump_h3:3;
	unsigned ReservedZero:5;
	unsigned ThrottleBump_l8:8;
	unsigned DemandH:8;
	unsigned DemandM:8;
	unsigned DemandL:8;
	unsigned ProfileSlotSelect:1;
	unsigned FeedbackDeviceSelect:4;
	unsigned OverrideLimitSwitchEn:3;
	unsigned RevFeedbackSensor:1;
	unsigned RevMotDuringCloseLoopEn:1;
	unsigned OverrideBrakeType:2;
	unsigned ModeSelect:4;
	unsigned RampThrottle:8;
} TALON_Control_5_General_10ms_t ;

typedef struct _TALON_Control_6_MotProfAddTrajPoint_t {
	unsigned huffCode:2; //!< Compression coding
	unsigned NextPt_VelOnly:1;
	unsigned NextPt_IsLast:1;
	unsigned reserved0:2;
	unsigned NextPt_ZeroPosition:1;
	unsigned NextPt_ProfileSlotSelect:1;
	unsigned Idx:4;
	unsigned reserved1:4;
	unsigned restOfFrame0:8;
	unsigned restOfFrame1:8;
	unsigned restOfFrame2:8;
	unsigned restOfFrame3:8;
	unsigned restOfFrame4:8;
	unsigned restOfFrame5:8;
} TALON_Control_6_MotProfAddTrajPoint_t;

typedef struct _TALON_Control_6_MotProfAddTrajPoint_huff0_t {
	unsigned huffCode_expect_0:2; //!< Compression coding
	unsigned NextPt_VelOnly:1;
	unsigned NextPt_IsLast:1;
	unsigned reserved0:2;
	unsigned NextPt_ZeroPosition:1;
	unsigned NextPt_ProfileSlotSelect:1;
	unsigned Idx:4;
	unsigned reserved1:4;
	unsigned NextPt_DurationMs:8;
	unsigned NextPt_VelocityH:8;
	unsigned NextPt_VelocityL:8;
	unsigned NextPt_PositionH:8;
	unsigned NextPt_PositionM:8;
	unsigned NextPt_PositionL:8;
} TALON_Control_6_MotProfAddTrajPoint_huff0_t;

typedef struct _TALON_Control_6_MotProfAddTrajPoint_huff1_t {
	unsigned huffCode_expect_1:2; //!< Compression coding
	unsigned NextPt_VelOnly:1;
	unsigned NextPt_IsLast:1;
	unsigned reserved0:2;
	unsigned NextPt_ZeroPosition:1;
	unsigned NextPt_ProfileSlotSelect:1;
	unsigned Idx:4;
	unsigned reserved1:4;
	unsigned NextPt_DurationMs:8;
	unsigned NextPt_SameVelocityH:8;
	unsigned NextPt_SameVelocityL:8;
	unsigned NextPt_DeltaPositionH:8;
	unsigned NextPt_DeltaPositionL:8;
	unsigned NextPt_Count:8;
} TALON_Control_6_MotProfAddTrajPoint_huff1_t;

/** status */
typedef struct _TALON_Status_1_General_10ms_t {
	unsigned CloseLoopErrH:8;
	unsigned CloseLoopErrM:8;
	unsigned CloseLoopErrL:8;
	unsigned AppliedThrottle_h3:3;
	unsigned Fault_RevSoftLim:1;
	unsigned Fault_ForSoftLim:1;
	unsigned TokLocked:1;
	unsigned LimitSwitchClosedRev:1;
	unsigned LimitSwitchClosedFor:1;
	unsigned AppliedThrottle_l8:8;
	unsigned ModeSelect_h1:1;
	unsigned FeedbackDeviceSelect:4;
	unsigned LimitSwitchEn:3;
	unsigned Fault_HardwareFailure:1;
	unsigned Fault_RevLim:1;
	unsigned Fault_ForLim:1;
	unsigned Fault_UnderVoltage:1;
	unsigned Fault_OverTemp:1;
	unsigned ModeSelect_b3:3;
	unsigned TokenSeed:8;
} TALON_Status_1_General_10ms_t ;
typedef struct _TALON_Status_2_Feedback_20ms_t {
	unsigned SensorPositionH:8;
	unsigned SensorPositionM:8;
	unsigned SensorPositionL:8;
	unsigned SensorVelocityH:8;
	unsigned SensorVelocityL:8;
	unsigned Current_h8:8;
	unsigned StckyFault_RevSoftLim:1;
	unsigned StckyFault_ForSoftLim:1;
	unsigned StckyFault_RevLim:1;
	unsigned StckyFault_ForLim:1;
	unsigned StckyFault_UnderVoltage:1;
	unsigned StckyFault_OverTemp:1;
	unsigned Current_l2:2;
	unsigned reserved:3;
	unsigned Cmd5Allowed:1;
	unsigned VelDiv4:1;
	unsigned PosDiv8:1;
	unsigned ProfileSlotSelect:1;
	unsigned BrakeIsEnabled:1;
} TALON_Status_2_Feedback_20ms_t ;
typedef struct _TALON_Status_3_Enc_100ms_t {
	unsigned EncPositionH:8;
	unsigned EncPositionM:8;
	unsigned EncPositionL:8;
	unsigned EncVelH:8;
	unsigned EncVelL:8;
	unsigned EncIndexRiseEventsH:8;
	unsigned EncIndexRiseEventsL:8;
	unsigned reserved:3;
	unsigned VelDiv4:1;
	unsigned PosDiv8:1;
	unsigned QuadIdxpin:1;
	unsigned QuadBpin:1;
	unsigned QuadApin:1;
} TALON_Status_3_Enc_100ms_t ;
typedef struct _TALON_Status_4_AinTempVbat_100ms_t {
	unsigned AnalogInWithOvH:8;
	unsigned AnalogInWithOvM:8;
	unsigned AnalogInWithOvL:8;
	unsigned AnalogInVelH:8;
	unsigned AnalogInVelL:8;
	unsigned Temp:8;
	unsigned BatteryV:8;
	unsigned reserved:6;
	unsigned VelDiv4:1;
	unsigned PosDiv8:1;
} TALON_Status_4_AinTempVbat_100ms_t ;
typedef struct _TALON_Status_5_Startup_OneShot_t {
	unsigned ResetCountH:8;
	unsigned ResetCountL:8;
	unsigned ResetFlagsH:8;
	unsigned ResetFlagsL:8;
	unsigned FirmVersH:8;
	unsigned FirmVersL:8;
} TALON_Status_5_Startup_OneShot_t ;
typedef struct _TALON_Status_6_Eol_t {
	unsigned currentAdcUncal_h2:2;
	unsigned reserved1:5;
	unsigned SpiCsPin_GadgeteerPin6:1;
	unsigned currentAdcUncal_l8:8;
	unsigned tempAdcUncal_h2:2;
	unsigned reserved2:6;
	unsigned tempAdcUncal_l8:8;
	unsigned vbatAdcUncal_h2:2;
	unsigned reserved3:6;
	unsigned vbatAdcUncal_l8:8;
	unsigned analogAdcUncal_h2:2;
	unsigned reserved4:6;
	unsigned analogAdcUncal_l8:8;
} TALON_Status_6_Eol_t ;
typedef struct _TALON_Status_7_Debug_200ms_t {
	unsigned TokenizationFails_h8:8;
	unsigned TokenizationFails_l8:8;
	unsigned LastFailedToken_h8:8;
	unsigned LastFailedToken_l8:8;
	unsigned TokenizationSucceses_h8:8;
	unsigned TokenizationSucceses_l8:8;
} TALON_Status_7_Debug_200ms_t ;
typedef struct _TALON_Status_8_PulseWid_100ms_t {
	unsigned PulseWidPositionH:8;
	unsigned PulseWidPositionM:8;
	unsigned PulseWidPositionL:8;
	unsigned reserved:6;
	unsigned VelDiv4:1;
	unsigned PosDiv8:1;
	unsigned PeriodUsM8:8;
	unsigned PeriodUsL8:8;
	unsigned PulseWidVelH:8;
	unsigned PulseWidVelL:8;
} TALON_Status_8_PulseWid_100ms_t ;
typedef struct _TALON_Status_9_MotProfBuffer_100ms_t {
	unsigned ActTraj_IsValid:1; //!< '1' if other ActTraj_* signals are valid.  '0' if there is no active trajectory pt.
	unsigned ActTraj_ProfileSlotSelect:1;
	unsigned ActTraj_VelOnly:1;
	unsigned ActTraj_IsLast:1;
	unsigned OutputType:2;
	unsigned HasUnderrun:1;
	unsigned IsUnderrun:1;
	unsigned NextID:4;
	unsigned reserved1:3;
	unsigned BufferIsFull:1;
	unsigned Count:8;
	unsigned ActTraj_VelocityH:8;
	unsigned ActTraj_VelocityL:8;
	unsigned ActTraj_PositionH:8;
	unsigned ActTraj_PositionM:8;
	unsigned ActTraj_PositionL:8;
} TALON_Status_9_MotProfBuffer_100ms_t ;
typedef struct _TALON_Param_Request_t {
	unsigned ParamEnum:8;
} TALON_Param_Request_t ;
typedef struct _TALON_Param_Response_t {
	unsigned ParamEnum:8;
	unsigned ParamValueL:8;
	unsigned ParamValueML:8;
	unsigned ParamValueMH:8;
	unsigned ParamValueH:8;
} TALON_Param_Response_t ;

#endif /* CTRE_FRAMES_H */
