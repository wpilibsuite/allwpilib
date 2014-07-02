#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include "PCM.h"
#include "NetworkCommunication/CANSessionMux.h"
#include <string.h> // memset
#include <unistd.h> // usleep
/* This can be a constant, as long as nobody needs to updatie solenoids within
    1/50 of a second. */
static const INT32 kCANPeriod = 20;

#define STATUS_1  			0x9041400
#define STATUS_SOL_FAULTS  	0x9041440
#define STATUS_DEBUG  		0x9041480

#define EXPECTED_RESPONSE_TIMEOUT_MS	(50)
#define GET_PCM_STATUS()			CtreCanNode::recMsg<PcmStatus_t> 		rx = GetRx<PcmStatus_t>			(STATUS_1,EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_PCM_SOL_FAULTS()		CtreCanNode::recMsg<PcmStatusFault_t> 	rx = GetRx<PcmStatusFault_t>	(STATUS_SOL_FAULTS,EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_PCM_DEBUG()				CtreCanNode::recMsg<PcmDebug_t> 		rx = GetRx<PcmDebug_t>			(STATUS_DEBUG,EXPECTED_RESPONSE_TIMEOUT_MS)

#define CONTROL_1 			0x09041C00

/* encoder/decoders */
typedef struct _PcmStatus_t{
	/* Byte 0 */
	unsigned SolenoidBits:8;
	/* Byte 1 */
	unsigned compressorOn:1;
	unsigned stickyFaultFuseTripped:1;
	unsigned stickyFaultCompCurrentTooHigh:1;
	unsigned faultCompCurrentTooHigh:1;
	unsigned faultFuseTripped:1;
	unsigned faultHardwareFailure:1;
	unsigned isCloseloopEnabled:1;
	unsigned pressureSwitchEn:1;
	/* Byte 2*/
	unsigned battVoltage:8;
	/* Byte 3 */
	unsigned solenoidVoltageTop8:8;
	/* Byte 4 */
	unsigned compressorCurrentTop6:6;
	unsigned solenoidVoltageBtm2:2;
	/* Byte 5 */
	unsigned reserved:2;
	unsigned moduleEnabled:1;
	unsigned closedLoopOutput:1;
	unsigned compressorCurrentBtm4:4;
	/* Byte 6 */
	unsigned tokenSeedTop8:8;
	/* Byte 7 */
	unsigned tokenSeedBtm8:8;
}PcmStatus_t;

typedef struct _PcmControl_t{
	/* Byte 0 */
	unsigned tokenTop8:8;
	/* Byte 1 */
	unsigned tokenBtm8:8;
	/* Byte 2 */
	unsigned solenoidBits:8;
	/* Byte 3*/
	unsigned reserved:4;
	unsigned closeLoopOutput:1;
	unsigned compressorOn:1;
	unsigned closedLoopEnable:1;
	unsigned clearStickyFaults:1;
}PcmControl_t;

typedef struct _PcmStatusFault_t{
	/* Byte 0 */
	unsigned SolenoidBlacklist:8;
	/* Byte 1 */
	unsigned reserved1:8;
	unsigned reserved2:8;
	unsigned reserved3:8;
	unsigned reserved4:8;
	unsigned reserved5:8;
	unsigned reserved6:8;
	unsigned reserved7:8;
}PcmStatusFault_t;

typedef struct _PcmDebug_t{
	unsigned tokFailsTop8:8;
	unsigned tokFailsBtm8:8;
	unsigned lastFailedTokTop8:8;
	unsigned lastFailedTokBtm8:8;
	unsigned tokSuccessTop8:8;
	unsigned tokSuccessBtm8:8;
}PcmDebug_t;


/* PCM Constructor - Clears all vars, establishes default settings, starts PCM background process
 *
 * @Return	-	void
 *
 * @Param 	-	deviceNumber	- 	Device ID of PCM to be controlled
 */
