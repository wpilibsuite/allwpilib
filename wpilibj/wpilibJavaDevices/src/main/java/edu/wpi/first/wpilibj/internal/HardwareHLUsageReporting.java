package edu.wpi.first.wpilibj.internal;

import edu.wpi.first.wpilibj.HLUsageReporting;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tInstances;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;

public class HardwareHLUsageReporting implements HLUsageReporting.Interface {
	@Override
	public void reportScheduler() {
		UsageReporting.report(tResourceType.kResourceType_Command, tInstances.kCommand_Scheduler);
	}

	@Override
	public void reportPIDController(int num) {
		UsageReporting.report(tResourceType.kResourceType_PIDController, num);
	}

	@Override
	public void reportSmartDashboard() {
		UsageReporting.report(tResourceType.kResourceType_SmartDashboard, 0);
	}
}
