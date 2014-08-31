package edu.wpi.first.wpilib.plugins.cpp.installer;

import java.io.InputStream;
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
		super(version, 
				WPILibCPPPlugin.getDefault().getPreferenceStore().getString(PreferenceConstants.LIBRARY_INSTALLED), WPILibCPPPlugin.getDefault().getCPPDir());
	}
	@Override
	protected void updateInstalledVersion(String version) {
		WPILibCPPPlugin.getDefault().getPreferenceStore().setValue(PreferenceConstants.LIBRARY_INSTALLED,
				WPILibCPPPlugin.getDefault().getCurrentVersion());
	}
	
	@Override
	protected InputStream getInstallResourceStream() {
		return CPPInstaller.class.getResourceAsStream("/resources/cpp.zip");
	}

	@Override
	protected String getFeatureName()
	{
		return "cpp";
	}
}
