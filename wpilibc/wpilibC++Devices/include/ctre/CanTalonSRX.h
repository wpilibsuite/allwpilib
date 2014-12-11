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
#ifndef CanTalonSRX_H_
#define CanTalonSRX_H_
#include "ctre.h"				//BIT Defines + Typedefs
#include "CtreCanNode.h"
#include <NetworkCommunication/CANSessionMux.h>	//CAN Comm
#include <map>
class CanTalonSRX : public CtreCanNode
{
private:

	/** just in case user wants to modify periods of certain status frames.
	 * 	Default the vars to match the firmware default. */
	uint32_t _statusRateMs[4];
	//---------------------- Vars for opening a CAN stream if caller needs signals that require soliciting */
	uint32_t _can_h; 	//!< Session handle for catching response params.
	int32_t _can_stat; //!< Session handle status.
	struct tCANStreamMessage _msgBuff[20];
	static int const kMsgCapacity	= 20;
	typedef std::map<uint32_t, uint32_t> sigs_t;
	sigs_t _sigs; //!< Catches signal updates that are solicited.  Expect this to be very few.
	void OpenSessionIfNeedBe();
	void ProcessStreamMessages();
	/**
	 * Send a one shot frame to set an arbitrary signal.
	 * Most signals are in the control frame so avoid using this API unless you have to.
	 * Use this api for...
	 * -A motor controller profile signal eProfileParam_XXXs.  These are backed up in flash.  If you are gain-scheduling then call this periodically.
	 * -Default brake and limit switch signals... eOnBoot_XXXs.  Avoid doing this, use the override signals in the control frame.
	 * Talon will automatically send a PARAM_RESPONSE after the set, so GetParamResponse will catch the latest value after a couple ms.
	 */
	CTR_Code SetParamRaw(uint32_t paramEnum, int32_t rawBits);
	/**
	 * Checks cached CAN frames and updating solicited signals.
	 */
	CTR_Code GetParamResponseRaw(uint32_t paramEnum, int32_t & rawBits);
public:
	static const int kDefaultControlPeriodMs = 10; //!< default control update rate is 10ms.
	CanTalonSRX(int deviceNumber = 0,int controlPeriodMs = kDefaultControlPeriodMs);
	~CanTalonSRX();
	void Set(double value);
	/* mode select enumerations */
	static const int kMode_DutyCycle = 0; //!< Demand is 11bit signed duty cycle [-1023,1023].
	static const int kMode_PositionCloseLoop = 1; //!< Position PIDF.
	static const int kMode_VelocityCloseLoop = 2; //!< Velocity PIDF.
	static const int kMode_CurrentCloseLoop = 3; //!< Current close loop - not done.
	static const int kMode_VoltCompen = 4; //!< Voltage Compensation Mode - not done.  Demand is fixed pt target 8.8 volts.
	static const int kMode_SlaveFollower = 5; //!< Demand is the 6 bit Device ID of the 'master' TALON SRX.
	static const int kMode_NoDrive = 15; //!< Zero the output (honors brake/coast) regardless of demand.  Might be useful if we need to change modes but can't atomically change all the signals we want in between.
	/* limit switch enumerations */
	static const int kLimitSwitchOverride_UseDefaultsFromFlash = 1;
	static const int kLimitSwitchOverride_DisableFwd_DisableRev = 4;
	static const int kLimitSwitchOverride_DisableFwd_EnableRev = 5;
	static const int kLimitSwitchOverride_EnableFwd_DisableRev = 6;
	static const int kLimitSwitchOverride_EnableFwd_EnableRev = 7;
	/* brake override enumerations */
	static const int kBrakeOverride_UseDefaultsFromFlash = 0;
	static const int kBrakeOverride_OverrideCoast = 1;
	static const int kBrakeOverride_OverrideBrake = 2;
	/* feedback device enumerations */
	static const int kFeedbackDev_DigitalQuadEnc=0;
	static const int kFeedbackDev_AnalogPot=2;
	static const int kFeedbackDev_AnalogEncoder=3;
	static const int kFeedbackDev_CountEveryRisingEdge=4;
	static const int kFeedbackDev_CountEveryFallingEdge=5;
	static const int kFeedbackDev_PosIsPulseWidth=8;
	/* ProfileSlotSelect enumerations*/
	static const int kProfileSlotSelect_Slot0 = 0;
	static const int kProfileSlotSelect_Slot1 = 1;
    /* status frame rate types */
    static const int kStatusFrame_General = 0;
    static const int kStatusFrame_Feedback = 1;
    static const int kStatusFrame_Encoder = 2;
    static const int kStatusFrame_AnalogTempVbat = 3;
	/**
	 * Signal enumeration for generic signal access.
	 * Although every signal is enumerated, only use this for traffic that must be solicited.
	 * Use the auto generated getters/setters at bottom of this header as much as possible.
	 */
	typedef enum _param_t{
		eProfileParamSlot0_P=1,
		eProfileParamSlot0_I=2,
		eProfileParamSlot0_D=3,
		eProfileParamSlot0_F=4,
		eProfileParamSlot0_IZone=5,
		eProfileParamSlot0_CloseLoopRampRate=6,
		eProfileParamSlot1_P=11,
		eProfileParamSlot1_I=12,
		eProfileParamSlot1_D=13,
		eProfileParamSlot1_F=14,
		eProfileParamSlot1_IZone=15,
		eProfileParamSlot1_CloseLoopRampRate=16,
		eProfileParamSoftLimitForThreshold=21,
		eProfileParamSoftLimitRevThreshold=22,
		eProfileParamSoftLimitForEnable=23,
		eProfileParamSoftLimitRevEnable=24,
		eOnBoot_BrakeMode=31,
		eOnBoot_LimitSwitch_Forward_NormallyClosed=32,
		eOnBoot_LimitSwitch_Reverse_NormallyClosed=33,
		eOnBoot_LimitSwitch_Forward_Disable=34,
		eOnBoot_LimitSwitch_Reverse_Disable=35,
		eFault_OverTemp=41,
		eFault_UnderVoltage=42,
		eFault_ForLim=43,
		eFault_RevLim=44,
		eFault_HardwareFailure=45,
		eFault_ForSoftLim=46,
		eFault_RevSoftLim=47,
		eStckyFault_OverTemp=48,
		eStckyFault_UnderVoltage=49,
		eStckyFault_ForLim=50,
		eStckyFault_RevLim=51,
		eStckyFault_ForSoftLim=52,
		eStckyFault_RevSoftLim=53,
		eAppliedThrottle=61,
		eCloseLoopErr=62,
		eFeedbackDeviceSelect=63,
		eRevMotDuringCloseLoopEn=64,
		eModeSelect=65,
		eProfileSlotSelect=66,
		eRampThrottle=67,
		eRevFeedbackSensor=68,
		eLimitSwitchEn=69,
		eLimitSwitchClosedFor=70,
		eLimitSwitchClosedRev=71,
		eSensorPosition=73,
		eSensorVelocity=74,
		eCurrent=75,
		eBrakeIsEnabled=76,
		eEncPosition=77,
		eEncVel=78,
		eEncIndexRiseEvents=79,
		eQuadApin=80,
		eQuadBpin=81,
		eQuadIdxpin=82,
		eAnalogInWithOv=83,
		eAnalogInVel=84,
		eTemp=85,
		eBatteryV=86,
		eResetCount=87,
		eResetFlags=88,
		eFirmVers=89,
		eSettingsChanged=90,
		eQuadFilterEn=91,	
		ePidIaccum=93,
	}param_t;
    /*---------------------setters and getters that use the solicated param request/response-------------*//**
     * Send a one shot frame to set an arbitrary signal.
     * Most signals are in the control frame so avoid using this API unless you have to.
     * Use this api for...
     * -A motor controller profile signal eProfileParam_XXXs.  These are backed up in flash.  If you are gain-scheduling then call this periodically.
     * -Default brake and limit switch signals... eOnBoot_XXXs.  Avoid doing this, use the override signals in the control frame.
     * Talon will automatically send a PARAM_RESPONSE after the set, so GetParamResponse will catch the latest value after a couple ms.
     */
	CTR_Code SetParam(param_t paramEnum, double value);
	/**
	 * Asks TALON to immedietely respond with signal value.  This API is only used for signals that are not sent periodically.
	 * This can be useful for reading params that rarely change like Limit Switch settings and PIDF values.
	  * @param param to request.
	 */
	CTR_Code RequestParam(param_t paramEnum);
	CTR_Code GetParamResponse(param_t paramEnum, double & value);
	CTR_Code GetParamResponseInt32(param_t paramEnum, int & value);
    /*----- getters and setters that use param request/response. These signals are backed up in flash and will survive a power cycle. ---------*/
	/*----- If your application requires changing these values consider using both slots and switch between slot0 <=> slot1. ------------------*/
	/*----- If your application requires changing these signals frequently then it makes sense to leverage this API. --------------------------*/
	/*----- Getters don't block, so it may require several calls to get the latest value. --------------------------*/
	CTR_Code SetPgain(unsigned slotIdx,double gain);
	CTR_Code SetIgain(unsigned slotIdx,double gain);
	CTR_Code SetDgain(unsigned slotIdx,double gain);
	CTR_Code SetFgain(unsigned slotIdx,double gain);
	CTR_Code SetIzone(unsigned slotIdx,int zone);
	CTR_Code SetCloseLoopRampRate(unsigned slotIdx,int closeLoopRampRate);
	CTR_Code SetSensorPosition(int pos);
	CTR_Code SetForwardSoftLimit(int forwardLimit);
	CTR_Code SetReverseSoftLimit(int reverseLimit);
	CTR_Code SetForwardSoftEnable(int enable);
	CTR_Code SetReverseSoftEnable(int enable);
	CTR_Code GetPgain(unsigned slotIdx,double & gain);
	CTR_Code GetIgain(unsigned slotIdx,double & gain);
	CTR_Code GetDgain(unsigned slotIdx,double & gain);
	CTR_Code GetFgain(unsigned slotIdx,double & gain);
	CTR_Code GetIzone(unsigned slotIdx,int & zone);
	CTR_Code GetCloseLoopRampRate(unsigned slotIdx,int & closeLoopRampRate);
	CTR_Code GetForwardSoftLimit(int & forwardLimit);
	CTR_Code GetReverseSoftLimit(int & reverseLimit);
	CTR_Code GetForwardSoftEnable(int & enable);
	CTR_Code GetReverseSoftEnable(int & enable);
	/**
	 * Change the periodMs of a TALON's status frame.  See kStatusFrame_* enums for what's available.
	 */
	CTR_Code SetStatusFrameRate(unsigned frameEnum, unsigned periodMs);
	/**
	 * Clear all sticky faults in TALON.
	 */
	CTR_Code ClearStickyFaults();
    /*------------------------ auto generated.  This API is optimal since it uses the fire-and-forget CAN interface ----------------------*/
    /*------------------------ These signals should cover the majority of all use cases. ----------------------------------*/
	CTR_Code GetFault_OverTemp(int &param);
	CTR_Code GetFault_UnderVoltage(int &param);
	CTR_Code GetFault_ForLim(int &param);
	CTR_Code GetFault_RevLim(int &param);
	CTR_Code GetFault_HardwareFailure(int &param);
	CTR_Code GetFault_ForSoftLim(int &param);
	CTR_Code GetFault_RevSoftLim(int &param);
	CTR_Code GetStckyFault_OverTemp(int &param);
	CTR_Code GetStckyFault_UnderVoltage(int &param);
	CTR_Code GetStckyFault_ForLim(int &param);
	CTR_Code GetStckyFault_RevLim(int &param);
	CTR_Code GetStckyFault_ForSoftLim(int &param);
	CTR_Code GetStckyFault_RevSoftLim(int &param);
	CTR_Code GetAppliedThrottle(int &param);
	CTR_Code GetCloseLoopErr(int &param);
	CTR_Code GetFeedbackDeviceSelect(int &param);
	CTR_Code GetModeSelect(int &param);
	CTR_Code GetLimitSwitchEn(int &param);
	CTR_Code GetLimitSwitchClosedFor(int &param);
	CTR_Code GetLimitSwitchClosedRev(int &param);
	CTR_Code GetSensorPosition(int &param);
	CTR_Code GetSensorVelocity(int &param);
	CTR_Code GetCurrent(double &param);
	CTR_Code GetBrakeIsEnabled(int &param);
	CTR_Code GetEncPosition(int &param);
	CTR_Code GetEncVel(int &param);
	CTR_Code GetEncIndexRiseEvents(int &param);
	CTR_Code GetQuadApin(int &param);
	CTR_Code GetQuadBpin(int &param);
	CTR_Code GetQuadIdxpin(int &param);
	CTR_Code GetAnalogInWithOv(int &param);
	CTR_Code GetAnalogInVel(int &param);
	CTR_Code GetTemp(double &param);
	CTR_Code GetBatteryV(double &param);
	CTR_Code GetResetCount(int &param);
	CTR_Code GetResetFlags(int &param);
	CTR_Code GetFirmVers(int &param);
	CTR_Code SetDemand(int param);
	CTR_Code SetOverrideLimitSwitchEn(int param);
	CTR_Code SetFeedbackDeviceSelect(int param);
	CTR_Code SetRevMotDuringCloseLoopEn(int param);
	CTR_Code SetOverrideBrakeType(int param);
	CTR_Code SetModeSelect(int param);
	CTR_Code SetModeSelect(int modeSelect,int demand);
	CTR_Code SetProfileSlotSelect(int param);
	CTR_Code SetRampThrottle(int param);
	CTR_Code SetRevFeedbackSensor(int param);
};
extern "C" {
	void *c_TalonSRX_Create(int deviceNumber, int controlPeriodMs);
	void c_TalonSRX_Destroy(void *handle);
	CTR_Code c_TalonSRX_SetParam(void *handle, int paramEnum, double value);
	CTR_Code c_TalonSRX_RequestParam(void *handle, int paramEnum);
	CTR_Code c_TalonSRX_GetParamResponse(void *handle, int paramEnum, double *value);
	CTR_Code c_TalonSRX_GetParamResponseInt32(void *handle, int paramEnum, int *value);
	CTR_Code c_TalonSRX_SetStatusFrameRate(void *handle, unsigned frameEnum, unsigned periodMs);
	CTR_Code c_TalonSRX_ClearStickyFaults(void *handle);
	CTR_Code c_TalonSRX_GetFault_OverTemp(void *handle, int *param);
	CTR_Code c_TalonSRX_GetFault_UnderVoltage(void *handle, int *param);
	CTR_Code c_TalonSRX_GetFault_ForLim(void *handle, int *param);
	CTR_Code c_TalonSRX_GetFault_RevLim(void *handle, int *param);
	CTR_Code c_TalonSRX_GetFault_HardwareFailure(void *handle, int *param);
	CTR_Code c_TalonSRX_GetFault_ForSoftLim(void *handle, int *param);
	CTR_Code c_TalonSRX_GetFault_RevSoftLim(void *handle, int *param);
	CTR_Code c_TalonSRX_GetStckyFault_OverTemp(void *handle, int *param);
	CTR_Code c_TalonSRX_GetStckyFault_UnderVoltage(void *handle, int *param);
	CTR_Code c_TalonSRX_GetStckyFault_ForLim(void *handle, int *param);
	CTR_Code c_TalonSRX_GetStckyFault_RevLim(void *handle, int *param);
	CTR_Code c_TalonSRX_GetStckyFault_ForSoftLim(void *handle, int *param);
	CTR_Code c_TalonSRX_GetStckyFault_RevSoftLim(void *handle, int *param);
	CTR_Code c_TalonSRX_GetAppliedThrottle(void *handle, int *param);
	CTR_Code c_TalonSRX_GetCloseLoopErr(void *handle, int *param);
	CTR_Code c_TalonSRX_GetFeedbackDeviceSelect(void *handle, int *param);
	CTR_Code c_TalonSRX_GetModeSelect(void *handle, int *param);
	CTR_Code c_TalonSRX_GetLimitSwitchEn(void *handle, int *param);
	CTR_Code c_TalonSRX_GetLimitSwitchClosedFor(void *handle, int *param);
	CTR_Code c_TalonSRX_GetLimitSwitchClosedRev(void *handle, int *param);
	CTR_Code c_TalonSRX_GetSensorPosition(void *handle, int *param);
	CTR_Code c_TalonSRX_GetSensorVelocity(void *handle, int *param);
	CTR_Code c_TalonSRX_GetCurrent(void *handle, double *param);
	CTR_Code c_TalonSRX_GetBrakeIsEnabled(void *handle, int *param);
	CTR_Code c_TalonSRX_GetEncPosition(void *handle, int *param);
	CTR_Code c_TalonSRX_GetEncVel(void *handle, int *param);
	CTR_Code c_TalonSRX_GetEncIndexRiseEvents(void *handle, int *param);
	CTR_Code c_TalonSRX_GetQuadApin(void *handle, int *param);
	CTR_Code c_TalonSRX_GetQuadBpin(void *handle, int *param);
	CTR_Code c_TalonSRX_GetQuadIdxpin(void *handle, int *param);
	CTR_Code c_TalonSRX_GetAnalogInWithOv(void *handle, int *param);
	CTR_Code c_TalonSRX_GetAnalogInVel(void *handle, int *param);
	CTR_Code c_TalonSRX_GetTemp(void *handle, double *param);
	CTR_Code c_TalonSRX_GetBatteryV(void *handle, double *param);
	CTR_Code c_TalonSRX_GetResetCount(void *handle, int *param);
	CTR_Code c_TalonSRX_GetResetFlags(void *handle, int *param);
	CTR_Code c_TalonSRX_GetFirmVers(void *handle, int *param);
	CTR_Code c_TalonSRX_SetDemand(void *handle, int param);
	CTR_Code c_TalonSRX_SetOverrideLimitSwitchEn(void *handle, int param);
	CTR_Code c_TalonSRX_SetFeedbackDeviceSelect(void *handle, int param);
	CTR_Code c_TalonSRX_SetRevMotDuringCloseLoopEn(void *handle, int param);
	CTR_Code c_TalonSRX_SetOverrideBrakeType(void *handle, int param);
	CTR_Code c_TalonSRX_SetModeSelect(void *handle, int param);
	CTR_Code c_TalonSRX_SetProfileSlotSelect(void *handle, int param);
	CTR_Code c_TalonSRX_SetRampThrottle(void *handle, int param);
	CTR_Code c_TalonSRX_SetRevFeedbackSensor(void *handle, int param);
}
#endif

