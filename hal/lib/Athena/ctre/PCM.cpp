#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include "PCM.h"
#include "NetworkCommunication/JaguarCANDriver.h"
#include <string.h> // memset
#include <unistd.h> // usleep
/* PCM Constructor - Clears all vars, establishes default settings, starts PCM background process
 * 
 * @Return	-	void
 * 
 * @Param 	-	deviceNumber	- 	Device ID of PCM to be controlled
 */
PCM::PCM(UINT8 deviceNumber)
{
    memset(&_PcmDebug, 		0, sizeof(_PcmDebug));
    memset(&_PcmControl, 	0, sizeof(_PcmControl));
    memset(&_PcmStatus, 	0, sizeof(_PcmStatus));
    memset(&_PcmStatusFault,0, sizeof(_PcmStatusFault));
    /* setup arbids */
	SetDeviceNumber(deviceNumber);
	/* clear error info */
	_timeSinceLastRx = 0;
	_timeSinceLastTx = 0;
	_numFailedRxs = 0;
	_numFailedTxs = 0;
	/* start thread */
	_threadIsRunning = 1;
	_threadErr = pthread_create( &_thread, NULL, ThreadFunc, (void*) this);
}
/* PCM D'tor
 */
PCM::~PCM() {
	/* wait for thread to finish */
	_threadIsRunning = 0;
	pthread_join( _thread, NULL);
	_thread = 0;
}
/* Set PCM Device Number and according CAN frame IDs
 *
 * @Return	-	void
 *
 * @Param	-	deviceNumber	-	Device number of PCM to control
 */
void PCM::SetDeviceNumber(UINT8 deviceNumber) {
	PCM_settings.deviceNumber = deviceNumber;
	PCM_settings.controlFrameID		= 0x9041C00 + (deviceNumber) + (UINT32) (0	* BIT6);
	PCM_settings.statusFrameID 		= 0x9041400 + (deviceNumber) + (UINT32) (0	* BIT6);
	PCM_settings.statusFaultFrameID = 0x9041400 + (deviceNumber) + (UINT32) (1	* BIT6);
	PCM_settings.debugFrameID 		= 0x9041400 + (deviceNumber) + (UINT32) (2 	* BIT6);
}

/* Set PCM solenoid state
 *
 * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
 *
 * @Param 	-	idx			- 	ID of solenoid (1-8)
 * @Param 	-	en			- 	Enable / Disable identified solenoid
 */
CTR_Code PCM::SetSolenoid(unsigned char idx, bool en) {
	idx--; /* make it zero based */
	if (en)
		_PcmControl.solenoidBits |= (1ul << (7-idx));
	else
		_PcmControl.solenoidBits &= ~(1ul << (7-idx));
	if (GetTimeSinceLastTx() >= 50)
		return CTR_TxTimeout;
	return CTR_OKAY;
}

/* Clears PCM sticky faults (indicators of past faults
 *
 * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
 *
 * @Param 	-	clr		- 	Clear / do not clear faults
 */
CTR_Code PCM::ClearStickyFaults(bool clr) {
	_PcmControl.clearStickyFaults = clr;
	if (GetTimeSinceLastTx() >= 50)
		return CTR_TxTimeout;
	return CTR_OKAY;
}

/* Enables PCM Closed Loop Control of Compressor via pressure switch
 *
 * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
 *
 * @Param 	-	en		- 	Enable / Disable Closed Loop Control
 */
CTR_Code PCM::SetClosedLoopControl(bool en) {
	_PcmControl.closedLoopEnable = en;
	if (GetTimeSinceLastTx() >= 50)
		return CTR_TxTimeout;
	return CTR_OKAY;
}

/* Set Compressor state
 * 
 * @Return	-	void
 * 
 * @Param 	-	en		- 	Enable / Disable compressor
 */
CTR_Code PCM::SetCompressor(bool en) {
	_PcmControl.compressorOn = en;
	if (GetTimeSinceLastTx() >= 50)
		return CTR_TxTimeout;
	return CTR_OKAY;
}

/* Get solenoid state
 * 
 * @Return	-	True/False	-	True if solenoid enabled, false otherwise
 * 
 * @Param 	-	idx		- 	ID of solenoid (1-8) to return status of
 */
