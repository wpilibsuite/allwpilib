#include "PDP.h"
#include "NetworkCommunication/CANSessionMux.h"	//CAN Comm
#include <string.h> // memset
#include <unistd.h> // usleep

#define STATUS_1  		0x8041400
#define STATUS_2  		0x8041440
#define STATUS_3  		0x8041480
#define STATUS_ENERGY	0x8041740

#define CONTROL_1		0x08041C00	/* PDP_Control_ClearStats */

#define EXPECTED_RESPONSE_TIMEOUT_MS	(50)
#define GET_STATUS1()		CtreCanNode::recMsg<PdpStatus1_t> rx = GetRx<PdpStatus1_t>(STATUS_1,EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS2()		CtreCanNode::recMsg<PdpStatus2_t> rx = GetRx<PdpStatus2_t>(STATUS_2,EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS3()		CtreCanNode::recMsg<PdpStatus3_t> rx = GetRx<PdpStatus3_t>(STATUS_3,EXPECTED_RESPONSE_TIMEOUT_MS)
#define GET_STATUS_ENERGY()	CtreCanNode::recMsg<PDP_Status_Energy_t> rx = GetRx<PDP_Status_Energy_t>(STATUS_ENERGY,EXPECTED_RESPONSE_TIMEOUT_MS)

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
typedef struct _PDP_Status_Energy_t {
	unsigned TmeasMs_likelywillbe20ms_:8;
	unsigned TotalCurrent_125mAperunit_h8:8;
	unsigned Power_125mWperunit_h4:4;
	unsigned TotalCurrent_125mAperunit_l4:4;
	unsigned Power_125mWperunit_m8:8;
	unsigned Energy_125mWPerUnitXTmeas_h4:4;
	unsigned Power_125mWperunit_l4:4;
	unsigned Energy_125mWPerUnitXTmeas_mh8:8;
	unsigned Energy_125mWPerUnitXTmeas_ml8:8;
	unsigned Energy_125mWPerUnitXTmeas_l8:8;
} PDP_Status_Energy_t ;

PDP::PDP(UINT8 deviceNumber): CtreCanNode(deviceNumber)
{
	RegisterRx(STATUS_1 | deviceNumber );
	RegisterRx(STATUS_2 | deviceNumber );
	RegisterRx(STATUS_3 | deviceNumber );
}
/* PDP D'tor
 */
PDP::~PDP()
{
}

