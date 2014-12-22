#ifndef PCM_H_
#define PCM_H_
#include "ctre.h"				//BIT Defines + Typedefs
#include <NetworkCommunication/CANSessionMux.h>	//CAN Comm
#include "CtreCanNode.h"
#include <pthread.h>
class PCM : public CtreCanNode
{
public:
    PCM(UINT8 deviceNumber=0);
    ~PCM();
    
    /* Set PCM solenoid state
     *
     * @Return	-	CTR_Code	-	Error code (if any) for setting solenoid
     * @Param 	-	idx			- 	ID of solenoid (0-7)
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
     */
    CTR_Code 	ClearStickyFaults();
    
    /* Get solenoid state
     *
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param 	-	idx		- 	ID of solenoid (0-7) to return if solenoid is on.
     * @Param	-	status	-	OK if solenoid enabled, false otherwise
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
     * @Param	-	status		-	True if abnormally high compressor current detected, false if otherwise
     */
    CTR_Code 	GetCompressorCurrentTooHighFault(bool &status);

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
    CTR_Code 	GetCompressorCurrentTooHighStickyFault(bool &status);
    /* Get compressor shorted sticky fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if compressor output is shorted, false if otherwise
     */
    CTR_Code 	GetCompressorShortedStickyFault(bool &status);
    /* Get compressor shorted fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if compressor output is shorted, false if otherwise
     */
    CTR_Code 	GetCompressorShortedFault(bool &status);
    /* Get compressor is not connected sticky fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if compressor current is too low, 
     * 					indicating compressor is not connected, false if otherwise
     */
    CTR_Code 	GetCompressorNotConnectedStickyFault(bool &status);
    /* Get compressor is not connected fault value
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	status		-	True if compressor current is too low, 
     * 					indicating compressor is not connected, false if otherwise
     */
    CTR_Code 	GetCompressorNotConnectedFault(bool &status);

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
     * @Param	-	idx			-	ID of solenoid [0,7]
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

    /* Get solenoid Blacklist status
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	idx			-	ID of solenoid [0,7] to fire one shot pulse.
     */
    CTR_Code FireOneShotSolenoid(UINT8 idx);

    /* Configure the pulse width of a solenoid channel for one-shot pulse.
	 * Preprogrammed pulsewidth is 10ms resolute and can be between 20ms and 5.1s.
     * @Return	-	CTR_Code	-	Error code (if any)
     * @Param	-	idx			-	ID of solenoid [0,7] to configure.
     * @Param	-	durMs		-	pulse width in ms.
     */
    CTR_Code SetOneShotDurationMs(UINT8 idx,uint32_t durMs);

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
