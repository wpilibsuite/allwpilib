#ifndef PDP_H_
#define PDP_H_
#include "ctre.h"				//BIT Defines + Typedefs
#include <NetworkCommunication/JaguarCANDriver.h>	//CAN Comm
#include <pthread.h>
/* encoder/decoders */
typedef struct _PdpStatus1_t{
	unsigned chan1_h8:8;
	unsigned chan2_h6:6;
	unsigned chan1_l2:2;
	unsigned chan3_h4:4;
	unsigned chan2_l4:4;
	unsigned chan4_h2:2;
	unsigned chan3_l6:6;
	unsigned chan4_l8:8;
	unsigned chan5_h8:8;
	unsigned chan6_h6:6;
	unsigned chan5_l2:2;
	unsigned reserved4:4;
	unsigned chan6_l4:4;
}PdpStatus1_t;
typedef struct _PdpStatus2_t{
	unsigned chan7_h8:8;
	unsigned chan8_h6:6;
	unsigned chan7_l2:2;
	unsigned chan9_h4:4;
	unsigned chan8_l4:4;
	unsigned chan10_h2:2;
	unsigned chan9_l6:6;
	unsigned chan10_l8:8;
	unsigned chan11_h8:8;
	unsigned chan12_h6:6;
	unsigned chan11_l2:2;
	unsigned reserved4:4;
	unsigned chan12_l4:4;
}PdpStatus2_t;
typedef struct _PdpStatus3_t{
	unsigned chan13_h8:8;
	unsigned chan14_h6:6;
	unsigned chan13_l2:2;
	unsigned chan15_h4:4;
	unsigned chan14_l4:4;
	unsigned chan16_h2:2;
	unsigned chan15_l6:6;
	unsigned chan16_l8:8;
	unsigned internalResBattery_mOhms:8;
	unsigned busVoltage:8;
	unsigned temp:8;
}PdpStatus3_t;
class PDP
{
public:
    /* Get PDP Channel Current
     *
     * @Param	-	deviceNumber	-	Device ID for PDP. Factory default is 60. Function defaults to 60.
     */
    PDP(UINT8 deviceNumber=60);
    ~PDP();
    /* Get PDP Channel Current
     *
     * @Return	-	CTR_Code	-	Error code (if any)
     *
     * @Param	-	idx			-	ID of channel to return current for (channels 1-16)
     *
     * @Param	-	status		-	Current of channel 'idx' in Amps (A)
     */
    CTR_Code GetChannelCurrent(UINT8 idx, double &status);

    /* Get Bus Voltage of PDP
     *
     * @Return	-	CTR_Code	- 	Error code (if any)
     *
     * @Param	-	status		- 	Voltage (V) across PDP
     */
    CTR_Code GetVoltage(double &status);

    /* Get Temperature of PDP
     *
     * @Return	-	CTR_Code	-	Error code (if any)
     *
     * @Param	-	status		-	Temperature of PDP in Centigrade / Celcius (C)
     */
    CTR_Code GetTemperature(double &status);
    /* Set PDP Device Number
     *
     * @Return	-	void
     *
     * @Param	-	deviceNumber	-	Device number of PDP to control
     */
    void SetDeviceNumber(UINT8 deviceNumber);
	/* Get time since last received frame
	 * @Return	-	int		-	Returns time in milliseconds (ms) since last received PCM frame
	 */
	int GetTimeSinceLastRx(void) { return _timeSinceLastRx;}
private:
	/* frames to receive */
    PdpStatus1_t _status1;
    PdpStatus2_t _status2;
    PdpStatus3_t _status3;
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
    struct PDPSettings{
    	UINT8	deviceNumber;
    	UINT32	frameIDs[3];
    }PDP_Settings;
    uint64_t ReadCurrents(uint8_t api);
    void GetErrorInfo(	uint32_t * timeSinceLastRx,uint32_t * numFailedRxs);
    static void * ThreadFunc(void *);
    void * ThreadFunc();
};
extern "C" {
	void * c_PDP_Init();
	CTR_Code c_GetChannelCurrent(void * handle,UINT8 idx, double *status);
	CTR_Code c_GetVoltage(void * handle,double *status);
	CTR_Code c_GetTemperature(void * handle,double *status);
	void c_SetDeviceNumber_PDP(void * handle,UINT8 deviceNumber);
}
#endif /* PDP_H_ */
