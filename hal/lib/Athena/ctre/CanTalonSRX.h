/**
 * auto generated using spreadsheet and WpiClassGen.csproj
 * @link https://docs.google.com/spreadsheets/d/1OU_ZV7fZLGYUQ-Uhc8sVAmUmWTlT8XBFYK8lfjg_tac/edit#gid=1766046967
 */
#ifndef CanTalonSRX_H_
#define CanTalonSRX_H_
#include "ctre.h"				//BIT Defines + Typedefs
#include "CtreCanNode.h"
#include <NetworkCommunication/CANSessionMux.h>	//CAN Comm
class CanTalonSRX : public CtreCanNode
{
public:
    CanTalonSRX(UINT8 deviceNumber=0);
    ~CanTalonSRX();
    void Set(double value);

    /*------------------------ auto generated ----------------------*/
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
	CTR_Code GetAppliedThrottle11(int &param);
	CTR_Code GetCloseLoopErr(int &param);
	CTR_Code GetSelectlFeedbackDevice(int &param);
	CTR_Code GetModeSelect(int &param);
	CTR_Code GetLimitSwitchEn(int &param);
	CTR_Code GetLimitSwitchClosedFor(int &param);
	CTR_Code GetLimitSwitchClosedRev(int &param);
	CTR_Code GetCloseLoopCellSelect(int &param);
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
	CTR_Code SetDemand24(int param);
	CTR_Code SetLimitSwitchEn(int param);
	CTR_Code SetSelectlFeedbackDevice(int param);
	CTR_Code SetRevMotDuringCloseLoopEn(int param);
	CTR_Code SetBrakeType(int param);
	CTR_Code SetModeSelect(int param);
	CTR_Code SetCloseLoopCellSelect(int param);
	CTR_Code SetRampThrottle(int param);
	CTR_Code SetRevEncoderPosAndVel(int param);
};
#endif

