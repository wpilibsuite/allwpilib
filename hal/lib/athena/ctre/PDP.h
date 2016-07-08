#ifndef PDP_H_
#define PDP_H_
#include "ctre.h"				//BIT Defines + Typedefs
#include "CtreCanNode.h"
class PDP : public CtreCanNode
{
public:
    /* Get PDP Channel Current
     *
     * @Param	-	deviceNumber	-	Device ID for PDP. Factory default is 60. Function defaults to 60.
     */
    PDP(UINT8 deviceNumber=0);
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

	CTR_Code GetTotalCurrent(double &currentAmps);
	CTR_Code GetTotalPower(double &powerWatts);
	CTR_Code GetTotalEnergy(double &energyJoules);
    /* Clear sticky faults.
     * @Return	-	CTR_Code	-	Error code (if any)
     */
	CTR_Code ClearStickyFaults();

	/* Reset Energy Signals
	 * @Return	-	CTR_Code	-	Error code (if any)
	 */
	CTR_Code ResetEnergy();
private:
    uint64_t ReadCurrents(uint8_t api);
};
extern "C" {
	void * c_PDP_Init();
	CTR_Code c_GetChannelCurrent(void * handle,UINT8 idx, double *status);
	CTR_Code c_GetVoltage(void * handle,double *status);
	CTR_Code c_GetTemperature(void * handle,double *status);
	void c_SetDeviceNumber_PDP(void * handle,UINT8 deviceNumber);
}
#endif /* PDP_H_ */