CTR_Code PCM::GetSolenoid(UINT8 idx, bool &status) {
	idx--;
	status = (_PcmStatus.SolenoidBits & (1ul<<(7-idx))) ? 1 : 0;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get pressure switch state
 * 
 * @Return	-	True/False	-	True if pressure adequate, false if low
 */
CTR_Code PCM::GetPressure(bool &status) {
	status = _PcmStatus.pressureSwitchEn;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get compressor state
 * 
 * @Return	-	True/False	-	True if enabled, false if otherwise
 */
CTR_Code PCM::GetCompressor(bool &status) {
	status = _PcmStatus.compressorOn;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get closed loop control state
 * 
 * @Return	-	True/False	-	True if closed loop enabled, false if otherwise
 */
CTR_Code PCM::GetClosedLoopControl(bool &status) {
	status = _PcmStatus.isCloseloopEnabled;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get compressor current draw
 * 
 * @Return	-	Amperes	-	Compressor current 
 */
CTR_Code PCM::GetCompressorCurrent(float &status) {
	uint8_t bt = _PcmStatus.compressorCurrentTop6;
	bt <<= 4;
	bt |=  _PcmStatus.compressorCurrentBtm4;
	status = 20.1612903225806 * bt;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get suggested compressor state determined by Closed Loop logic
 * 
 * @Return	-	True/False	-	True if closed loop suggests enabling compressor, false if otherwise
 */
CTR_Code PCM::GetClosedLoopSuggestedOutput(bool &status) {
	status = _PcmStatus.closedLoopOutput;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get voltage across solenoid rail
 * 
 * @Return	-	Volts	-	Voltage across solenoid rail
 */
CTR_Code PCM::GetSolenoidVoltage(float &status) {
	uint32_t raw = _PcmStatus.solenoidVoltageTop8;
	raw <<= 2;
	raw |=  _PcmStatus.solenoidVoltageBtm2;
	status = (double) raw * 24.7800586510264 / 1000;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get hardware fault value
 * 
 * @Return	-	True/False	-	True if hardware failure detected, false if otherwise
 */
CTR_Code PCM::GetHardwareFault(bool &status) {
	status = _PcmStatus.faultHardwareFailure;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get compressor fault value
 * 
 * @Return	-	True/False	-	True if shorted compressor detected, false if otherwise
 */
CTR_Code PCM::GetCompressorFault(bool &status) {
	status = _PcmStatus.faultCompCurrentTooHigh;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get solenoid fault value
 * 
 * @Return	-	True/False	-	True if shorted solenoid detected, false if otherwise
 */
CTR_Code PCM::GetSolenoidFault(bool &status) {
	status = _PcmStatus.faultFuseTripped;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}
//			Past Faults

/* Get compressor sticky fault value
 * 
 * @Return	-	True/False	-	True if solenoid had previously been shorted
 * 								(and sticky fault was not cleared), false if otherwise
 */
CTR_Code PCM::GetCompressorStickyFault(bool &status) {
	status = _PcmStatus.stickyFaultCompCurrentTooHigh;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

/* Get solenoid sticky fault value
 * 
 * @Return	-	True/False	-	True if compressor had previously been shorted
 * 								(and sticky fault was not cleared), false if otherwise
 */
CTR_Code PCM::GetSolenoidStickyFault(bool &status) { /* fix this */
	status = _PcmStatus.stickyFaultFuseTripped;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}
/* Get battery voltage
 * 
 * @Return	-	Volts	-	Voltage across PCM power ports
 */
CTR_Code PCM::GetBatteryVoltage(float &status) {
	status = (float)_PcmStatus.battVoltage * ((59.0420332355816) / 1000.0);;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}
/* Get number of total failed PCM Control Frame
 * 
 * @Return	-	Failed Control Frames	-	Number of failed control frames (tokenization fails)
 * 
 * @WARNING	-	Return only valid if [SeekDebugFrames] is enabled
 * 				See function SeekDebugFrames
 * 				See function EnableSeekDebugFrames
 */
CTR_Code PCM::GetNumberOfFailedControlFrames(UINT16 &status) {
	status = _PcmDebug.tokFailsTop8;
	status <<= 8;
	status |= _PcmDebug.tokFailsBtm8;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
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
CTR_Code PCM::GetSolenoidBlackList(UINT8 &status) {
	status = _PcmStatusFault.SolenoidBlacklist;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}
/* Get solenoid Blacklist status
 * - Blacklisted solenoids cannot be enabled until PCM is power cycled
 * 
 * @Return	-	True/False	-	True if Solenoid is blacklisted, false if otherwise
 * 
 * @Param	-	idx			-	ID of solenoid
 * 
 * @WARNING	-	Return only valid if [SeekStatusFaultFrames] is enabled
 * 				See function SeekStatusFaultFrames
 * 				See function EnableSeekStatusFaultFrames
 */
CTR_Code PCM::IsSolenoidBlacklisted(UINT8 idx, bool &status) {
	idx--;
	if(_PcmStatusFault.SolenoidBlacklist & (1ul<<(7-idx)))
		status = 1;
	else
		status = 0;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}
/* Return status of module enable/disable
 *
 * @Return	-	bool		-	Returns TRUE if PCM is enabled, FALSE if disabled
 */
CTR_Code PCM::isModuleEnabled(bool &status) {
	status = _PcmStatus.moduleEnabled;
	if (GetTimeSinceLastRx() >= 50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}
void PCM::GetErrorInfo(	uint32_t * timeSinceLastRx,
						uint32_t * timeSinceLastTx,
						uint32_t * numFailedRxs,
						uint32_t * numFailedTxs)
{
	if(timeSinceLastRx) 	*timeSinceLastRx = _timeSinceLastRx;
	if(timeSinceLastTx) 	*timeSinceLastTx = _timeSinceLastTx;
	if(numFailedRxs) 		*numFailedRxs = _numFailedRxs;
	if(numFailedTxs) 		*numFailedTxs = _numFailedTxs;
}
//------------------ CAN interface and thread --------------------------------------------//
/* Search for PCM Status Frame on CAN bus */
void PCM::ReadStatusFrame(void) {
	PcmStatus_t frame = {0};
	UINT8 size = 0;
	INT32 status = 0;
	FRC_NetworkCommunication_JaguarCANDriver_receiveMessage(&PCM_settings.statusFrameID, (uint8_t *)&frame, &size, 0 , &status);
	if (status == 0) {
		_timeSinceLastRx = 0;
		_PcmStatus = frame;
	} else {
		++_numFailedRxs;
	}
}
/* Search for PCM Status Fault Frame on CAN bus */
void PCM::ReadStatusFaultFrame(void) {
	PcmStatusFault_t frame= {0};
	UINT8 size = 0;
	INT32 status = 0;
	FRC_NetworkCommunication_JaguarCANDriver_receiveMessage(&PCM_settings.statusFaultFrameID, (uint8_t *)&frame, &size, 0, &status);
	if (status == 0) {
		_timeSinceLastRx = 0;
		_PcmStatusFault = frame;
	} else {
		++_numFailedRxs;
	}
}
/* Search for PCM Debug Frame on CAN bus */
void PCM::ReadDebugFrame(void) {
	PcmDebug_t frame= {0};
	UINT8 size = 0;
	INT32 status = 0;
	FRC_NetworkCommunication_JaguarCANDriver_receiveMessage(&PCM_settings.debugFrameID, (uint8_t *)&frame, &size, 0, &status);
	if (status == 0) {
		_timeSinceLastRx = 0;
		_PcmDebug = frame;
	} else {
		++_numFailedRxs;
	}
}
void * PCM::ThreadFunc()
{
	while(_threadIsRunning){
		int32_t status = 0;
		FRC_NetworkCommunication_JaguarCANDriver_sendMessage(PCM_settings.controlFrameID, (const uint8_t *)&_PcmControl, sizeof(_PcmControl), &status);
		if(status == 0){
			/* success */
			_timeSinceLastTx = 0;
		}else {
			/* something is wrong */
			++_numFailedTxs;
		}
		/* reads */
		ReadStatusFrame();
		ReadStatusFaultFrame();
		ReadDebugFrame();
		/* yield for 25ms */
		usleep(25e3);
		/* incrememnt times since comm without overflow */
		if(_timeSinceLastTx < 60000)
			_timeSinceLastTx += 25;
		if(_timeSinceLastRx < 60000)
			_timeSinceLastRx += 25;
	}
	return 0;
}
void * PCM::ThreadFunc( void *ptr )
{
	return ((PCM*)ptr)->ThreadFunc();
}
//------------------ C interface --------------------------------------------//
extern "C" {
	void * c_PCM_Init(void) {
		return new PCM();
	}
	CTR_Code c_SetSolenoid(void * handle, unsigned char idx, INT8 param) {
		return ((PCM*) handle)->SetSolenoid(idx, param);
	}
	CTR_Code c_SetCompressor(void * handle, INT8 param) {
		return ((PCM*) handle)->SetCompressor(param);
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
	CTR_Code c_GetClosedLoopSuggestedOutput(void * handle, INT8 * status) {
		bool bstatus;
		CTR_Code retval = ((PCM*) handle)->GetClosedLoopSuggestedOutput(bstatus);
		*status = bstatus;
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
		return ((PCM*) handle)->SetDeviceNumber(deviceNumber);
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

