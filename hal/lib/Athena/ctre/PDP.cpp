#include "PDP.h"
#include "NetworkCommunication/JaguarCANDriver.h"
#include <string.h> // memset
#include <unistd.h> // usleep
PDP::PDP(UINT8 deviceNumber)
{
    memset(&_status1, 0, sizeof(_status1));
    memset(&_status2, 0, sizeof(_status2));
    memset(&_status3, 0, sizeof(_status3));
    memset(&PDP_Settings, 0, sizeof PDP_Settings);
    /* setup arbids */
	SetDeviceNumber(deviceNumber);
	/* clear error info */
	_timeSinceLastRx = 0;
	_numFailedRxs = 0;
	/* start thread */
	_threadIsRunning = 1;
	_threadErr = pthread_create( &_thread, NULL, ThreadFunc, (void*) this);
}
/* PDP D'tor
 */
PDP::~PDP() {
	/* wait for thread to finish */
	_threadIsRunning = 0;
	pthread_join( _thread, NULL);
	_thread = 0;
}
CTR_Code PDP::GetChannelCurrent(UINT8 idx, double &current){
	/* atomically copy out our data */
    PdpStatus1_t status1 = _status1;
    PdpStatus2_t status2 = _status2;
    PdpStatus3_t status3 = _status3;
	uint32_t raw = 0;
	if(idx <= 6)
	{
		switch(idx)
		{
			case 0:	raw = 0; break;
			case 1:	raw = ((uint32_t)status1.chan1_h8 << 2) | status1.chan1_l2;	break;
			case 2:	raw = ((uint32_t)status1.chan2_h6 << 4) | status1.chan2_l4;	break;
			case 3:	raw = ((uint32_t)status1.chan3_h4 << 6) | status1.chan3_l6;	break;
			case 4:	raw = ((uint32_t)status1.chan4_h2 << 8) | status1.chan4_l8;	break;
			case 5:	raw = ((uint32_t)status1.chan5_h8 << 2) | status1.chan5_l2;	break;
			case 6:	raw = ((uint32_t)status1.chan6_h6 << 4) | status1.chan6_l4;	break;
		}
	}
	else if(idx <= 12)
	{
		switch(idx)
		{
			case  7:	raw = ((uint32_t)status2.chan7_h8  << 2) | status2.chan7_l2;	break;
			case  8:	raw = ((uint32_t)status2.chan8_h6  << 4) | status2.chan8_l4;	break;
			case  9:	raw = ((uint32_t)status2.chan9_h4  << 6) | status2.chan9_l6;	break;
			case 10:	raw = ((uint32_t)status2.chan10_h2 << 8) | status2.chan10_l8;	break;
			case 11:	raw = ((uint32_t)status2.chan11_h8 << 2) | status2.chan11_l2;	break;
			case 12:	raw = ((uint32_t)status2.chan12_h6 << 4) | status2.chan12_l4;	break;
		}
	}
	else if(idx <= 16)
	{
		switch(idx)
		{
			case 13:	raw = ((uint32_t)status3.chan13_h8  << 2) | status3.chan13_l2;	break;
			case 14:	raw = ((uint32_t)status3.chan14_h6  << 4) | status3.chan14_l4;	break;
			case 15:	raw = ((uint32_t)status3.chan15_h4  << 6) | status3.chan15_l6;	break;
			case 16:	raw = ((uint32_t)status3.chan16_h2  << 8) | status3.chan16_l8;	break;
		}
	}
	/* convert to amps */
	current = 0.06724511900000001*raw + 1.527114967;
	/* signal caller with success */
	if(GetTimeSinceLastRx()>=50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}
CTR_Code PDP::GetVoltage(double &voltage){
	uint32_t raw = _status3.busVoltage;
	voltage = 0.0554413328606877 * raw;
	if(GetTimeSinceLastRx()>=50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}

CTR_Code PDP::GetTemperature(double &tempC){
	uint32_t raw = _status3.temp;
	tempC =	((double)raw-67.8564500484966)*1.03250836957542;
	if(GetTimeSinceLastRx()>=50)
		return CTR_RxTimeout;
	return CTR_OKAY;
}
void PDP::SetDeviceNumber(UINT8 deviceNumber){
	PDP_Settings.deviceNumber = deviceNumber;
	PDP_Settings.frameIDs[0] =	0x8041400 + (deviceNumber) + (UINT32) (0 * BIT6);
	PDP_Settings.frameIDs[1] =	0x8041400 + (deviceNumber) + (UINT32) (1 * BIT6);
	PDP_Settings.frameIDs[2] =	0x8041400 + (deviceNumber) + (UINT32) (2 * BIT6);
}
void PDP::GetErrorInfo(	uint32_t * timeSinceLastRx,
						uint32_t * numFailedRxs)
{
	if(timeSinceLastRx) 	*timeSinceLastRx = _timeSinceLastRx;
	if(numFailedRxs) 		*numFailedRxs = _numFailedRxs;
}
//------------------ CAN interface and thread --------------------------------------------//
/* Search for PCM Status Frame on CAN bus */
uint64_t PDP::ReadCurrents(uint8_t api)
{
	uint64_t frame = 0;
	UINT8 size = 0;
	INT32 status = 0;
	FRC_NetworkCommunication_JaguarCANDriver_receiveMessage(&PDP_Settings.frameIDs[api], (uint8_t *)&frame, &size, 0 , &status);
	if (status == 0) {
		_timeSinceLastRx = 0;
	} else {
		++_numFailedRxs;
	}
	return frame;
}
void * PDP::ThreadFunc()
{
	while(_threadIsRunning){
		/* reads */
		uint64_t frame1 = ReadCurrents(0);
		uint64_t frame2 = ReadCurrents(1);
		uint64_t frame3 = ReadCurrents(2);
		/* update stats */
		memcpy(&_status1,&frame1,8);
		memcpy(&_status2,&frame2,8);
		memcpy(&_status3,&frame3,8);
		/* yield for 25ms */
		usleep(25e3);
		/* timeouts */
		if(_timeSinceLastRx < 60000)
			_timeSinceLastRx += 20;
	}
	return 0;
}
void * PDP::ThreadFunc( void *ptr )
{
	return ((PDP*)ptr)->ThreadFunc();
}
//------------------ C interface --------------------------------------------//
extern "C" {
	void * c_PDP_Init(void)
	{
		return new PDP();
	}
	CTR_Code c_GetChannelCurrent(void * handle,UINT8 idx, double *status)
	{
		return ((PDP*)handle)-> GetChannelCurrent(idx,*status);
	}
	CTR_Code c_GetVoltage(void * handle,double *status)
	{
		return ((PDP*)handle)-> GetVoltage(*status);
	}
	CTR_Code c_GetTemperature(void * handle,double *status)
	{
		return ((PDP*)handle)-> GetTemperature(*status);
	}
	void c_SetDeviceNumber_PDP(void * handle,UINT8 deviceNumber)
	{
		return ((PDP*)handle)-> SetDeviceNumber(deviceNumber);
	}
}
