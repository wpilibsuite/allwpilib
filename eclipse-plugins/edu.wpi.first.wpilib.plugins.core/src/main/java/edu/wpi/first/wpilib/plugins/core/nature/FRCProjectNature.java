package edu.wpi.first.wpilib.plugins.core.nature;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectNature;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;


/**
 * Project nature which configures any project which is given it to be an 
 * FRC Project, and designates it so FRC options will be enabled to act on
 * it
 * 
 */
public class FRCProjectNature implements IProjectNature {
	
	public static final String FRC_PROJECT_NATURE = 
		"edu.wpi.first.wpilib.plugins.core.nature.FRCProjectNature";
	
	
	private IProject internalProject;
	
	/**
	 * IStatus representing a failed configuration attempt
	 * 
	 * @author Ryan O'Meara
	 */
	private class FRCProjectFailedStatus implements IStatus{
		String message;
		
		public FRCProjectFailedStatus(String message){
			this.message = message;
		}
		
		@Override
		public IStatus[] getChildren() {return null;}

		@Override
		public int getCode() {return 0;}

		@Override
		public Throwable getException() {return null;}

		@Override
		public String getMessage() {return message;}

		@Override
		public String getPlugin() {return WPILibCore.PLUGIN_ID;}

		@Override
		public int getSeverity() {return ERROR;}

		@Override
		public boolean isMultiStatus() {return false;}

		@Override
		public boolean isOK() {return false;}

		@Override
		public boolean matches(int severityMask) {
			if((severityMask & ERROR) == ERROR){return true;}
			return false;
		}	
	}
	
	public FRCProjectNature(){
		internalProject = null;
	}
	
	@Override
	public void configure() throws CoreException {
		if(internalProject == null){
			throw new CoreException(
					new FRCProjectFailedStatus("No project set"));
		}
	}

	@Override
	public void deconfigure() throws CoreException {
		if(internalProject == null){
			throw new CoreException(
					new FRCProjectFailedStatus("No project set"));
		}
	}

	@Override
	public IProject getProject() {
		return internalProject;
	}

	@Override
	public void setProject(IProject project) {
		internalProject = project;
	}
}
