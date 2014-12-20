/**
 * @brief CAN TALON SRX driver.
 *
 * The TALON SRX is designed to instrument all runtime signals periodically.  The default periods are chosen to support 16 TALONs
 * with 10ms update rate for control (throttle or setpoint).  However these can be overridden with SetStatusFrameRate. @see SetStatusFrameRate
 * The getters for these unsolicited signals are auto generated at the bottom of this module.
 *
 * Likewise most control signals are sent periodically using the fire-and-forget CAN API.
 * The setters for these unsolicited signals are auto generated at the bottom of this module.
 *
 * Signals that are not available in an unsolicited fashion are the Close Loop gains.
 * For teams that have a single profile for their TALON close loop they can use either the webpage to configure their TALONs once
 * 	or set the PIDF,Izone,CloseLoopRampRate,etc... once in the robot application.  These parameters are saved to flash so once they are
 * 	loaded in the TALON, they will persist through power cycles and mode changes.
 *
 * For teams that have one or two profiles to switch between, they can use the same strategy since there are two slots to choose from
 * and the ProfileSlotSelect is periodically sent in the 10 ms control frame.
 *
 * For teams that require changing gains frequently, they can use the soliciting API to get and set those parameters.  Most likely
 * they will only need to set them in a periodic fashion as a function of what motion the application is attempting.
 * If this API is used, be mindful of the CAN utilization reported in the driver station.
 *
 * Encoder position is measured in encoder edges.  Every edge is counted (similar to roboRIO 4X mode).
 * Analog position is 10 bits, meaning 1024 ticks per rotation (0V => 3.3V).
 * Use SetFeedbackDeviceSelect to select which sensor type you need.  Once you do that you can use GetSensorPosition()
 * and GetSensorVelocity().  These signals are updated on CANBus every 20ms (by default).
 * If a relative sensor is selected, you can zero (or change the current value) using SetSensorPosition.
 *
 * Analog Input and quadrature position (and velocity) are also explicitly reported in GetEncPosition, GetEncVel, GetAnalogInWithOv, GetAnalogInVel.
 * These signals are available all the time, regardless of what sensor is selected at a rate of 100ms.  This allows easy instrumentation
 * for "in the pits" checking of all sensors regardless of modeselect.  The 100ms rate is overridable for teams who want to acquire sensor
 * data for processing, not just instrumentation.  Or just select the sensor using SetFeedbackDeviceSelect to get it at 20ms.
 *
 * Velocity is in position ticks / 100ms.
 *
 * All output units are in respect to duty cycle (throttle) which is -1023(full reverse) to +1023 (full forward).
 *  This includes demand (which specifies duty cycle when in duty cycle mode) and rampRamp, which is in throttle units per 10ms (if nonzero).
 *
 * Pos and velocity close loops are calc'd as
 * 		err = target - posOrVel.
 * 		iErr += err;
 * 		if(   (IZone!=0)  and  abs(err) > IZone)
 * 			ClearIaccum()
 * 		output = P X err + I X iErr + D X dErr + F X target
 * 		dErr = err - lastErr
 *	P, I,and D gains are always positive. F can be negative.
 *	Motor direction can be reversed using SetRevMotDuringCloseLoopEn if sensor and motor are out of phase.
 *	Similarly feedback sensor can also be reversed (multiplied by -1) if you prefer the sensor to be inverted.
 *
 * P gain is specified in throttle per error tick.  For example, a value of 102 is ~9.9% (which is 102/1023) throttle per 1
 * 		ADC unit(10bit) or 1 quadrature encoder edge depending on selected sensor.
 *
 * I gain is specified in throttle per integrated error. For example, a value of 10 equates to ~0.99% (which is 10/1023)
 *  	for each accumulated ADC unit(10bit) or 1 quadrature encoder edge depending on selected sensor.
 *  	Close loop and integral accumulator runs every 1ms.
 *
 * D gain is specified in throttle per derivative error. For example a value of 102 equates to ~9.9% (which is 102/1023)
 * 	per change of 1 unit (ADC or encoder) per ms.
 *
 * I Zone is specified in the same units as sensor position (ADC units or quadrature edges).  If pos/vel error is outside of
 * 		this value, the integrated error will auto-clear...
 * 		if(   (IZone!=0)  and  abs(err) > IZone)
 * 			ClearIaccum()
 * 		...this is very useful in preventing integral windup and is highly recommended if using full PID to keep stability low.
 *
 * CloseLoopRampRate is in throttle units per 1ms.  Set to zero to disable ramping.
 * 		Works the same as RampThrottle but only is in effect when a close loop mode and profile slot is selected.
 *
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
#define CONTROL_2 			0x02040040
#define CONTROL_3 			0x02040080

#define EXPECTED_RESPONSE_TIMEOUT_MS	(200)
#define GET_STATUS1() CtreCanNode::recMsg<TALON_Status_1_General_10ms_t		> rx = GetRx<TALON_Status_1_General_10ms_t>(STATUS_1	  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS2() CtreCanNode::recMsg<TALON_Status_2_Feedback_20ms_t	> rx = GetRx<TALON_Status_2_Feedback_20ms_t>(STATUS_2	  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS3() CtreCanNode::recMsg<TALON_Status_3_Enc_100ms_t		> rx = GetRx<TALON_Status_3_Enc_100ms_t>(STATUS_3		  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS4() CtreCanNode::recMsg<TALON_Status_4_AinTempVbat_100ms_t> rx = GetRx<TALON_Status_4_AinTempVbat_100ms_t>(STATUS_4 | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS5() CtreCanNode::recMsg<TALON_Status_5_Startup_OneShot_t	> rx = GetRx<TALON_Status_5_Startup_OneShot_t>(STATUS_5	  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS6() CtreCanNode::recMsg<TALON_Status_6_Eol_t				> rx = GetRx<TALON_Status_6_Eol_t>(STATUS_6				  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS7() CtreCanNode::recMsg<TALON_Status_7_Debug_200ms_t		> rx = GetRx<TALON_Status_7_Debug_200ms_t>(STATUS_7		  | GetDeviceNumber(), EXPECTED_RESPONSE_TIMEOUT_MS)

#define PARAM_REQUEST 		0x02041800
#define PARAM_RESPONSE 		0x02041840
#define PARAM_SET			0x02041880

const int kParamArbIdValue = 	PARAM_RESPONSE;
const int kParamArbIdMask = 	0xFFFFFFFF;

const double FLOAT_TO_FXP = (double)0x400000;
const double FXP_TO_FLOAT = 0.0000002384185791015625;

/* encoder/decoders */
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
	unsigned reserved:6;
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


