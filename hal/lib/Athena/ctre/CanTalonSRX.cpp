/**
 * auto generated using spreadsheet and WpiClassGen.csproj
 * @link https://docs.google.com/spreadsheets/d/1OU_ZV7fZLGYUQ-Uhc8sVAmUmWTlT8XBFYK8lfjg_tac/edit#gid=1766046967
 */
#include "CanTalonSRX.h"
#include "NetworkCommunication/CANSessionMux.h"	//CAN Comm
#include <string.h> // memset
#include <unistd.h> // usleep

#define STATUS_1  		0x02041400
#define STATUS_2  		0x02041440
#define STATUS_3  		0x02041480
#define STATUS_4  		0x020414C0
#define STATUS_5  		0x02041500
#define STATUS_6  		0x02041540
#define STATUS_7  		0x02041580

#define CONTROL_1 			0x02040000

#define EXPECTED_RESPONSE_TIMEOUT_MS	(200)
#define GET_STATUS1() CtreCanNode::recMsg<TALON_Status_1_General_10ms_t		> rx = GetRx<TALON_Status_1_General_10ms_t>(STATUS_1	  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS2() CtreCanNode::recMsg<TALON_Status_2_Feedback_20ms_t	> rx = GetRx<TALON_Status_2_Feedback_20ms_t>(STATUS_2	  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS3() CtreCanNode::recMsg<TALON_Status_3_Enc_100ms_t		> rx = GetRx<TALON_Status_3_Enc_100ms_t>(STATUS_3		  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS4() CtreCanNode::recMsg<TALON_Status_4_AinTempVbat_100ms_t> rx = GetRx<TALON_Status_4_AinTempVbat_100ms_t>(STATUS_4 | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS5() CtreCanNode::recMsg<TALON_Status_5_Startup_OneShot_t	> rx = GetRx<TALON_Status_5_Startup_OneShot_t>(STATUS_5	  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS6() CtreCanNode::recMsg<TALON_Status_6_Eol_t				> rx = GetRx<TALON_Status_6_Eol_t>(STATUS_6				  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS7() CtreCanNode::recMsg<TALON_Status_7_Debug_200ms_t		> rx = GetRx<TALON_Status_7_Debug_200ms_t>(STATUS_7		  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)

/* encoder/decoders */
/** control */
typedef struct _TALON_Control_1_General_10ms_t {
	unsigned TokenH:8;
	unsigned TokenL:8;
	unsigned Demand24H:8;
	unsigned Demand24M:8;
	unsigned Demand24L:8;
	unsigned CloseLoopCellSelect:1;
	unsigned SelectlFeedbackDevice:4;
	unsigned LimitSwitchEn:3;
	unsigned RevEncoderPosAndVel:1;
	unsigned RevMotDuringCloseLoopEn:1;
	unsigned BrakeType:2;
	unsigned ModeSelect:4;
	unsigned RampThrottle:8;
} TALON_Control_1_General_10ms_t ;
typedef struct _TALON_Control_2_Rates_OneShot_t {
	unsigned Status1Ms:8;
	unsigned Status2Ms:8;
	unsigned Status3Ms:8;
	unsigned Status4Ms:8;
} TALON_Control_2_Rates_OneShot_t ;
typedef struct _TALON_Control_3_ClearFlags_OneShot_t {
	unsigned ZeroFeedbackSensor:1;
	unsigned ClearStickyFaults:1;
} TALON_Control_3_ClearFlags_OneShot_t ;

