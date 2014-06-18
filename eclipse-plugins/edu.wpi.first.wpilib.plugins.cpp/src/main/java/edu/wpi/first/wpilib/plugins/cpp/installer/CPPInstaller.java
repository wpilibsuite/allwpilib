package edu.wpi.first.wpilib.plugins.cpp.installer;

import java.io.InputStream;
import java.util.Properties;

import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.installer.AbstractInstaller;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;
import edu.wpi.first.wpilib.plugins.cpp.preferences.PreferenceConstants;

/**
 * Installs the given version of WPILib into the correct location. Where the
 * install directory is usually ~/wpilib/cpp/version.
 * 
 * @author alex
 */
public class CPPInstaller extends AbstractInstaller {

	public CPPInstaller(String version) {
		super(version);
	}

	@Override
	protected String getFeatureName() {
		return "cpp";
	}

	@Override
	protected void updateInstalledVersion(String version) {
		IPreferenceStore prefs = WPILibCPPPlugin.getDefault().getPreferenceStore();
		if (prefs.getBoolean(PreferenceConstants.UPDATE_LIBRARY_VERSION)) {
			WPILibCPPPlugin.logInfo("Forcing library version to "+version);
	    	Properties props = WPILibCore.getDefault().getProjectProperties(null);
	    	props.setProperty("cpp-version", version);
	    	WPILibCore.getDefault().saveGlobalProperties(props);
			prefs.setValue(PreferenceConstants.LIBRARY_VERSION, version);
		}
	}

	@Override
	protected InputStream getInstallResourceStream() {
		return CPPInstaller.class.getResourceAsStream("/resources/cpp.zip");
	}
}