PCM::PCM(UINT8 deviceNumber): CtreCanNode(deviceNumber)
{
	RegisterRx(STATUS_1 | deviceNumber );
	RegisterRx(STATUS_SOL_FAULTS | deviceNumber );
	RegisterRx(STATUS_DEBUG | deviceNumber );
	RegisterTx(CONTROL_1 | deviceNumber, kCANPeriod);
	/* enable close loop */
	CtreCanNode::txTask<PcmControl_t> toFill = GetTx<PcmControl_t>(CONTROL_1 | GetDeviceNumber());
	toFill->closedLoopEnable = 1;
}
/* PCM D'tor
 */
PCM::~PCM()
{

}

/* Set PCM solenoid state
 *
 * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
 *
 * @Param 	-	idx			- 	ID of solenoid (0-7)
 * @Param 	-	en			- 	Enable / Disable identified solenoid
 */
CTR_Code PCM::SetSolenoid(unsigned char idx, bool en)
{
	CtreCanNode::txTask<PcmControl_t> toFill = GetTx<PcmControl_t>(CONTROL_1 | GetDeviceNumber());
	if(toFill.IsEmpty())return CTR_UnexpectedArbId;
	if (en)
		toFill->solenoidBits |= (1ul << (7-idx));
	else
		toFill->solenoidBits &= ~(1ul << (7-idx));
	FlushTx(toFill);
	return CTR_OKAY;
}

/* Clears PCM sticky faults (indicators of past faults
 *
 * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
 *
 * @Param 	-	clr		- 	Clear / do not clear faults
 */
CTR_Code PCM::ClearStickyFaults(bool clr)
{
	CtreCanNode::txTask<PcmControl_t> toFill = GetTx<PcmControl_t>(CONTROL_1 | GetDeviceNumber());
	if(toFill.IsEmpty())return CTR_UnexpectedArbId;
	toFill->clearStickyFaults = clr;
	FlushTx(toFill);
	return CTR_OKAY;
}

/* Enables PCM Closed Loop Control of Compressor via pressure switch
 *
 * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
 *
 * @Param 	-	en		- 	Enable / Disable Closed Loop Control
 */
CTR_Code PCM::SetClosedLoopControl(bool en)
{
	CtreCanNode::txTask<PcmControl_t> toFill = GetTx<PcmControl_t>(CONTROL_1 | GetDeviceNumber());
	if(toFill.IsEmpty())return CTR_UnexpectedArbId;
	toFill->closedLoopEnable = en;
	FlushTx(toFill);
	return CTR_OKAY;
}

/* Get solenoid state
 * 
 * @Return	-	True/False	-	True if solenoid enabled, false otherwise
 * 
 * @Param 	-	idx		- 	ID of solenoid (0-7) to return status of
 */
CTR_Code PCM::GetSolenoid(UINT8 idx, bool &status)
{
	GET_PCM_STATUS();
	status = (rx->SolenoidBits & (1ul<<(7-idx)) ) ? 1 : 0;
	return rx.err;
}

/* Get pressure switch state
 * 
 * @Return	-	True/False	-	True if pressure adequate, false if low
 */
CTR_Code PCM::GetPressure(bool &status)
{
	GET_PCM_STATUS();
	status = (rx->pressureSwitchEn ) ? 1 : 0;
	return rx.err;
}

/* Get compressor state
 * 
 * @Return	-	True/False	-	True if enabled, false if otherwise
 */
CTR_Code PCM::GetCompressor(bool &status)
{
	GET_PCM_STATUS();
	status = (rx->compressorOn);
	return rx.err;
}

/* Get closed loop control state
 * 
 * @Return	-	True/False	-	True if closed loop enabled, false if otherwise
 */
CTR_Code PCM::GetClosedLoopControl(bool &status)
{
	GET_PCM_STATUS();
	status = (rx->isCloseloopEnabled);
	return rx.err;
}