CanTalonSRX::CanTalonSRX(int deviceNumber,int controlPeriodMs): CtreCanNode(deviceNumber), _can_h(0), _can_stat(0)
{
	/* bound period to be within [1 ms,95 ms] */
	if(controlPeriodMs < 1)
		controlPeriodMs = 1;
	else if(controlPeriodMs > 95)
		controlPeriodMs = 95;
	RegisterRx(STATUS_1 | (UINT8)deviceNumber );
	RegisterRx(STATUS_2 | (UINT8)deviceNumber );
	RegisterRx(STATUS_3 | (UINT8)deviceNumber );
	RegisterRx(STATUS_4 | (UINT8)deviceNumber );
	RegisterRx(STATUS_5 | (UINT8)deviceNumber );
	RegisterRx(STATUS_6 | (UINT8)deviceNumber );
	RegisterRx(STATUS_7 | (UINT8)deviceNumber );
	RegisterTx(CONTROL_1 | (UINT8)deviceNumber, (UINT8)controlPeriodMs);
	/* default our frame rate table to what firmware defaults to. */
	_statusRateMs[0] = 10;	/* 	TALON_Status_1_General_10ms_t 		*/
	_statusRateMs[1] = 20;	/* 	TALON_Status_2_Feedback_20ms_t 		*/
	_statusRateMs[2] = 100;	/* 	TALON_Status_3_Enc_100ms_t 			*/
	_statusRateMs[3] = 100;	/* 	TALON_Status_4_AinTempVbat_100ms_t 	*/
	/* the only default param that is nonzero is limit switch.
	 * Default to using the flash settings. */
	SetOverrideLimitSwitchEn(kLimitSwitchOverride_UseDefaultsFromFlash);
}
/* CanTalonSRX D'tor
 */
