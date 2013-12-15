package edu.wpi.first.wpilib.plugins.cpp.toolchains.windows;

import java.io.InputStream;

import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.installer.AbstractInstaller;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;
import edu.wpi.first.wpilib.plugins.cpp.preferences.PreferenceConstants;

public class ToolchainInstaller extends AbstractInstaller {

	public ToolchainInstaller(String version) {
		super(version);
	}

	@Override
	protected String getFeatureName() {
		return "toolchains";
	}

	@Override
	protected void updateInstalledVersion(String version) {
		IPreferenceStore prefs = WPILibCPPPlugin.getDefault().getPreferenceStore();
		if (prefs.getBoolean(PreferenceConstants.UPDATE_TOOLCHAIN_VERSION)) {
			System.out.println("Forcing library version to "+version);
			prefs.setValue(PreferenceConstants.TOOLCHAIN_VERSION, version);
		}
	}

	@Override
	protected InputStream getInstallResourceStream() {
		return ToolchainInstaller.class.getResourceAsStream("/resources/toolchain.zip");
	}

}
