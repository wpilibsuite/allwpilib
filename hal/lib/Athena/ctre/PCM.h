#ifndef PCM_H_
#define PCM_H_
#include "ctre.h"				//BIT Defines + Typedefs
#include <NetworkCommunication/JaguarCANDriver.h>	//CAN Comm
#include <pthread.h>
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
	unsigned reserved:5;
	unsigned CompressorOn_deprecated:1; //!< This is ignored by PCM firm now.
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

class PCM
{
public:
    PCM(UINT8 deviceNumber=50);
    ~PCM();
    
    /* Set PCM solenoid state
     * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
     * @Param 	-	idx			- 	ID of solenoid (1-8)
     * @Param 	-	en			- 	Enable / Disable identified solenoid
     */
    CTR_Code 	SetSolenoid(unsigned char idx, bool en);

    /* Enables PCM Closed Loop Control of Compressor via pressure switch
     * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
     * @Param 	-	en		- 	Enable / Disable Closed Loop Control
     */
    CTR_Code 	SetClosedLoopControl(bool en);

    /* Clears PCM sticky faults (indicators of past faults
     * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
     * @Param 	-	clr		- 	Clear / do not clear faults
     */
    CTR_Code 	ClearStickyFaults(bool clr);
    
    /* Get solenoid state
     *
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param 	-	idx		- 	ID of solenoid (1-8) to return status of
     * @Param	-	status	-	True if solenoid output is set to be enabled, false otherwise.
     *                          If the phsyical output led still isn't on, then check webdash for
     *                          any faults/is PCM enabled.
     */
    CTR_Code 	GetSolenoid(UINT8 idx, bool &status);

    /* Get pressure switch state
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if pressure adequate, false if low
     */
    CTR_Code 	GetPressure(bool &status);

    /* Get compressor state
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if compress ouput is on, false if otherwise
     */
    CTR_Code	GetCompressor(bool &status);

    /* Get closed loop control state
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status	-	True if closed loop enabled, false if otherwise
     */
    CTR_Code 	GetClosedLoopControl(bool &status);

    /* Get compressor current draw
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	Compressor current returned in Amperes (A)
     */
    CTR_Code 	GetCompressorCurrent(float &status);

    /* Get voltage across solenoid rail
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	Voltage across solenoid rail in Volts (V)
     */
    CTR_Code 	GetSolenoidVoltage(float &status);

    /* Get hardware fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if hardware failure detected, false if otherwise
     */
    CTR_Code 	GetHardwareFault(bool &status);

    /* Get compressor fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if shorted compressor detected, false if otherwise
     */
    CTR_Code 	GetCompressorFault(bool &status);

    /* Get solenoid fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if shorted solenoid detected, false if otherwise
     */
    CTR_Code 	GetSolenoidFault(bool &status);

    /* Get compressor sticky fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if solenoid had previously been shorted
     * 								(and sticky fault was not cleared), false if otherwise
     */
    CTR_Code 	GetCompressorStickyFault(bool &status);

    /* Get solenoid sticky fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if compressor had previously been shorted
     * 								(and sticky fault was not cleared), false if otherwise
     */
    CTR_Code 	GetSolenoidStickyFault(bool &status);

    /* Get battery voltage
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	Voltage across PCM power ports in Volts (V)
     */
    CTR_Code 	GetBatteryVoltage(float &status);
    
    /* Set PCM Device Number and according CAN frame IDs
     * @Return	-	void
     * @Param	-	deviceNumber	-	Device number of PCM to control
     */
    void	SetDeviceNumber(UINT8 deviceNumber);
    /* Get number of total failed PCM Control Frame
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	Number of failed control frames (tokenization fails)
     * @WARNING	-	Return only valid if [SeekDebugFrames] is enabled
     * 				See function SeekDebugFrames
     * 				See function EnableSeekDebugFrames
     */
	CTR_Code GetNumberOfFailedControlFrames(UINT16 &status);
    
    /* Get raw Solenoid Blacklist
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	Raw binary breakdown of Solenoid Blacklist
     * 								BIT7 = Solenoid 1, BIT6 = Solenoid 2, etc.
     * @WARNING	-	Return only valid if [SeekStatusFaultFrames] is enabled
     * 				See function SeekStatusFaultFrames
     * 				See function EnableSeekStatusFaultFrames
     */
    CTR_Code 	GetSolenoidBlackList(UINT8 &status);

    /* Get solenoid Blacklist status
     * - Blacklisted solenoids cannot be enabled until PCM is power cycled
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	idx			-	ID of solenoid
     * @Param	-	status		-	True if Solenoid is blacklisted, false if otherwise
     * @WARNING	-	Return only valid if [SeekStatusFaultFrames] is enabled
     * 				See function SeekStatusFaultFrames
     * 				See function EnableSeekStatusFaultFrames
     */
    CTR_Code 	IsSolenoidBlacklisted(UINT8 idx, bool &status);

