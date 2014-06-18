package edu.wpi.first.wpilib.plugins.java;

import java.io.File;
import java.io.FileInputStream;
import java.util.Properties;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jdt.core.JavaCore;
import org.eclipse.jdt.core.JavaModelException;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.ant.AntPropertiesParser;
import edu.wpi.first.wpilib.plugins.java.installer.JavaInstaller;

/**
 * The activator class controls the plug-in life cycle
 */
public class WPILibJavaPlugin extends AbstractUIPlugin implements IStartup {

	// The plug-in ID
	public static final String PLUGIN_ID = "WPILib_Java_Robot_Development"; //$NON-NLS-1$

	// The shared instance
	private static WPILibJavaPlugin plugin;
	
	/**
	 * The constructor
	 */
	public WPILibJavaPlugin() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
		plugin = null;
		super.stop(context);
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static WPILibJavaPlugin getDefault() {
		return plugin;
	}

	public String getCurrentVersion() {
		try {
			Properties props = new AntPropertiesParser(WPILibJavaPlugin.class.getResourceAsStream("/resources/configuration.properties")).getProperties();
			if (props.getProperty("version").startsWith("$")) {
				return "DEVELOPMENT";
			} else {
				return props.getProperty("version");
			}
		} catch (CoreException e) {
            WPILibJavaPlugin.logError("Error getting properties.", e);
			return "DEVELOPMENT";
		}
	}

	public Properties getProjectProperties(IProject project) {
		Properties defaults = WPILibCore.getDefault().getProjectProperties(project);
		Properties props;
		try {
			File file = new File(WPILibCore.getDefault().getWPILibBaseDir()+"/java/"+getCurrentVersion()+"/ant/build.properties");
			props = new AntPropertiesParser(new FileInputStream(file)).getProperties(defaults);
		} catch (Exception e) {
            WPILibJavaPlugin.logError("Error getting properties.", e);
			props = new Properties(defaults);
		}
		return props;
	}
	
	public void updateProjects() {
		// Get the root of the workspace
	    IWorkspace workspace = ResourcesPlugin.getWorkspace();
	    IWorkspaceRoot root = workspace.getRoot();
	    // Get all projects in the workspace
	    IProject[] projects = root.getProjects();
	    // Loop over all projects
	    for (IProject project : projects) {
	      try {
	    	  updateVariables(project);
	      } catch (CoreException e) {
            WPILibJavaPlugin.logError("Error updating projects.", e);
	      }
	    }
	}
	
	public void updateVariables(IProject project) throws CoreException {
		Properties props = WPILibJavaPlugin.getDefault().getProjectProperties(project);

		try {
			JavaCore.setClasspathVariable("wpilib", new Path(props.getProperty("wpilib.jar")), null);
			JavaCore.setClasspathVariable("wpilib.sources", new Path(props.getProperty("wpilib.sources")), null);
			JavaCore.setClasspathVariable("networktables", new Path(props.getProperty("networktables.jar")), null);
			JavaCore.setClasspathVariable("networktables.sources", new Path(props.getProperty("networktables.sources")), null);
		} catch (JavaModelException e) {
            // Classpath variables didn't get set
            WPILibJavaPlugin.logError("Error setting classpath..", e);
		}
	}

	@Override
	public void earlyStartup() {
		new JavaInstaller(getCurrentVersion()).installIfNecessary();
	}

	public static void logInfo(String msg) {
		getDefault().getLog().log(new Status(Status.INFO, PLUGIN_ID, Status.OK, msg, null));
	}

	public static void logError(String msg, Exception e) {
		getDefault().getLog().log(new Status(Status.ERROR, PLUGIN_ID, Status.OK, msg, e));
	}
}