CanTalonSRX::~CanTalonSRX()
{
	if(_can_h){
		FRC_NetworkCommunication_CANSessionMux_closeStreamSession(_can_h);
		_can_h = 0;
	}
}
void CanTalonSRX::OpenSessionIfNeedBe()
{
	_can_stat = 0;
	if (_can_h == 0) {
		/* bit30 - bit8 must match $000002XX.  Top bit is not masked to get remote frames */
		FRC_NetworkCommunication_CANSessionMux_openStreamSession(&_can_h,kParamArbIdValue | GetDeviceNumber(), kParamArbIdMask, kMsgCapacity, &_can_stat);
		if (_can_stat == 0) {
			/* success */
		} else {
			/* something went wrong, try again later */
			_can_h = 0;
		}
	}
}
void CanTalonSRX::ProcessStreamMessages()
{
	if(0 == _can_h)
		OpenSessionIfNeedBe();
	/* process receive messages */
	uint32_t i;
	uint32_t messagesToRead = sizeof(_msgBuff) / sizeof(_msgBuff[0]);
	uint32_t messagesRead = 0;
	/* read out latest bunch of messages */
	_can_stat = 0;
	if (_can_h){
		FRC_NetworkCommunication_CANSessionMux_readStreamSession(_can_h,_msgBuff, messagesToRead, &messagesRead, &_can_stat);
	}
	/* loop thru each message of interest */
	for (i = 0; i < messagesRead; ++i) {
		tCANStreamMessage * msg = _msgBuff + i;
		if(msg->messageID == (PARAM_RESPONSE | GetDeviceNumber()) ){
			TALON_Param_Response_t * paramResp = (TALON_Param_Response_t*)msg->data;
			/* decode value */
			int32_t val = paramResp->ParamValueH;
			val <<= 8;
			val |=  paramResp->ParamValueMH;
			val <<= 8;
			val |=  paramResp->ParamValueML;
			val <<= 8;
			val |=  paramResp->ParamValueL;
			/* save latest signal */
			_sigs[paramResp->ParamEnum] = val;
		}else{
			int brkpthere = 42;
			++brkpthere;
		}
	}
}
void CanTalonSRX::Set(double value)
{
	if(value > 1)
		value = 1;
	else if(value < -1)
		value = -1;
	SetDemand(1023*value); /* must be within [-1023,1023] */
}
/*---------------------setters and getters that use the param request/response-------------*/
/**
 * Send a one shot frame to set an arbitrary signal.
 * Most signals are in the control frame so avoid using this API unless you have to.
 * Use this api for...
 * -A motor controller profile signal eProfileParam_XXXs.  These are backed up in flash.  If you are gain-scheduling then call this periodically.
 * -Default brake and limit switch signals... eOnBoot_XXXs.  Avoid doing this, use the override signals in the control frame.
 * Talon will automatically send a PARAM_RESPONSE after the set, so GetParamResponse will catch the latest value after a couple ms.
 */
CTR_Code CanTalonSRX::SetParamRaw(unsigned paramEnum, int rawBits)
{
	/* caller is using param API.  Open session if it hasn'T been done. */
	if(0 == _can_h)
		OpenSessionIfNeedBe();
	TALON_Param_Response_t frame;
	memset(&frame,0,sizeof(frame));
	frame.ParamEnum = paramEnum;
	frame.ParamValueH = rawBits >> 0x18;
	frame.ParamValueMH = rawBits >> 0x10;
	frame.ParamValueML = rawBits >> 0x08;
	frame.ParamValueL = rawBits;
	int32_t status = 0;
	FRC_NetworkCommunication_CANSessionMux_sendMessage(PARAM_SET | GetDeviceNumber(), (const uint8_t*)&frame, 5, 0, &status);
	if(status)
		return CTR_TxFailed;
	return CTR_OKAY;
}
/**
 * Checks cached CAN frames and updating solicited signals.
 */
CTR_Code CanTalonSRX::GetParamResponseRaw(unsigned paramEnum, int & rawBits)
{
	CTR_Code retval = CTR_OKAY;
	/* process received param events. We don't expect many since this API is not used often. */
	ProcessStreamMessages();
	/* grab the solicited signal value */
	sigs_t::iterator i = _sigs.find(paramEnum);
	if(i == _sigs.end()){
		retval = CTR_SigNotUpdated;
	}else{
		rawBits = i->second;
	}
	return retval;
}
/**
 * Asks TALON to immedietely respond with signal value.  This API is only used for signals that are not sent periodically.
 * This can be useful for reading params that rarely change like Limit Switch settings and PIDF values.
  * @param param to request.
 */
