package edu.wpi.first.wpilib.plugins.cpp.launching;

import java.io.File;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.eclipse.cdt.debug.core.ICDTLaunchConfigurationConstants;
import org.eclipse.cdt.debug.core.executables.Executable;
import org.eclipse.cdt.debug.core.executables.ExecutablesManager;
import org.eclipse.cdt.debug.mi.core.IGDBServerMILaunchConfigurationConstants;
import org.eclipse.cdt.debug.mi.core.IMIConstants;
import org.eclipse.cdt.debug.mi.core.IMILaunchConfigurationConstants;
import org.eclipse.cdt.launch.remote.IRemoteConnectionConfigurationConstants;
import org.eclipse.cdt.launch.remote.IRemoteConnectionHostConstants;
import org.eclipse.cdt.ui.ICDTConstants;
import org.eclipse.core.internal.resources.Resource;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.debug.core.DebugException;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfigurationType;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.internal.ui.launchConfigurations.LaunchHistory;
import org.eclipse.debug.ui.DebugUITools;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.debug.ui.ILaunchGroup;
import org.eclipse.debug.ui.ILaunchShortcut;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PlatformUI;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.launching.AntLauncher;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;

/**
 * Launch shortcut base functionality, common for deploying to the robot.
 * Retrieves the project the operation is being called on, and runs the correct
 * ant targets based on polymorphically determined data values
 * 
 * @author Ryan O'Meara
 * @author Alex Henning
 */
@SuppressWarnings("restriction")
public class DeployLaunchShortcut implements ILaunchShortcut
{
	// Class constants - used to delineate types for launch shortcuts
	public static final String DEPLOY_TYPE = "edu.wpi.first.wpilib.plugins.core.deploy";
	
	/**
	 * Returns the launch type of the shortcut that was used, one of the
	 * constants defined in BaseLaunchShortcut
	 * 
	 * @return Launch shortcut type
	 */
	public String getLaunchType()
	{
		return DEPLOY_TYPE;
	}
	
	@Override
	public void launch(ISelection selection, String mode)
	{
		// Extract resource from selection
		StructuredSelection sel = (StructuredSelection) selection;
		IProject activeProject = null;
		if (sel.getFirstElement() instanceof IProject)
		{
			activeProject = (IProject) sel.getFirstElement();
		} else
		{
			return;
		}
		
		// Run config using project found in extracted resource, with indicated
		// mode
		runConfig(activeProject, mode, PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell());
	}
	
	@Override
	public void launch(IEditorPart editor, String mode)
	{
		// Extract resource from editor
		if (editor != null)
		{
			IFileEditorInput input = (IFileEditorInput) editor.getEditorInput();
			IFile file = input.getFile();
			IProject activeProject = file.getProject();
			
			// If editor existed, run config using extracted resource in
			// indicated mode
			runConfig(activeProject, mode, editor.getSite().getWorkbenchWindow().getShell());
		} else
		{
			System.err.println("editor was null");
		}
		
	}
	
	/**
	 * 
	 * @param activeProj
	 *            The project that the script will be run on/from
	 * @param mode
	 *            The mode it will be run in (ILaunchManager.RUN_MODE or
	 *            ILaunchManager.DEBUG_MODE)
	 */
	public void runConfig(IProject activeProj, String mode, Shell shell) //TODO: figure out UI issues. tjats why this is undocumented""
	{	
		ILaunchConfigurationWorkingCopy config;
		try
		{
			config = getRemoteDebugConfig(activeProj);
			//config.doSave(); // NOTE: For debugging
			//org.eclipse.debug.core.DebugPlugin.getDefault().getLaunchManager().addLaunch(config.launch(mode, null));
			//THIS IS MADDENING! we want to add to the recent history, but I can't seem to find a public api to do so, so lets just launch the config dialog
			//DebugUITools.openLaunchConfigurationPropertiesDialog(shell, config, "org.eclipse.cdt.launch.launchGroup");
			//config.launch(mode, new NullProgressMonitor(), false, true);
			DebugUITools.launch(config, mode);
		} catch (CoreException e)
		{
			System.err.println("Debug attach failed.");
			e.printStackTrace();
		}
		
		try
		{
			activeProj.refreshLocal(Resource.DEPTH_INFINITE, null);
		} catch (Exception e)
		{
		}
	}
	
	private ILaunchConfigurationWorkingCopy getRemoteDebugConfig(IProject activeProj) throws CoreException
	{
		ILaunchManager manager = DebugPlugin.getDefault().getLaunchManager();
		ILaunchConfigurationType type = manager.getLaunchConfigurationType(ICDTLaunchConfigurationConstants.ID_LAUNCH_C_REMOTE_APP);
		int teamNumber = WPILibCore.getDefault().getTeamNumber(activeProj);
		String remote_connection = RSEUtils.getTarget(teamNumber).getName();
		
		ILaunchConfigurationWorkingCopy config = type.newInstance(null, activeProj.getName());
		config.setAttribute(ICDTLaunchConfigurationConstants.ATTR_PROJECT_NAME, activeProj.getName());
		Collection<Executable> exes = ExecutablesManager.getExecutablesManager().getExecutablesForProject(activeProj);
		config.setAttribute(ICDTLaunchConfigurationConstants.ATTR_PROGRAM_NAME, 
				exes.size() > 0 ? exes.toArray(new Executable[0])[0].getPath().makeRelativeTo(activeProj.getLocation()).toString():
			 "Debug/FRCUserProgram");
		config.setAttribute(IRemoteConnectionConfigurationConstants.ATTR_REMOTE_PATH, "/home/admin/FRCUserProgram");
		config.setAttribute(IMILaunchConfigurationConstants.ATTR_DEBUG_NAME, WPILibCPPPlugin.getDefault().getToolchain() + "/bin/arm-none-linux-gnueabi-gdb");
		config.setAttribute(IRemoteConnectionConfigurationConstants.ATTR_REMOTE_CONNECTION, remote_connection);
		config.setAttribute(ICDTLaunchConfigurationConstants.ATTR_DEBUGGER_ID, "gdbserver");
		List<String> solibs = new ArrayList<>();
		solibs.add(WPILibCPPPlugin.getDefault().getToolchain() + "/arm-non-linux-gnueabi/libc/lib");
		solibs.add(WPILibCPPPlugin.getDefault().getToolchain() + "/arm-non-linux-gnueabi/libc/usr/lib");
		solibs.add(WPILibCPPPlugin.getDefault().getCPPDir() + "/lib");
		config.setAttribute(IMILaunchConfigurationConstants.ATTR_DEBUGGER_SOLIB_PATH, solibs);
		
		return config;
	}
}
