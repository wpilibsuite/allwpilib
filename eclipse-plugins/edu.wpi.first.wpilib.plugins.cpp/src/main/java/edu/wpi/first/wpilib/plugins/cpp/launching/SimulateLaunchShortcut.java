package edu.wpi.first.wpilib.plugins.cpp.launching;

import java.io.File;
import java.lang.reflect.Method;
import java.util.Vector;

import org.eclipse.core.internal.resources.Resource;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.ui.IDebugUIConstants;
import org.eclipse.debug.ui.ILaunchShortcut;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.PlatformUI;

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
public class SimulateLaunchShortcut implements ILaunchShortcut {
	//Class constants - used to delineate types for launch shortcuts
	public static final String DEPLOY_TYPE = "edu.wpi.first.wpilib.plugins.core.deploy";
	private static final String ANT_SERVER_THREAD_NAME = "Ant Build Server Connection";
	
	private static ILaunch lastDeploy = null;
	
	/**
	 * Returns the launch type of the shortcut that was used, one of the constants
	 * defined in BaseLaunchShortcut
	 * @return Launch shortcut type
	 */
	public String getLaunchType() {return DEPLOY_TYPE;}
	
	@Override
	public void launch(ISelection selection, String mode) {
		//Extract resource from selection
		StructuredSelection sel = (StructuredSelection)selection;
		IProject activeProject = null;
		if (sel.getFirstElement() instanceof IProject) {
			activeProject = (IProject) sel.getFirstElement();
		} else {
			return;
		}
        
        //Run config using project found in extracted resource, with indicated mode
        runConfig(activeProject, mode);
	}

	@Override
	public void launch(IEditorPart editor, String mode) {
		//Extract resource from editor
		if(editor  != null){
		    IFileEditorInput input = (IFileEditorInput)editor.getEditorInput();
		    IFile file = input.getFile();
		    IProject activeProject = file.getProject();
		    
		    //If editor existed, run config using extracted resource in indicated mode
		    runConfig(activeProject, mode);
		}else{
			System.err.println("editor was null");
		}

	}
	
	/**
	 * Runs the ant script using the correct target for the indicated mode (deploy to cRIO or just compile)
	 * @param activeProj The project that the script will be run on/from
	 * @param mode The mode it will be run in (ILaunchManager.RUN_MODE or ILaunchManager.DEBUG_MODE)
	 */
	public void runConfig(IProject activeProj, String mode){
		String targets = "simulate";
		    
		if(mode.equals(ILaunchManager.RUN_MODE)){
			if(getLaunchType().equals(DEPLOY_TYPE)){
				targets = "simulate";
			}
		} else if ((mode.equals(ILaunchManager.DEBUG_MODE))&&(getLaunchType().equals(DEPLOY_TYPE))) {
			targets = "debug-simulate";
			try{
				PlatformUI.getWorkbench().showPerspective(IDebugUIConstants.ID_DEBUG_PERSPECTIVE, 
					PlatformUI.getWorkbench().getActiveWorkbenchWindow());
				
			}catch(Exception e){}
		}
		
		if((lastDeploy != null)&&(!lastDeploy.isTerminated())){
			WPILibCPPPlugin.logInfo("Last deploy running");
			// Find the server connection thread and kill it
			Vector<ThreadGroup> threadGroups = new Vector<ThreadGroup>();
	        ThreadGroup root = Thread.currentThread().getThreadGroup().getParent();
	        while (root.getParent() != null) {root = root.getParent();}
	        threadGroups.add(root);
	        ThreadGroup threadGroup = threadGroups.remove(0);
	        int numThreads = threadGroup.activeCount();
	        Thread[] threads = new Thread[numThreads*100];
            numThreads = threadGroup.enumerate(threads, true);
            
            for(Thread current: threads){
            	if(current != null){
            		if(current.getName().equals(ANT_SERVER_THREAD_NAME)){
            			try {
            				// Manually end thread and then try terminating launch
            				Method stopMethod = current.getClass().getMethod("stop");
            				stopMethod.invoke(current);
            				lastDeploy.terminate();
            				break;
            			} catch(Exception e){
                          WPILibCPPPlugin.logError("Error killing ant thread.", e);
                        }
            		}
            	}
            }
            
            WPILibCPPPlugin.logInfo("Waiting");
            try{wait(1000);}catch(Exception e){}
		}
		
		WPILibCPPPlugin.logInfo("Running ant file: " + activeProj.getLocation().toOSString() + File.separator + "build.xml");
		WPILibCPPPlugin.logInfo("Targets: " + targets + ", Mode: " + mode);
		lastDeploy = AntLauncher.runAntFile(new File (activeProj.getLocation().toOSString() + File.separator + "build.xml"), targets, null, mode);
		
		try {
			activeProj.refreshLocal(Resource.DEPTH_INFINITE, null);
		} catch (Exception e) {}
	}
}