CTR_Code CanTalonSRX::RequestParam(param_t paramEnum)
{
	/* process received param events. We don't expect many since this API is not used often. */
	ProcessStreamMessages();
	TALON_Param_Request_t frame;
	memset(&frame,0,sizeof(frame));
	frame.ParamEnum = paramEnum;
	int32_t status = 0;
	FRC_NetworkCommunication_CANSessionMux_sendMessage(PARAM_REQUEST | GetDeviceNumber(), (const uint8_t*)&frame, 1, 0, &status);
	if(status)
		return CTR_TxFailed;
	return CTR_OKAY;
}

CTR_Code CanTalonSRX::SetParam(param_t paramEnum, double value)
{
	int32_t rawbits = 0;
	switch(paramEnum){
		case eProfileParamSlot0_P:/* unsigned 10.22 fixed pt value */
		case eProfileParamSlot0_I:
		case eProfileParamSlot0_D:
		case eProfileParamSlot1_P:
		case eProfileParamSlot1_I:
		case eProfileParamSlot1_D:
		{
			uint32_t urawbits;
			value = std::min(value,1023.0); /* bounds check doubles that are outside u10.22 */
			value = std::max(value,0.0);
			urawbits = value * FLOAT_TO_FXP; /* perform unsign arithmetic */
			rawbits = urawbits; /* copy bits over.  SetParamRaw just stuffs into CAN frame with no sense of signedness */
		}	break;
		case eProfileParamSlot1_F:	/* signed 10.22 fixed pt value */
		case eProfileParamSlot0_F:
			value = std::min(value, 512.0); /* bounds check doubles that are outside s10.22 */
			value = std::max(value,-512.0);
			rawbits = value * FLOAT_TO_FXP;
			break;
		default: /* everything else is integral */
			rawbits = (int32_t)value;
			break;
	}
	return SetParamRaw(paramEnum,rawbits);
}
CTR_Code CanTalonSRX::GetParamResponse(param_t paramEnum, double & value)
{
	int32_t rawbits = 0;
	CTR_Code retval = GetParamResponseRaw(paramEnum,rawbits);
	switch(paramEnum){
		case eProfileParamSlot0_P:/* 10.22 fixed pt value */
		case eProfileParamSlot0_I:
		case eProfileParamSlot0_D:
		case eProfileParamSlot0_F:
		case eProfileParamSlot1_P:
		case eProfileParamSlot1_I:
		case eProfileParamSlot1_D:
		case eProfileParamSlot1_F:
		case eCurrent:
		case eTemp:
		case eBatteryV:
			value = ((double)rawbits) * FXP_TO_FLOAT;
			break;
		default: /* everything else is integral */
			value = (double)rawbits;
			break;
	}
	return retval;
}
CTR_Code CanTalonSRX::GetParamResponseInt32(param_t paramEnum, int & value)
{
	double dvalue = 0;
	CTR_Code retval = GetParamResponse(paramEnum, dvalue);
	value = (int32_t)dvalue;
	return retval;
}
/*----- getters and setters that use param request/response. These signals are backed up in flash and will survive a power cycle. ---------*/
/*----- If your application requires changing these values consider using both slots and switch between slot0 <=> slot1. ------------------*/
/*----- If your application requires changing these signals frequently then it makes sense to leverage this API. --------------------------*/
/*----- Getters don't block, so it may require several calls to get the latest value. --------------------------*/
CTR_Code CanTalonSRX::SetPgain(unsigned slotIdx,double gain)
{
	if(slotIdx == 0)
		return SetParam(eProfileParamSlot0_P, gain);
	return SetParam(eProfileParamSlot1_P, gain);
}
CTR_Code CanTalonSRX::SetIgain(unsigned slotIdx,double gain)
{
	if(slotIdx == 0)
		return SetParam(eProfileParamSlot0_I, gain);
	return SetParam(eProfileParamSlot1_I, gain);
}
CTR_Code CanTalonSRX::SetDgain(unsigned slotIdx,double gain)
{
	if(slotIdx == 0)
		return SetParam(eProfileParamSlot0_D, gain);
	return SetParam(eProfileParamSlot1_D, gain);
}
CTR_Code CanTalonSRX::SetFgain(unsigned slotIdx,double gain)
{
	if(slotIdx == 0)
		return SetParam(eProfileParamSlot0_F, gain);
	return SetParam(eProfileParamSlot1_F, gain);
}
CTR_Code CanTalonSRX::SetIzone(unsigned slotIdx,int zone)
{
	if(slotIdx == 0)
		return SetParam(eProfileParamSlot0_IZone, zone);
	return SetParam(eProfileParamSlot1_IZone, zone);
}
CTR_Code CanTalonSRX::SetCloseLoopRampRate(unsigned slotIdx,int closeLoopRampRate)
{
	if(slotIdx == 0)
		return SetParam(eProfileParamSlot0_CloseLoopRampRate, closeLoopRampRate);
	return SetParam(eProfileParamSlot1_CloseLoopRampRate, closeLoopRampRate);
}
CTR_Code CanTalonSRX::GetPgain(unsigned slotIdx,double & gain)
{
	if(slotIdx == 0)
		return GetParamResponse(eProfileParamSlot0_P, gain);
	return GetParamResponse(eProfileParamSlot1_P, gain);
}
CTR_Code CanTalonSRX::GetIgain(unsigned slotIdx,double & gain)
{
	if(slotIdx == 0)
		return GetParamResponse(eProfileParamSlot0_I, gain);
	return GetParamResponse(eProfileParamSlot1_I, gain);
}
CTR_Code CanTalonSRX::GetDgain(unsigned slotIdx,double & gain)
{
	if(slotIdx == 0)
		return GetParamResponse(eProfileParamSlot0_D, gain);
	return GetParamResponse(eProfileParamSlot1_D, gain);
}
CTR_Code CanTalonSRX::GetFgain(unsigned slotIdx,double & gain)
{
	if(slotIdx == 0)
		return GetParamResponse(eProfileParamSlot0_F, gain);
	return GetParamResponse(eProfileParamSlot1_F, gain);
}
CTR_Code CanTalonSRX::GetIzone(unsigned slotIdx,int & zone)
{
	if(slotIdx == 0)
		return GetParamResponseInt32(eProfileParamSlot0_IZone, zone);
	return GetParamResponseInt32(eProfileParamSlot1_IZone, zone);
}
CTR_Code CanTalonSRX::GetCloseLoopRampRate(unsigned slotIdx,int & closeLoopRampRate)
{
	if(slotIdx == 0)
		return GetParamResponseInt32(eProfileParamSlot0_CloseLoopRampRate, closeLoopRampRate);
	return GetParamResponseInt32(eProfileParamSlot1_CloseLoopRampRate, closeLoopRampRate);
}

