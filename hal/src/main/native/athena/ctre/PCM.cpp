#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include "PCM.h"
#include "FRC_NetworkCommunication/CANSessionMux.h"
#include <string.h> // memset
/* This can be a constant, as long as nobody needs to update solenoids within
    1/50 of a second. */
static const INT32 kCANPeriod = 20;

#define STATUS_1  			0x9041400
#define STATUS_SOL_FAULTS  	0x9041440
#define STATUS_DEBUG  		0x9041480

#define EXPECTED_RESPONSE_TIMEOUT_MS	(50)
#define GET_PCM_STATUS()			CtreCanNode::recMsg<PcmStatus_t> 		rx = GetRx<PcmStatus_t>			(STATUS_1|GetDeviceNumber(),EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_PCM_SOL_FAULTS()		CtreCanNode::recMsg<PcmStatusFault_t> 	rx = GetRx<PcmStatusFault_t>	(STATUS_SOL_FAULTS|GetDeviceNumber(),EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_PCM_DEBUG()				CtreCanNode::recMsg<PcmDebug_t> 		rx = GetRx<PcmDebug_t>			(STATUS_DEBUG|GetDeviceNumber(),EXPECTED_RESPONSE_TIMEOUT_MS)

#define CONTROL_1 			0x09041C00	/* PCM_Control */
#define CONTROL_2 			0x09041C40	/* PCM_SupplemControl */
#define CONTROL_3 			0x09041C80	/* PcmControlSetOneShotDur_t */

/* encoder/decoders */
typedef struct _PcmStatus_t{
	/* Byte 0 */
	unsigned SolenoidBits:8;
	/* Byte 1 */
	unsigned compressorOn:1;
	unsigned stickyFaultFuseTripped:1;
	unsigned stickyFaultCompCurrentTooHigh:1;
	unsigned faultFuseTripped:1;
	unsigned faultCompCurrentTooHigh:1;
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
	unsigned StickyFault_dItooHigh :1;
	unsigned Fault_dItooHigh :1;
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
	/* Byte 4 */
	unsigned OneShotField_h8:8;
	/* Byte 5 */
	unsigned OneShotField_l8:8;
}PcmControl_t;

typedef struct _PcmControlSetOneShotDur_t{
	uint8_t sol10MsPerUnit[8];
}PcmControlSetOneShotDur_t;

typedef struct _PcmStatusFault_t{
	/* Byte 0 */
	unsigned SolenoidBlacklist:8;
	/* Byte 1 */
	unsigned reserved_bit0 :1;
	unsigned reserved_bit1 :1;
	unsigned reserved_bit2 :1;
	unsigned reserved_bit3 :1;
	unsigned StickyFault_CompNoCurrent :1;
	unsigned Fault_CompNoCurrent :1;
	unsigned StickyFault_SolenoidJumper :1;
	unsigned Fault_SolenoidJumper :1;
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
	SetClosedLoopControl(1);
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
		toFill->solenoidBits |= (1ul << (idx));
	else
		toFill->solenoidBits &= ~(1ul << (idx));
	FlushTx(toFill);
	return CTR_OKAY;
}

/* Set all PCM solenoid states
 *
 * @Return	-	CTR_Code	-	Error code (if any) for setting solenoids
 * @Param 	-	state			Bitfield to set all solenoids to
 */
CTR_Code PCM::SetAllSolenoids(UINT8 state) {
	CtreCanNode::txTask<PcmControl_t> toFill = GetTx<PcmControl_t>(CONTROL_1 | GetDeviceNumber());
	if(toFill.IsEmpty())return CTR_UnexpectedArbId;
	toFill->solenoidBits = state;
	FlushTx(toFill);
	return CTR_OKAY;
}

/* Clears PCM sticky faults (indicators of past faults
 *
 * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
 *
 * @Param 	-	clr		- 	Clear / do not clear faults
 */
