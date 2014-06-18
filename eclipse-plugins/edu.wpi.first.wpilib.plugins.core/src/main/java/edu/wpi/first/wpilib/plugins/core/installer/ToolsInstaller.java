package edu.wpi.first.wpilib.plugins.core.installer;

import java.io.InputStream;
import java.util.Properties;

import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.preferences.PreferenceConstants;

public class ToolsInstaller extends AbstractInstaller {

	public ToolsInstaller(String version) {
		super(version);
	}

	@Override
	protected String getFeatureName() {
		return "tools";
	}

	@Override
	protected void updateInstalledVersion(String version) {
		IPreferenceStore prefs = WPILibCore.getDefault().getPreferenceStore();
		if (prefs.getBoolean(PreferenceConstants.UPDATE_TOOLS_VERSION)) {
			WPILibCore.logInfo("Forcing library version to "+version);
	    	Properties props = WPILibCore.getDefault().getProjectProperties(null);
	    	props.setProperty("version", version);
	    	WPILibCore.getDefault().saveGlobalProperties(props);
			prefs.setValue(PreferenceConstants.TOOLS_VERSION, version);
		}
	}

	@Override
	protected InputStream getInstallResourceStream() {
		return ToolsInstaller.class.getResourceAsStream("/resources/tools.zip");
	}
}