CTR_Code CanTalonSRX::SetSensorPosition(int pos)
{
	return SetParam(eSensorPosition, pos);
}
CTR_Code CanTalonSRX::SetForwardSoftLimit(int forwardLimit)
{
	return SetParam(eProfileParamSoftLimitForThreshold, forwardLimit);
}
CTR_Code CanTalonSRX::SetReverseSoftLimit(int reverseLimit)
{
	return SetParam(eProfileParamSoftLimitRevThreshold, reverseLimit);
}
CTR_Code CanTalonSRX::SetForwardSoftEnable(int enable)
{
	return SetParam(eProfileParamSoftLimitForEnable, enable);
}
CTR_Code CanTalonSRX::SetReverseSoftEnable(int enable)
{
	return SetParam(eProfileParamSoftLimitRevEnable, enable);
}
CTR_Code CanTalonSRX::GetForwardSoftLimit(int & forwardLimit)
{
	return GetParamResponseInt32(eProfileParamSoftLimitForThreshold, forwardLimit);
}
CTR_Code CanTalonSRX::GetReverseSoftLimit(int & reverseLimit)
{
	return GetParamResponseInt32(eProfileParamSoftLimitRevThreshold, reverseLimit);
}
CTR_Code CanTalonSRX::GetForwardSoftEnable(int & enable)
{
	return GetParamResponseInt32(eProfileParamSoftLimitForEnable, enable);
}
CTR_Code CanTalonSRX::GetReverseSoftEnable(int & enable)
{
	return GetParamResponseInt32(eProfileParamSoftLimitRevEnable, enable);
}
/**
 * Change the periodMs of a TALON's status frame.  See kStatusFrame_* enums for what's available.
 */
