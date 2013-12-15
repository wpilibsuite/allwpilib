package edu.wpi.first.wpilib.plugins.java.installer;

import java.io.InputStream;
import java.util.Properties;

import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.installer.AbstractInstaller;
import edu.wpi.first.wpilib.plugins.java.WPILibJavaPlugin;
import edu.wpi.first.wpilib.plugins.java.preferences.PreferenceConstants;

/**
 * Installs the given version of WPILib into the correct location. Where the
 * install directory is usually ~/wpilib/java/version.
 * 
 * @author alex
 */
public class JavaInstaller extends AbstractInstaller {

	public JavaInstaller(String version) {
		super(version);
	}

	@Override
	protected String getFeatureName() {
		return "java";
	}

	@Override
	protected void updateInstalledVersion(String version) {
		IPreferenceStore prefs = WPILibJavaPlugin.getDefault().getPreferenceStore();
		if (prefs.getBoolean(PreferenceConstants.UPDATE_LIBRARY_VERSION)) {
			System.out.println("Forcing library version to "+version);
	    	Properties props = WPILibCore.getDefault().getProjectProperties(null);
	    	props.setProperty("version", version);
	    	WPILibCore.getDefault().saveGlobalProperties(props);
			prefs.setValue(PreferenceConstants.LIBRARY_VERSION, version);
		}
    	WPILibJavaPlugin.getDefault().updateProjects();
	}

	@Override
	protected InputStream getInstallResourceStream() {
		return JavaInstaller.class.getResourceAsStream("/resources/java.zip");
	}
}