    /* Return status of module enable/disable
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	Returns TRUE if PCM is enabled, FALSE if disabled
     */
    CTR_Code	isModuleEnabled(bool &status);

    /* Get time since last sent frame
	 * @Return	-	int		-	Returns time in milliseconds (ms) since last sent PCM frame
	 */
	int GetTimeSinceLastTx(void) { return _timeSinceLastTx;}

	/* Get time since last received frame
	 * @Return	-	int		-	Returns time in milliseconds (ms) since last received PCM frame
	 */
	int GetTimeSinceLastRx(void) { return _timeSinceLastRx;}
private:

	    /* Seek PCM Status Frames on CAN bus
	     * @Return	-	void
	     * @Param	-	en	-	Enable / Disable seeking of PCM Status Frame
	     * @Notes	-	Status Frames identify
	     */
	    void 	EnableSeekStatusFrames(bool en);

	    /* Seek PCM Status Fault Frames on CAN bus
	     * @Return	-	void
	     * @Param	-	en	-	Enable / Disable seeking of PCM Status Fault Frame
	     * @Notes	-	Status Fault Frames identify Blacklisted Solenoids
	     */
	    void 	EnableSeekStatusFaultFrames(bool en);

	    /* Seek PCM Debug Frames on CAN bus
	     * @Return	-	void
	     * @Param	-	en	-	Enable / Disable seeking of PCM Debug Frame
	     * @Notes	-	Debug Frames identify the number of failed tokens (for exclusive, secure control of PCM by RoboRIO)
	     */
	    void 	EnableSeekDebugFrames(bool en);
	/* frames to receive */
	PcmDebug_t 			_PcmDebug;
	PcmStatus_t 		_PcmStatus;
	PcmStatusFault_t 	_PcmStatusFault;
	/* frames to send */
	PcmControl_t 		_PcmControl;
	/* tracking health and error info */
	uint32_t _timeSinceLastRx;
	uint32_t _timeSinceLastTx;
	uint32_t _numFailedRxs;
	uint32_t _numFailedTxs;
	/* threading */
	pthread_t _thread;
	int _threadErr;
	int _threadIsRunning;
	/** arbids */
    struct PCM_SETTINGS{
    	UINT8 deviceNumber;
    	UINT32 controlFrameID;
    	UINT32 statusFrameID;
    	UINT32 statusFaultFrameID;
    	UINT32 debugFrameID;
    }PCM_settings;
    void ReadStatusFrame(void);
    void ReadStatusFaultFrame(void);
    void ReadDebugFrame(void);
    void GetErrorInfo(	uint32_t * timeSinceLastRx,
						uint32_t * timeSinceLastTx,
						uint32_t * numFailedRxs,
						uint32_t * numFailedTxs);
    static void * ThreadFunc(void *);
    void * ThreadFunc();
};
//------------------ C interface --------------------------------------------//
extern "C" {
	void * c_PCM_Init(void);
	CTR_Code c_SetSolenoid(void * handle,unsigned char idx,INT8 param);
	CTR_Code c_SetClosedLoopControl(void * handle,INT8 param);
	CTR_Code c_ClearStickyFaults(void * handle,INT8 param);
	CTR_Code c_GetSolenoid(void * handle,UINT8 idx,INT8 * status);
	CTR_Code c_GetPressure(void * handle,INT8 * status);
	CTR_Code c_GetCompressor(void * handle,INT8 * status);
	CTR_Code c_GetClosedLoopControl(void * handle,INT8 * status);
	CTR_Code c_GetCompressorCurrent(void * handle,float * status);
	CTR_Code c_GetSolenoidVoltage(void * handle,float*status);
	CTR_Code c_GetHardwareFault(void * handle,INT8*status);
	CTR_Code c_GetCompressorFault(void * handle,INT8*status);
	CTR_Code c_GetSolenoidFault(void * handle,INT8*status);
	CTR_Code c_GetCompressorStickyFault(void * handle,INT8*status);
	CTR_Code c_GetSolenoidStickyFault(void * handle,INT8*status);
	CTR_Code c_GetBatteryVoltage(void * handle,float*status);
	void c_SetDeviceNumber_PCM(void * handle,UINT8 deviceNumber);
	void c_EnableSeekStatusFrames(void * handle,INT8 enable);
	void c_EnableSeekStatusFaultFrames(void * handle,INT8 enable);
	void c_EnableSeekDebugFrames(void * handle,INT8 enable);
	CTR_Code c_GetNumberOfFailedControlFrames(void * handle,UINT16*status);
	CTR_Code c_GetSolenoidBlackList(void * handle,UINT8 *status);
	CTR_Code c_IsSolenoidBlacklisted(void * handle,UINT8 idx,INT8*status);
}
#endif
