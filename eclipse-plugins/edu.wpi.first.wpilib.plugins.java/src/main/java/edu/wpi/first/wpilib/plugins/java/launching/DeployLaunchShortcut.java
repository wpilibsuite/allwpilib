package edu.wpi.first.wpilib.plugins.java.launching;

import org.eclipse.core.resources.IProject;

/**
 * Launch shortcut base functionality, common for deploying to the robot.
 * Retrieves the project the operation is being called on, and runs the correct
 * ant targets based on polymorphically determined data values
 * 
 * @author Ryan O'Meara
 * @author Alex Henning
 */
public class DeployLaunchShortcut extends JavaLaunchShortcut {
	
	/**
	 * Runs the ant script using the correct target for the indicated mode (deploy to cRIO or just compile)
	 * @param activeProj The project that the script will be run on/from
	 * @param mode The mode it will be run in (ILaunchManager.RUN_MODE or ILaunchManager.DEBUG_MODE)
	 */
	public void runConfig(IProject activeProj, String mode){
		runConfigHelper(activeProj, mode, "deploy", "debug-deploy");
	}
}

