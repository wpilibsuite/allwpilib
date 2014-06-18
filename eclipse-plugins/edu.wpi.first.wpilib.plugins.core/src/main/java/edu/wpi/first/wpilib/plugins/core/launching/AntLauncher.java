package edu.wpi.first.wpilib.plugins.core.launching;

import java.io.File;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.ant.internal.ui.launchConfigurations.AntLaunchShortcut;
import org.eclipse.ant.launching.IAntLaunchConstants;
import org.eclipse.core.externaltools.internal.IExternalToolConstants;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.console.IConsoleConstants;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;

/**
 * Contains functions to launch ant scripts while having the output sent to the
 * console.  Allows for the use of arguments, the specification of targets, and
 * the choice of launch modes
 * 
 * @author Ryan O'Meara
 */
@SuppressWarnings("restriction")
public class AntLauncher {
	private static boolean installing = false;
	
	/**
	 * Runs an ant script's target's with the given arguments, in the given launch mode
	 * @param antScript A java.io.File representation of the ant file to execute
	 * @param targets A String of the ant targets to run.  For multiple targets, 
	 * format if "target1,target2".  For no targets, this argument can be null.  This
	 * method will assume output should be sent to the console, same as calling
	 * runAntFile(antScript, targets, arguments, mode, true)
	 * @param arguments A String of arguments to run the file with.  Format is
	 * "-argument1 -argument2".  For no arguments, this argument can be null
	 * @param mode String, either "run" or "debug".  If an invalid input is given, 
	 * defaults to "run"
	 * @return The ILaunch started, or null if it failed to start
	 */
	public static ILaunch runAntFile(File antScript, String targets, String arguments, String mode){
		return runAntFile(antScript, targets, arguments, mode, true);
	}
	
	/**
	 * Runs an ant script's target's with the given arguments, in the given launch mode
	 * @param antScript A java.io.File representation of the ant file to execute
	 * @param targets A String of the ant targets to run.  For multiple targets, 
	 * format if "target1,target2".  For no targets, this argument can be null.  Allows
	 * choice of outputting to the console
	 * @param arguments A String of arguments to run the file with.  Format is
	 * "-argument1 -argument2".  For no arguments, this argument can be null
	 * @param mode String, either "run" or "debug".  If an invalid input is given, 
	 * defaults to "run"
	 * @param outputToConsole If true, output will be sent to console, if false it will not
	 * @return The ILaunch started, or null if it failed to start
	 */
	public static ILaunch runAntFile(File antScript, String targets, String arguments, String mode, boolean outputToConsole){
		//Input error checking
		if((mode == null)||(!mode.equalsIgnoreCase(ILaunchManager.DEBUG_MODE))){mode = ILaunchManager.RUN_MODE;}  			//Launcher only accepts run or debug
		if((targets != null)&&(targets.equalsIgnoreCase(""))){targets = null;}  		//Standardize representation of "none"
		if((arguments != null)&&(arguments.equalsIgnoreCase(""))){arguments = null;}
		
		if(antScript.getAbsolutePath().indexOf("install.xml") != -1){
			installing = true;
		}else{
			installing = false;
		}
		
		ILaunch ret = null;
		
		try{
			
			//Show the console
			if(outputToConsole){
				try{
				final IWorkbenchPage activePage = PlatformUI.getWorkbench()
				        .getActiveWorkbenchWindow()
				        .getActivePage();
				activePage.showView(IConsoleConstants.ID_CONSOLE_VIEW);
				}catch(Exception e){}
			}

			//Get an ant launch config setup to use eclipse VM - which users will setup to be the jdk,
			//allowing the use of javac.  Also sets the location of the file to execute 
			ILaunchConfiguration launcher = AntLaunchShortcut.createDefaultLaunchConfiguration(new Path(antScript.getAbsolutePath()), null);
			
			//Get copy that can be edited
			ILaunchConfigurationWorkingCopy workingCopy = launcher.getWorkingCopy();
			
			//Prevent this configuration from appearing in history or dialogs
			workingCopy.setAttribute(ILaunchManager.ATTR_PRIVATE, false);
			
			//Setup to show output on console
			if(outputToConsole){
				workingCopy.setAttribute(DebugPlugin.ATTR_CAPTURE_OUTPUT, true);
				workingCopy.setAttribute(IExternalToolConstants.ATTR_SHOW_CONSOLE, true);
			}else{
				workingCopy.setAttribute(DebugPlugin.ATTR_CAPTURE_OUTPUT, false);
				workingCopy.setAttribute(IExternalToolConstants.ATTR_SHOW_CONSOLE, false);
			}
		    
		    
		    workingCopy.setAttribute(IDebugUIConstants.ATTR_LAUNCH_IN_BACKGROUND, true);
		    
		    if(targets != null){
		    	workingCopy.setAttribute(IAntLaunchConstants.ATTR_ANT_TARGETS, targets);
		    }
		    
		    if(arguments != null){
		    	 workingCopy.setAttribute(IExternalToolConstants.ATTR_TOOL_ARGUMENTS, arguments);
		    }
		    
		    if(mode.equals(ILaunchManager.DEBUG_MODE)){
		    	Set<String> modes = new HashSet<String>();
		    	modes.add(ILaunchManager.DEBUG_MODE);
		    	workingCopy.addModes(modes);
		    }
			
			//Save/assert all changes
		    launcher = workingCopy.doSave();
		    
		   //Launch the modified configuration in the specified mode
		   try {
			   ret = launcher.launch(mode, null, true, true);
		   } catch(Exception e) {
			   //Does not need Output, handled and resolved internally
               WPILibCore.logError("Error running launch.", e);
			   return null;
		   }
		} catch(Exception e) {
            WPILibCore.logError("Error running ant file", e);
			return null;
		}
		
		return ret;
	}
	
	public static boolean isInstalling(){
		return installing;
	}
}