/* Get compressor current draw
 * 
 * @Return	-	Amperes	-	Compressor current 
 */
CTR_Code PCM::GetCompressorCurrent(float &status)
{
	GET_PCM_STATUS();
	uint32_t temp =(rx->compressorCurrentTop6);
	temp <<= 4;
	temp |=  rx->compressorCurrentBtm4;
	status = 20.1612903225806 * temp;
	return rx.err;
}

/* Get voltage across solenoid rail
 * 
 * @Return	-	Volts	-	Voltage across solenoid rail
 */
CTR_Code PCM::GetSolenoidVoltage(float &status)
{
	GET_PCM_STATUS();
	uint32_t raw =(rx->solenoidVoltageTop8);
	raw <<= 2;
	raw |=  rx->solenoidVoltageBtm2;
	status = (double) raw * 24.7800586510264 / 1000;
	return rx.err;
}

/* Get hardware fault value
 * 
 * @Return	-	True/False	-	True if hardware failure detected, false if otherwise
 */
CTR_Code PCM::GetHardwareFault(bool &status)
{
	GET_PCM_STATUS();
	status = rx->faultHardwareFailure;
	return rx.err;
}

/* Get compressor fault value
 * 
 * @Return	-	True/False	-	True if shorted compressor detected, false if otherwise
 */
CTR_Code PCM::GetCompressorFault(bool &status)
{
	GET_PCM_STATUS();
	status = rx->faultCompCurrentTooHigh;
	return rx.err;
}

/* Get solenoid fault value
 * 
 * @Return	-	True/False	-	True if shorted solenoid detected, false if otherwise
 */
CTR_Code PCM::GetSolenoidFault(bool &status)
{
	GET_PCM_STATUS();
	status = rx->faultFuseTripped;
	return rx.err;
}

/* Get compressor sticky fault value
 * 
 * @Return	-	True/False	-	True if solenoid had previously been shorted
 * 								(and sticky fault was not cleared), false if otherwise
 */
CTR_Code PCM::GetCompressorStickyFault(bool &status)
{
	GET_PCM_STATUS();
	status = rx->stickyFaultCompCurrentTooHigh;
	return rx.err;
}

/* Get solenoid sticky fault value
 * 
 * @Return	-	True/False	-	True if compressor had previously been shorted
 * 								(and sticky fault was not cleared), false if otherwise
 */
CTR_Code PCM::GetSolenoidStickyFault(bool &status)
{
	GET_PCM_STATUS();
	status = rx->stickyFaultFuseTripped;
	return rx.err;
}
/* Get battery voltage
 * 
 * @Return	-	Volts	-	Voltage across PCM power ports
 */
CTR_Code PCM::GetBatteryVoltage(float &status)
{
	GET_PCM_STATUS();
	status = (float)rx->battVoltage * ((59.0420332355816) / 1000.0);;
	return rx.err;
}
/* Return status of module enable/disable
 *
 * @Return	-	bool		-	Returns TRUE if PCM is enabled, FALSE if disabled
 */
CTR_Code PCM::isModuleEnabled(bool &status)
{
	GET_PCM_STATUS();
	status = rx->moduleEnabled;
	return rx.err;
}
/* Get number of total failed PCM Control Frame
 * 
 * @Return	-	Failed Control Frames	-	Number of failed control frames (tokenization fails)
 * 
 * @WARNING	-	Return only valid if [SeekDebugFrames] is enabled
 * 				See function SeekDebugFrames
 * 				See function EnableSeekDebugFrames
 */
CTR_Code PCM::GetNumberOfFailedControlFrames(UINT16 &status)
{
	GET_PCM_DEBUG();
	status = rx->tokFailsTop8;
	status <<= 8;
	status |= rx->tokFailsBtm8;
	return rx.err;
}
/* Get raw Solenoid Blacklist
 * 
 * @Return	-	BINARY	-	Raw binary breakdown of Solenoid Blacklist
 * 							BIT7 = Solenoid 1, BIT6 = Solenoid 2, etc.
 * 
 * @WARNING	-	Return only valid if [SeekStatusFaultFrames] is enabled
 * 				See function SeekStatusFaultFrames
 * 				See function EnableSeekStatusFaultFrames
 */
