package edu.wpi.first.wpilib.plugins.cpp.launching;

import java.util.Arrays;

import org.eclipse.rse.core.IRSESystemType;
import org.eclipse.rse.core.PasswordPersistenceManager;
import org.eclipse.rse.core.RSECorePlugin;
import org.eclipse.rse.core.model.IHost;
import org.eclipse.rse.core.model.ISystemProfile;
import org.eclipse.rse.core.model.ISystemRegistry;
import org.eclipse.rse.core.model.SystemSignonInformation;

import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;

public class RSEUtils {

	public static IHost getTarget(int teamNumber) {
		// The ip address based on the team number
        String hostName = "10."+(teamNumber/100)+"."+(teamNumber%100)+".2";
        String connectionName = hostName; //"Team "+teamNumber;
   
        // get the singleton RSE registry
        try {
			RSECorePlugin.waitForInitCompletion();
		} catch (InterruptedException e) {
            WPILibCPPPlugin.logError("Error initializing RSE", e);
		}
        ISystemRegistry registry = RSECorePlugin.getDefault().getSystemRegistry();
   
        // get the default profile, used to store connections
        WPILibCPPPlugin.logInfo("Profiles: "+Arrays.toString(registry.getActiveSystemProfiles()));
        ISystemProfile profile = registry.getActiveSystemProfiles()[0];
   
        // see if a host object already exists for "build.eclipse.org"
        IHost host = registry.getHost(profile, connectionName);
        if (host == null) {
            // if there's no host then we will create it
            try {
                // create the host object as an SSH Only connection
            	IRSESystemType systemType =
            			RSECorePlugin.getTheCoreRegistry().getSystemTypeById("org.eclipse.rse.systemtype.ssh");
            	host = registry.createHost(profile.getName(), systemType, connectionName, hostName,
            			"The remote target for debugging the robot for team "+teamNumber+".");
            	host.setDefaultUserId("admin");
            	SystemSignonInformation info = new SystemSignonInformation(hostName, "admin",
            			"", systemType);
    			PasswordPersistenceManager.getInstance().add(info, true, false);
            } catch (Exception e) {
                WPILibCPPPlugin.logError("Error connecting to RoboRIO.", e);
            }
        }
        return host;
	}
}