CTR_Code PDP::GetChannelCurrent(UINT8 idx, double &current)
{
	CTR_Code retval = CTR_InvalidParamValue;
	uint32_t raw = 0;

	if(idx <= 5){
		GET_STATUS1();
	    retval = rx.err;
		switch(idx){
			case 0:	raw = ((uint32_t)rx->chan1_h8 << 2) | rx->chan1_l2;	break;
			case 1:	raw = ((uint32_t)rx->chan2_h6 << 4) | rx->chan2_l4;	break;
			case 2:	raw = ((uint32_t)rx->chan3_h4 << 6) | rx->chan3_l6;	break;
			case 3:	raw = ((uint32_t)rx->chan4_h2 << 8) | rx->chan4_l8;	break;
			case 4:	raw = ((uint32_t)rx->chan5_h8 << 2) | rx->chan5_l2;	break;
			case 5:	raw = ((uint32_t)rx->chan6_h6 << 4) | rx->chan6_l4;	break;
			default:	retval = CTR_InvalidParamValue;	break;
		}
	}else if(idx <= 11){
		GET_STATUS2();
	    retval = rx.err;
		switch(idx){
			case  6:	raw = ((uint32_t)rx->chan7_h8  << 2) | rx->chan7_l2;	break;
			case  7:	raw = ((uint32_t)rx->chan8_h6  << 4) | rx->chan8_l4;	break;
			case  8:	raw = ((uint32_t)rx->chan9_h4  << 6) | rx->chan9_l6;	break;
			case  9:	raw = ((uint32_t)rx->chan10_h2 << 8) | rx->chan10_l8;	break;
			case 10:	raw = ((uint32_t)rx->chan11_h8 << 2) | rx->chan11_l2;	break;
			case 11:	raw = ((uint32_t)rx->chan12_h6 << 4) | rx->chan12_l4;	break;
			default:	retval = CTR_InvalidParamValue;	break;
		}
	}else if(idx <= 15){
		GET_STATUS3();
	    retval = rx.err;
		switch(idx){
			case 12:	raw = ((uint32_t)rx->chan13_h8  << 2) | rx->chan13_l2;	break;
			case 13:	raw = ((uint32_t)rx->chan14_h6  << 4) | rx->chan14_l4;	break;
			case 14:	raw = ((uint32_t)rx->chan15_h4  << 6) | rx->chan15_l6;	break;
			case 15:	raw = ((uint32_t)rx->chan16_h2  << 8) | rx->chan16_l8;	break;
			default:	retval = CTR_InvalidParamValue;	break;
		}
	}
	/* convert to amps */
	current = (double)raw * 0.125;  /* 7.3 fixed pt value in Amps */
	/* signal caller with success */
	return retval;
}
CTR_Code PDP::GetVoltage(double &voltage)
{
	GET_STATUS3();
	uint32_t raw = rx->busVoltage;
	voltage = (double)raw * 0.05 + 4.0; /* 50mV per unit plus 4V. */;
	return rx.err;
}
CTR_Code PDP::GetTemperature(double &tempC)
{
	GET_STATUS3();
	uint32_t raw = rx->temp;
	tempC =	(double)raw * 1.03250836957542 - 67.8564500484966;
	return rx.err;
}
CTR_Code PDP::GetTotalCurrent(double &currentAmps)
{
	GET_STATUS_ENERGY();
	uint32_t raw;
	raw = rx->TotalCurrent_125mAperunit_h8;
	raw <<= 4;
	raw |=  rx->TotalCurrent_125mAperunit_l4;
	currentAmps = 0.125 * raw;
	return rx.err;
}
CTR_Code PDP::GetTotalPower(double &powerWatts)
{
	GET_STATUS_ENERGY();
	uint32_t raw;
	raw = rx->Power_125mWperunit_h4;
	raw <<= 8;
	raw |=  rx->Power_125mWperunit_m8;
	raw <<= 4;
	raw |=  rx->Power_125mWperunit_l4;
	powerWatts = 0.125 * raw;
	return rx.err;
}
CTR_Code PDP::GetTotalEnergy(double &energyJoules)
{
	GET_STATUS_ENERGY();
	uint32_t raw;
	raw = rx->Energy_125mWPerUnitXTmeas_h4;
	raw <<= 8;
	raw |=  rx->Energy_125mWPerUnitXTmeas_mh8;
	raw <<= 8;
	raw |=  rx->Energy_125mWPerUnitXTmeas_ml8;
	raw <<= 8;
	raw |=  rx->Energy_125mWPerUnitXTmeas_l8;
	energyJoules = 0.125 * raw; 						/* mW integrated every TmeasMs */
	energyJoules *= rx->TmeasMs_likelywillbe20ms_;	/* multiplied by TmeasMs = joules */
	return rx.err;
}
/* Clear sticky faults.
 * @Return	-	CTR_Code	-	Error code (if any)
 */
CTR_Code PDP::ClearStickyFaults()
{
	int32_t status = 0;
	uint8_t pdpControl[] = { 0x80 }; /* only bit set is ClearStickyFaults */
	FRC_NetworkCommunication_CANSessionMux_sendMessage(CONTROL_1  | GetDeviceNumber(), pdpControl, sizeof(pdpControl), 0, &status);
	if(status)
		return CTR_TxFailed;
	return CTR_OKAY;
}

/* Reset Energy Signals
 * @Return	-	CTR_Code	-	Error code (if any)
 */
CTR_Code PDP::ResetEnergy()
{
	int32_t status = 0;
	uint8_t pdpControl[] = { 0x40 }; /* only bit set is ResetEnergy */
	FRC_NetworkCommunication_CANSessionMux_sendMessage(CONTROL_1  | GetDeviceNumber(), pdpControl, sizeof(pdpControl), 0, &status);
	if(status)
		return CTR_TxFailed;
	return CTR_OKAY;
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
	}
}
