package edu.wpi.first.wpilib.plugins.core.installer;

import java.io.InputStream;
import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.preferences.PreferenceConstants;

public class ToolsInstaller extends AbstractInstaller {

	public ToolsInstaller(String version) {
		super(version, WPILibCore.getDefault().getPreferenceStore().getString(PreferenceConstants.TOOLS_VERSION), WPILibCore.getDefault().getWPILibBaseDir() + "/tools");
	}

	@Override
	protected String getFeatureName() {
		return "tools";
	}

	@Override
	protected void updateInstalledVersion(String version) {
		IPreferenceStore prefs = WPILibCore.getDefault().getPreferenceStore();
		prefs.setValue(PreferenceConstants.TOOLS_VERSION, version);
		
	}

	@Override
	protected InputStream getInstallResourceStream() {
		return ToolsInstaller.class.getResourceAsStream("/resources/tools.zip");
	}
}