/** status */
typedef struct _TALON_Status_1_General_10ms_t {
	unsigned CloseLoopErrH:8;
	unsigned CloseLoopErrM:8;
	unsigned CloseLoopErrL:8;
	unsigned Throttle_h3:3;
	unsigned Fault_RevSoftLim:1;
	unsigned Fault_ForSoftLim:1;
	unsigned TokLocked:1;
	unsigned LimitSwitchClosedRev:1;
	unsigned LimitSwitchClosedFor:1;
	unsigned Throttle_l8:8;
	unsigned ModeSelect_h1:1;
	unsigned SelectlFeedbackDevice:4;
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
	unsigned reserved:6;
	unsigned CloseLoopCellSelect:1;
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
	unsigned reserved:5;
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
	unsigned reserved:8;
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
typedef struct _TALON_Config_SetGains0_1_t {
	unsigned PH:8;
	unsigned PM:8;
	unsigned PL:8;
	unsigned IH:8;
	unsigned IM:8;
	unsigned IL:8;
	unsigned IZoneH:8;
	unsigned IZoneL:8;
} TALON_Config_SetGains0_1_t ;
typedef struct _TALON_Config_SetGains0_2_t {
	unsigned DH:8;
	unsigned DM:8;
	unsigned DL:8;
	unsigned FH:8;
	unsigned FM:8;
	unsigned FL:8;
	unsigned RampRateH:8;
	unsigned RampRateL:8;
} TALON_Config_SetGains0_2_t ;
typedef struct _TALON_Config_SetGains1_1_t {
	unsigned PH:8;
	unsigned PM:8;
	unsigned PL:8;
	unsigned IH:8;
	unsigned IM:8;
	unsigned IL:8;
	unsigned IZoneH:8;
	unsigned IZoneL:8;
} TALON_Config_SetGains1_1_t ;
typedef struct _TALON_Config_SetGains1_2_t {
	unsigned DH:8;
	unsigned DM:8;
	unsigned DL:8;
	unsigned FH:8;
	unsigned FM:8;
	unsigned FL:8;
	unsigned RampRateH:8;
	unsigned RampRateL:8;
} TALON_Config_SetGains1_2_t ;
typedef struct _TALON_Config_SetSoftLimits_t {
	unsigned LimitFH:8;
	unsigned LimitFMH:8;
	unsigned LimitFML:8;
	unsigned LimitFL:8;
	unsigned LimitRH:8;
	unsigned LimitRMH:8;
	unsigned LimitRML:8;
	unsigned LimitRL:8;
} TALON_Config_SetSoftLimits_t ;
typedef struct _TALON_Param_Request_t {
	unsigned ParamEnum:8;
} TALON_Param_Request_t ;
typedef struct _TALON_Param_Response_t {
	unsigned ParamEnum:8;
	unsigned ParamValueH:8;
	unsigned ParamValueMH:8;
	unsigned ParamValueML:8;
	unsigned ParamValueL:8;
} TALON_Param_Response_t ;


CanTalonSRX::CanTalonSRX(UINT8 deviceNumber): CtreCanNode(deviceNumber)
{
	RegisterRx(STATUS_1 | deviceNumber );
	RegisterRx(STATUS_2 | deviceNumber );
	RegisterRx(STATUS_3 | deviceNumber );
	RegisterRx(STATUS_4 | deviceNumber );
	RegisterRx(STATUS_5 | deviceNumber );
	RegisterRx(STATUS_6 | deviceNumber );
	RegisterRx(STATUS_7 | deviceNumber );
	RegisterTx(CONTROL_1 | deviceNumber, 10);
}
/* CanTalonSRX D'tor
 */
CanTalonSRX::~CanTalonSRX()
{
}
void CanTalonSRX::Set(double value)
{
	if(value > 1)
		value = 1;
	else if(value < -1)
		value = -1;
	value *= 1023;
	SetDemand24(value); /* must be within [-1023,1023] */
}
/*------------------------ auto generated ----------------------*/

CTR_Code CanTalonSRX::GetFault_OverTemp(int &param)
{
	GET_STATUS1();
	param = rx->Fault_OverTemp;
	return rx.err;
}
CTR_Code CanTalonSRX::GetFault_UnderVoltage(int &param)
{
	GET_STATUS1();
	param = rx->Fault_UnderVoltage;
	return rx.err;
}
CTR_Code CanTalonSRX::GetFault_ForLim(int &param)
{
	GET_STATUS1();
	param = rx->Fault_ForLim;
	return rx.err;
}
CTR_Code CanTalonSRX::GetFault_RevLim(int &param)
{
	GET_STATUS1();
	param = rx->Fault_RevLim;
	return rx.err;
}
CTR_Code CanTalonSRX::GetFault_HardwareFailure(int &param)
{
	GET_STATUS1();
	param = rx->Fault_HardwareFailure;
	return rx.err;
}
CTR_Code CanTalonSRX::GetFault_ForSoftLim(int &param)
{
	GET_STATUS1();
	param = rx->Fault_ForSoftLim;
	return rx.err;
}
CTR_Code CanTalonSRX::GetFault_RevSoftLim(int &param)
{
	GET_STATUS1();
	param = rx->Fault_RevSoftLim;
	return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_OverTemp(int &param)
{
	GET_STATUS2();
	param = rx->StckyFault_OverTemp;
	return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_UnderVoltage(int &param)
{
	GET_STATUS2();
	param = rx->StckyFault_UnderVoltage;
	return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_ForLim(int &param)
{
	GET_STATUS2();
	param = rx->StckyFault_ForLim;
	return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_RevLim(int &param)
{
	GET_STATUS2();
	param = rx->StckyFault_RevLim;
	return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_ForSoftLim(int &param)
{
	GET_STATUS2();
	param = rx->StckyFault_ForSoftLim;
	return rx.err;
}
CTR_Code CanTalonSRX::GetStckyFault_RevSoftLim(int &param)
{
	GET_STATUS2();
	param = rx->StckyFault_RevSoftLim;
	return rx.err;
}
CTR_Code CanTalonSRX::GetAppliedThrottle11(int &param)
{
	GET_STATUS1();
	uint32_t raw = 0;
	raw |= rx->Throttle_h3;
	raw <<= 8;
	raw |= rx->Throttle_l8;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetCloseLoopErr(int &param)
{
	GET_STATUS1();
	uint32_t raw = 0;
	raw |= rx->CloseLoopErrH;
	raw <<= 16;
	raw |= rx->CloseLoopErrM;
	raw <<= 8;
	raw |= rx->CloseLoopErrL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetSelectlFeedbackDevice(int &param)
{
	GET_STATUS1();
	param = rx->SelectlFeedbackDevice;
	return rx.err;
}
CTR_Code CanTalonSRX::GetModeSelect(int &param)
{
	GET_STATUS1();
	uint32_t raw = 0;
	raw |= rx->ModeSelect_h1;
	raw <<= 3;
	raw |= rx->ModeSelect_b3;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetLimitSwitchEn(int &param)
{
	GET_STATUS1();
	param = rx->LimitSwitchEn;
	return rx.err;
}
CTR_Code CanTalonSRX::GetLimitSwitchClosedFor(int &param)
{
	GET_STATUS1();
	param = rx->LimitSwitchClosedFor;
	return rx.err;
}
CTR_Code CanTalonSRX::GetLimitSwitchClosedRev(int &param)
{
	GET_STATUS1();
	param = rx->LimitSwitchClosedRev;
	return rx.err;
}
CTR_Code CanTalonSRX::GetCloseLoopCellSelect(int &param)
{
	GET_STATUS2();
	param = rx->CloseLoopCellSelect;
	return rx.err;
}
CTR_Code CanTalonSRX::GetSensorPosition(int &param)
{
	GET_STATUS2();
	uint32_t raw = 0;
	raw |= rx->SensorPositionH;
	raw <<= 16;
	raw |= rx->SensorPositionM;
	raw <<= 8;
	raw |= rx->SensorPositionL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetSensorVelocity(int &param)
{
	GET_STATUS2();
	uint32_t raw = 0;
	raw |= rx->SensorVelocityH;
	raw <<= 8;
	raw |= rx->SensorVelocityL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetCurrent(double &param)
{
	GET_STATUS2();
	uint32_t raw = 0;
	raw |= rx->Current_h8;
	raw <<= 2;
	raw |= rx->Current_l2;
	param = (double)raw * 0.125 + 0;
	return rx.err;
}
CTR_Code CanTalonSRX::GetBrakeIsEnabled(int &param)
{
	GET_STATUS2();
	param = rx->BrakeIsEnabled;
	return rx.err;
}
CTR_Code CanTalonSRX::GetEncPosition(int &param)
{
	GET_STATUS3();
	uint32_t raw = 0;
	raw |= rx->EncPositionH;
	raw <<= 16;
	raw |= rx->EncPositionM;
	raw <<= 8;
	raw |= rx->EncPositionL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetEncVel(int &param)
{
	GET_STATUS3();
	uint32_t raw = 0;
	raw |= rx->EncVelH;
	raw <<= 8;
	raw |= rx->EncVelL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetEncIndexRiseEvents(int &param)
{
	GET_STATUS3();
	uint32_t raw = 0;
	raw |= rx->EncIndexRiseEventsH;
	raw <<= 8;
	raw |= rx->EncIndexRiseEventsL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetQuadApin(int &param)
{
	GET_STATUS3();
	param = rx->QuadApin;
	return rx.err;
}
CTR_Code CanTalonSRX::GetQuadBpin(int &param)
{
	GET_STATUS3();
	param = rx->QuadBpin;
	return rx.err;
}
CTR_Code CanTalonSRX::GetQuadIdxpin(int &param)
{
	GET_STATUS3();
	param = rx->QuadIdxpin;
	return rx.err;
}
CTR_Code CanTalonSRX::GetAnalogInWithOv(int &param)
{
	GET_STATUS4();
	uint32_t raw = 0;
	raw |= rx->AnalogInWithOvH;
	raw <<= 16;
	raw |= rx->AnalogInWithOvM;
	raw <<= 8;
	raw |= rx->AnalogInWithOvL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetAnalogInVel(int &param)
{
	GET_STATUS4();
	uint32_t raw = 0;
	raw |= rx->AnalogInVelH;
	raw <<= 8;
	raw |= rx->AnalogInVelL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetTemp(double &param)
{
	GET_STATUS4();
	uint32_t raw = rx->Temp;
	param = (double)raw * 0.6451612903 + -50;
	return rx.err;
}
CTR_Code CanTalonSRX::GetBatteryV(double &param)
{
	GET_STATUS4();
	uint32_t raw = rx->BatteryV;
	param = (double)raw * 0.05 + 4;
	return rx.err;
}
CTR_Code CanTalonSRX::GetResetCount(int &param)
{
	GET_STATUS5();
	uint32_t raw = 0;
	raw |= rx->ResetCountH;
	raw <<= 8;
	raw |= rx->ResetCountL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetResetFlags(int &param)
{
	GET_STATUS5();
	uint32_t raw = 0;
	raw |= rx->ResetFlagsH;
	raw <<= 8;
	raw |= rx->ResetFlagsL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetFirmVers(int &param)
{
	GET_STATUS5();
	uint32_t raw = 0;
	raw |= rx->FirmVersH;
	raw <<= 8;
	raw |= rx->FirmVersL;
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::SetDemand24(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->Demand24H = param>>16;
	toFill->Demand24M = param>>8;
	toFill->Demand24L = param>>0;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetLimitSwitchEn(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->LimitSwitchEn = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetSelectlFeedbackDevice(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->SelectlFeedbackDevice = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetRevMotDuringCloseLoopEn(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->RevMotDuringCloseLoopEn = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetBrakeType(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->BrakeType = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetModeSelect(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->ModeSelect = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetCloseLoopCellSelect(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->CloseLoopCellSelect = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetRampThrottle(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->RampThrottle = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetRevEncoderPosAndVel(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->RevEncoderPosAndVel = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
