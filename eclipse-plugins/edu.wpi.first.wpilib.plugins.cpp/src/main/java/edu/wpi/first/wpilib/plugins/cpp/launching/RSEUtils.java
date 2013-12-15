package edu.wpi.first.wpilib.plugins.cpp.launching;

import java.util.Arrays;

import org.eclipse.rse.core.IRSESystemType;
import org.eclipse.rse.core.PasswordPersistenceManager;
import org.eclipse.rse.core.RSECorePlugin;
import org.eclipse.rse.core.model.IHost;
import org.eclipse.rse.core.model.ISystemProfile;
import org.eclipse.rse.core.model.ISystemRegistry;
import org.eclipse.rse.core.model.SystemSignonInformation;

public class RSEUtils {

	public static IHost getTarget(int teamNumber) {
		// The ip address based on the team number
        String hostName = "10."+(teamNumber/100)+"."+(teamNumber%100)+".2";
        String connectionName = "Team "+teamNumber;
   
        // get the singleton RSE registry
        try {
			RSECorePlugin.waitForInitCompletion();
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
        ISystemRegistry registry = RSECorePlugin.getDefault().getSystemRegistry();
   
        // get the default profile, used to store connections
        System.out.println("Profiles: "+Arrays.toString(registry.getActiveSystemProfiles()));
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
            			"XX", systemType);
    			PasswordPersistenceManager.getInstance().add(info, true, false);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return host;
	}
}
