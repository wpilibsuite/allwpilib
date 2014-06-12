package edu.wpi.first.wpilib.plugins.java.launching;

import org.eclipse.core.resources.IProject;

public class SimulateLaunchShortcut extends JavaLaunchShortcut {


	/**
	 * Runs the ant script using the correct target for the indicated mode (deploy to cRIO or just compile)
	 * @param activeProj The project that the script will be run on/from
	 * @param mode The mode it will be run in (ILaunchManager.RUN_MODE or ILaunchManager.DEBUG_MODE)
	 */
	public void runConfig(IProject activeProj, String mode){
		runConfigHelper(activeProj, mode, "simulate", "debug-simulate");
	}
	
	protected String getHostname(IProject proj) {
		return "localhost";
	}
}