CTR_Code PCM::GetSolenoidBlackList(UINT8 &status)
{
	GET_PCM_SOL_FAULTS();
	status = rx->SolenoidBlacklist;
	return rx.err;
}
/* Get solenoid Blacklist status
 * - Blacklisted solenoids cannot be enabled until PCM is power cycled
 * 
 * @Return	-	True/False	-	True if Solenoid is blacklisted, false if otherwise
 * 
 * @Param	-	idx			-	ID of solenoid [0,7]
 * 
 * @WARNING	-	Return only valid if [SeekStatusFaultFrames] is enabled
 * 				See function SeekStatusFaultFrames
 * 				See function EnableSeekStatusFaultFrames
 */
CTR_Code PCM::IsSolenoidBlacklisted(UINT8 idx, bool &status)
{
	GET_PCM_SOL_FAULTS();
	status = (rx->SolenoidBlacklist & (1ul<<(7-idx)) )? 1 : 0;
	return rx.err;
}
//------------------ C interface --------------------------------------------//
extern "C" {
	void * c_PCM_Init(void) {
		return new PCM();
	}
	CTR_Code c_SetSolenoid(void * handle, unsigned char idx, INT8 param) {
		return ((PCM*) handle)->SetSolenoid(idx, param);
	}
	CTR_Code c_SetClosedLoopControl(void * handle, INT8 param) {
		return ((PCM*) handle)->SetClosedLoopControl(param);
	}
	CTR_Code c_ClearStickyFaults(void * handle, INT8 param) {
		return ((PCM*) handle)->ClearStickyFaults(param);
	}
	CTR_Code c_GetSolenoid(void * handle, UINT8 idx, INT8 * status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetSolenoid(idx, bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetPressure(void * handle, INT8 * status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetPressure(bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetCompressor(void * handle, INT8 * status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetCompressor(bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetClosedLoopControl(void * handle, INT8 * status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetClosedLoopControl(bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetCompressorCurrent(void * handle, float * status) {
		CTR_Code retval = ((PCM*) handle)->GetCompressorCurrent(*status);
		return retval;
	}
	CTR_Code c_GetSolenoidVoltage(void * handle, float*status) {
		return ((PCM*) handle)->GetSolenoidVoltage(*status);
	}
	CTR_Code c_GetHardwareFault(void * handle, INT8*status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetHardwareFault(bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetCompressorFault(void * handle, INT8*status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetCompressorFault(bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetSolenoidFault(void * handle, INT8*status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetSolenoidFault(bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetCompressorStickyFault(void * handle, INT8*status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetCompressorStickyFault(bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetSolenoidStickyFault(void * handle, INT8*status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetSolenoidStickyFault(bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetBatteryVoltage(void * handle, float*status) {
		CTR_Code retval = ((PCM*) handle)->GetBatteryVoltage(*status);
		return retval;
	}
	void c_SetDeviceNumber_PCM(void * handle, UINT8 deviceNumber) {
	}
	CTR_Code c_GetNumberOfFailedControlFrames(void * handle, UINT16*status) {
		return ((PCM*) handle)->GetNumberOfFailedControlFrames(*status);
	}
	CTR_Code c_GetSolenoidBlackList(void * handle, UINT8 *status) {
		return ((PCM*) handle)->GetSolenoidBlackList(*status);
	}
	CTR_Code c_IsSolenoidBlacklisted(void * handle, UINT8 idx, INT8*status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->IsSolenoidBlacklisted(idx, bstatus);
		*status = bstatus;
		return retval;
	}
}