CTR_Code PCM::ClearStickyFaults()
{
	int32_t status = 0;
	uint8_t pcmSupplemControl[] = { 0, 0, 0, 0x80 }; /* only bit set is ClearStickyFaults */
	FRC_NetworkCommunication_CANSessionMux_sendMessage(CONTROL_2  | GetDeviceNumber(), pcmSupplemControl, sizeof(pcmSupplemControl), 0, &status);
	if(status)
		return CTR_TxFailed;
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
/* Get solenoid Blacklist status
 * @Return	-	CTR_Code	-	Error code (if any)
 * @Param	-	idx			-	ID of solenoid [0,7] to fire one shot pulse.
 */
CTR_Code PCM::FireOneShotSolenoid(UINT8 idx)
{
	CtreCanNode::txTask<PcmControl_t> toFill = GetTx<PcmControl_t>(CONTROL_1 | GetDeviceNumber());
	if(toFill.IsEmpty())return CTR_UnexpectedArbId;
	/* grab field as it is now */
	uint16_t oneShotField;
	oneShotField = toFill->OneShotField_h8;
	oneShotField <<= 8;
	oneShotField |= toFill->OneShotField_l8;
	/* get the caller's channel */
	uint16_t shift = 2*idx;
	uint16_t mask = 3; /* two bits wide */
	uint8_t chBits = (oneShotField >> shift) & mask;
	/* flip it */
	chBits = (chBits)%3 + 1;
	/* clear out 2bits for this channel*/
	oneShotField &= ~(mask << shift);
	/* put new field in */
	oneShotField |= chBits << shift;
	/* apply field as it is now */
	toFill->OneShotField_h8 = oneShotField >> 8;
	toFill->OneShotField_l8 = oneShotField;
	FlushTx(toFill);
	return CTR_OKAY;
}
/* Configure the pulse width of a solenoid channel for one-shot pulse.
 * Preprogrammed pulsewidth is 10ms resolution and can be between 10ms and
 * 2.55s.
 *
 * @Return	-	CTR_Code	-	Error code (if any)
 * @Param	-	idx			-	ID of solenoid [0,7] to configure.
 * @Param	-	durMs		-	pulse width in ms.
 */
CTR_Code PCM::SetOneShotDurationMs(UINT8 idx,uint32_t durMs)
{
	/* sanity check caller's param */
	if(idx > 7)
		return CTR_InvalidParamValue;
	/* get latest tx frame */
	CtreCanNode::txTask<PcmControlSetOneShotDur_t> toFill = GetTx<PcmControlSetOneShotDur_t>(CONTROL_3 | GetDeviceNumber());
	if(toFill.IsEmpty()){
		/* only send this out if caller wants to do one-shots */
		RegisterTx(CONTROL_3 | _deviceNumber, kCANPeriod);
		/* grab it */
		toFill = GetTx<PcmControlSetOneShotDur_t>(CONTROL_3 | GetDeviceNumber());
	}
	toFill->sol10MsPerUnit[idx] = std::min(durMs/10,(uint32_t)0xFF);
	/* apply the new data bytes */
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
	status = (rx->SolenoidBits & (1ul<<(idx)) ) ? 1 : 0;
	return rx.err;
}

/* Get solenoid state for all solenoids on the PCM
 *
 * @Return	-	Bitfield of solenoid states
 */
CTR_Code PCM::GetAllSolenoids(UINT8 &status)
{
	GET_PCM_STATUS();
	status = rx->SolenoidBits;
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
	status = temp * 0.03125; /* 5.5 fixed pt value in Amps */
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
	status = (double) raw * 0.03125; /* 5.5 fixed pt value in Volts */
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
CTR_Code PCM::GetCompressorCurrentTooHighFault(bool &status)
{
	GET_PCM_STATUS();
	status = rx->faultCompCurrentTooHigh;
	return rx.err;
}
CTR_Code PCM::GetCompressorShortedStickyFault(bool &status)
{
	GET_PCM_STATUS();
	status = rx->StickyFault_dItooHigh;
	return rx.err;
}
CTR_Code PCM::GetCompressorShortedFault(bool &status)
{
	GET_PCM_STATUS();
	status = rx->Fault_dItooHigh;
	return rx.err;
}
CTR_Code PCM::GetCompressorNotConnectedStickyFault(bool &status)
{
	GET_PCM_SOL_FAULTS();
	status = rx->StickyFault_CompNoCurrent;
	return rx.err;
}
CTR_Code PCM::GetCompressorNotConnectedFault(bool &status)
{
	GET_PCM_SOL_FAULTS();
	status = rx->Fault_CompNoCurrent;
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
CTR_Code PCM::GetCompressorCurrentTooHighStickyFault(bool &status)
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
	status = (float)rx->battVoltage * 0.05 + 4.0; /* 50mV per unit plus 4V. */
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
	status = (rx->SolenoidBlacklist & (1ul<<(idx)) )? 1 : 0;
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
	CTR_Code c_SetAllSolenoids(void * handle, UINT8 state) {
		return ((PCM*) handle)->SetAllSolenoids(state);
	}
	CTR_Code c_SetClosedLoopControl(void * handle, INT8 param) {
		return ((PCM*) handle)->SetClosedLoopControl(param);
	}
	CTR_Code c_ClearStickyFaults(void * handle, INT8 param) {
		return ((PCM*) handle)->ClearStickyFaults();
	}
	CTR_Code c_GetSolenoid(void * handle, UINT8 idx, INT8 * status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetSolenoid(idx, bstatus);
		*status = bstatus;
		return retval;
	}
	CTR_Code c_GetAllSolenoids(void * handle, UINT8 * status) {
		return ((PCM*) handle)->GetAllSolenoids(*status);
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
		CTR_Code retval = ((PCM*) handle)->GetCompressorCurrentTooHighFault(bstatus);
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
		CTR_Code retval = ((PCM*) handle)->GetCompressorCurrentTooHighStickyFault(bstatus);
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