CTR_Code CanTalonSRX::SetStatusFrameRate(unsigned frameEnum, unsigned periodMs)
{
	int32_t status = 0;
	/* bounds check the period */
	if(periodMs < 1)
		periodMs = 1;
	else if (periodMs > 255)
		periodMs = 255;
  uint8_t period = (uint8_t)periodMs;
	/* tweak just the status messsage rate the caller cares about */
	switch(frameEnum){
		case kStatusFrame_General:
			_statusRateMs[0] = period;
			break;
		case kStatusFrame_Feedback:
			_statusRateMs[1] = period;
			break;
		case kStatusFrame_Encoder:
			_statusRateMs[2] = period;
			break;
		case kStatusFrame_AnalogTempVbat:
			_statusRateMs[3] = period;
			break;
	}
	/* build our request frame */
	TALON_Control_2_Rates_OneShot_t frame;
	memset(&frame,0,sizeof(frame));
	frame.Status1Ms = _statusRateMs[0];
	frame.Status2Ms = _statusRateMs[1];
	frame.Status3Ms = _statusRateMs[2];
	frame.Status4Ms = _statusRateMs[3];
	FRC_NetworkCommunication_CANSessionMux_sendMessage(CONTROL_2 | GetDeviceNumber(), (const uint8_t*)&frame, sizeof(frame), 0, &status);
	if(status)
		return CTR_TxFailed;
	return CTR_OKAY;
}
/**
 * Clear all sticky faults in TALON.
 */
CTR_Code CanTalonSRX::ClearStickyFaults()
{
	int32_t status = 0;
	/* build request frame */
	TALON_Control_3_ClearFlags_OneShot_t frame;
	memset(&frame,0,sizeof(frame));
	frame.ClearStickyFaults = 1;
	FRC_NetworkCommunication_CANSessionMux_sendMessage(CONTROL_3 | GetDeviceNumber(), (const uint8_t*)&frame, sizeof(frame), 0, &status);
	if(status)
		return CTR_TxFailed;
	return CTR_OKAY;
}
/*------------------------ auto generated.  This API is optimal since it uses the fire-and-forget CAN interface ----------------------*/
/*------------------------ These signals should cover the majority of all use cases. ----------------------------------*/
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
CTR_Code CanTalonSRX::GetAppliedThrottle(int &param)
{
	GET_STATUS1();
	int32_t raw = 0;
	raw |= rx->AppliedThrottle_h3;
	raw <<= 8;
	raw |= rx->AppliedThrottle_l8;
	raw <<= (32-11); /* sign extend */
	raw >>= (32-11); /* sign extend */
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetCloseLoopErr(int &param)
{
	GET_STATUS1();
	int32_t raw = 0;
	raw |= rx->CloseLoopErrH;
	raw <<= 8;
	raw |= rx->CloseLoopErrM;
	raw <<= 8;
	raw |= rx->CloseLoopErrL;
	raw <<= (32-24); /* sign extend */
	raw >>= (32-24); /* sign extend */
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetFeedbackDeviceSelect(int &param)
{
	GET_STATUS1();
	param = rx->FeedbackDeviceSelect;
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
CTR_Code CanTalonSRX::GetSensorPosition(int &param)
{
	GET_STATUS2();
	int32_t raw = 0;
	raw |= rx->SensorPositionH;
	raw <<= 8;
	raw |= rx->SensorPositionM;
	raw <<= 8;
	raw |= rx->SensorPositionL;
	raw <<= (32-24); /* sign extend */
	raw >>= (32-24); /* sign extend */
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetSensorVelocity(int &param)
{
	GET_STATUS2();
	int32_t raw = 0;
	raw |= rx->SensorVelocityH;
	raw <<= 8;
	raw |= rx->SensorVelocityL;
	raw <<= (32-16); /* sign extend */
	raw >>= (32-16); /* sign extend */
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
	int32_t raw = 0;
	raw |= rx->EncPositionH;
	raw <<= 8;
	raw |= rx->EncPositionM;
	raw <<= 8;
	raw |= rx->EncPositionL;
	raw <<= (32-24); /* sign extend */
	raw >>= (32-24); /* sign extend */
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetEncVel(int &param)
{
	GET_STATUS3();
	int32_t raw = 0;
	raw |= rx->EncVelH;
	raw <<= 8;
	raw |= rx->EncVelL;
	raw <<= (32-16); /* sign extend */
	raw >>= (32-16); /* sign extend */
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
	int32_t raw = 0;
	raw |= rx->AnalogInWithOvH;
	raw <<= 8;
	raw |= rx->AnalogInWithOvM;
	raw <<= 8;
	raw |= rx->AnalogInWithOvL;
	raw <<= (32-24); /* sign extend */
	raw >>= (32-24); /* sign extend */
	param = (int)raw;
	return rx.err;
}
CTR_Code CanTalonSRX::GetAnalogInVel(int &param)
{
	GET_STATUS4();
	int32_t raw = 0;
	raw |= rx->AnalogInVelH;
	raw <<= 8;
	raw |= rx->AnalogInVelL;
	raw <<= (32-16); /* sign extend */
	raw >>= (32-16); /* sign extend */
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
CTR_Code CanTalonSRX::SetDemand(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->DemandH = param>>16;
	toFill->DemandM = param>>8;
	toFill->DemandL = param>>0;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetOverrideLimitSwitchEn(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->OverrideLimitSwitchEn = param;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetFeedbackDeviceSelect(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->FeedbackDeviceSelect = param;
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
CTR_Code CanTalonSRX::SetOverrideBrakeType(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->OverrideBrakeType = param;
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
/**
 * @param modeSelect selects which mode.
 * @param demand setpt or throttle or masterId to follow.
 * @return error code, 0 iff successful.
 * This function has the advantage of atomically setting mode and demand.
 */
CTR_Code CanTalonSRX::SetModeSelect(int modeSelect,int demand)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->ModeSelect = modeSelect;
	toFill->DemandH = demand>>16;
	toFill->DemandM = demand>>8;
	toFill->DemandL = demand>>0;
	FlushTx(toFill);
	return CTR_OKAY;
}
CTR_Code CanTalonSRX::SetProfileSlotSelect(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->ProfileSlotSelect = param;
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
CTR_Code CanTalonSRX::SetRevFeedbackSensor(int param)
{
	CtreCanNode::txTask<TALON_Control_1_General_10ms_t> toFill = GetTx<TALON_Control_1_General_10ms_t>(CONTROL_1 | GetDeviceNumber());
	if (toFill.IsEmpty()) return CTR_UnexpectedArbId;
	toFill->RevFeedbackSensor = param ? 1 : 0;
	FlushTx(toFill);
	return CTR_OKAY;
}
//------------------ C interface --------------------------------------------//
extern "C" {
void *c_TalonSRX_Create(int deviceNumber, int controlPeriodMs)
{
	return new CanTalonSRX(deviceNumber, controlPeriodMs);
}
void c_TalonSRX_Destroy(void *handle)
{
	delete (CanTalonSRX*)handle;
}
CTR_Code c_TalonSRX_SetParam(void *handle, int paramEnum, double value)
{
	return ((CanTalonSRX*)handle)->SetParam((CanTalonSRX::param_t)paramEnum, value);
}
CTR_Code c_TalonSRX_RequestParam(void *handle, int paramEnum)
{
	return ((CanTalonSRX*)handle)->RequestParam((CanTalonSRX::param_t)paramEnum);
}
CTR_Code c_TalonSRX_GetParamResponse(void *handle, int paramEnum, double *value)
{
	return ((CanTalonSRX*)handle)->GetParamResponse((CanTalonSRX::param_t)paramEnum, *value);
}
CTR_Code c_TalonSRX_GetParamResponseInt32(void *handle, int paramEnum, int *value)
{
	return ((CanTalonSRX*)handle)->GetParamResponseInt32((CanTalonSRX::param_t)paramEnum, *value);
}
CTR_Code c_TalonSRX_SetStatusFrameRate(void *handle, unsigned frameEnum, unsigned periodMs)
{
	return ((CanTalonSRX*)handle)->SetStatusFrameRate(frameEnum, periodMs);
}
CTR_Code c_TalonSRX_ClearStickyFaults(void *handle)
{
	return ((CanTalonSRX*)handle)->ClearStickyFaults();
}
CTR_Code c_TalonSRX_GetFault_OverTemp(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFault_OverTemp(*param);
}
CTR_Code c_TalonSRX_GetFault_UnderVoltage(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFault_UnderVoltage(*param);
}
CTR_Code c_TalonSRX_GetFault_ForLim(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFault_ForLim(*param);
}
CTR_Code c_TalonSRX_GetFault_RevLim(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFault_RevLim(*param);
}
CTR_Code c_TalonSRX_GetFault_HardwareFailure(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFault_HardwareFailure(*param);
}
CTR_Code c_TalonSRX_GetFault_ForSoftLim(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFault_ForSoftLim(*param);
}
CTR_Code c_TalonSRX_GetFault_RevSoftLim(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFault_RevSoftLim(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_OverTemp(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetStckyFault_OverTemp(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_UnderVoltage(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetStckyFault_UnderVoltage(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_ForLim(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetStckyFault_ForLim(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_RevLim(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetStckyFault_RevLim(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_ForSoftLim(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetStckyFault_ForSoftLim(*param);
}
CTR_Code c_TalonSRX_GetStckyFault_RevSoftLim(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetStckyFault_RevSoftLim(*param);
}
CTR_Code c_TalonSRX_GetAppliedThrottle(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetAppliedThrottle(*param);
}
CTR_Code c_TalonSRX_GetCloseLoopErr(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetCloseLoopErr(*param);
}
CTR_Code c_TalonSRX_GetFeedbackDeviceSelect(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFeedbackDeviceSelect(*param);
}
CTR_Code c_TalonSRX_GetModeSelect(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetModeSelect(*param);
}
CTR_Code c_TalonSRX_GetLimitSwitchEn(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetLimitSwitchEn(*param);
}
CTR_Code c_TalonSRX_GetLimitSwitchClosedFor(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetLimitSwitchClosedFor(*param);
}
CTR_Code c_TalonSRX_GetLimitSwitchClosedRev(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetLimitSwitchClosedRev(*param);
}
CTR_Code c_TalonSRX_GetSensorPosition(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetSensorPosition(*param);
}
CTR_Code c_TalonSRX_GetSensorVelocity(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetSensorVelocity(*param);
}
CTR_Code c_TalonSRX_GetCurrent(void *handle, double *param)
{
	return ((CanTalonSRX*)handle)->GetCurrent(*param);
}
CTR_Code c_TalonSRX_GetBrakeIsEnabled(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetBrakeIsEnabled(*param);
}
CTR_Code c_TalonSRX_GetEncPosition(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetEncPosition(*param);
}
CTR_Code c_TalonSRX_GetEncVel(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetEncVel(*param);
}
CTR_Code c_TalonSRX_GetEncIndexRiseEvents(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetEncIndexRiseEvents(*param);
}
CTR_Code c_TalonSRX_GetQuadApin(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetQuadApin(*param);
}
CTR_Code c_TalonSRX_GetQuadBpin(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetQuadBpin(*param);
}
CTR_Code c_TalonSRX_GetQuadIdxpin(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetQuadIdxpin(*param);
}
CTR_Code c_TalonSRX_GetAnalogInWithOv(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetAnalogInWithOv(*param);
}
CTR_Code c_TalonSRX_GetAnalogInVel(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetAnalogInVel(*param);
}
CTR_Code c_TalonSRX_GetTemp(void *handle, double *param)
{
	return ((CanTalonSRX*)handle)->GetTemp(*param);
}
CTR_Code c_TalonSRX_GetBatteryV(void *handle, double *param)
{
	return ((CanTalonSRX*)handle)->GetBatteryV(*param);
}
CTR_Code c_TalonSRX_GetResetCount(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetResetCount(*param);
}
CTR_Code c_TalonSRX_GetResetFlags(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetResetFlags(*param);
}
CTR_Code c_TalonSRX_GetFirmVers(void *handle, int *param)
{
	return ((CanTalonSRX*)handle)->GetFirmVers(*param);
}
CTR_Code c_TalonSRX_SetDemand(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetDemand(param);
}
CTR_Code c_TalonSRX_SetOverrideLimitSwitchEn(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetOverrideLimitSwitchEn(param);
}
CTR_Code c_TalonSRX_SetFeedbackDeviceSelect(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetFeedbackDeviceSelect(param);
}
CTR_Code c_TalonSRX_SetRevMotDuringCloseLoopEn(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetRevMotDuringCloseLoopEn(param);
}
CTR_Code c_TalonSRX_SetOverrideBrakeType(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetOverrideBrakeType(param);
}
CTR_Code c_TalonSRX_SetModeSelect(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetModeSelect(param);
}
CTR_Code c_TalonSRX_SetModeSelect2(void *handle, int modeSelect, int demand)
{
	return ((CanTalonSRX*)handle)->SetModeSelect(modeSelect, demand);
}
CTR_Code c_TalonSRX_SetProfileSlotSelect(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetProfileSlotSelect(param);
}
CTR_Code c_TalonSRX_SetRampThrottle(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetRampThrottle(param);
}
CTR_Code c_TalonSRX_SetRevFeedbackSensor(void *handle, int param)
{
	return ((CanTalonSRX*)handle)->SetRevFeedbackSensor(param);
}
}
