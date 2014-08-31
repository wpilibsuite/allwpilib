package edu.wpi.first.wpilib.plugins.cpp.toolchains.linux;

import java.io.InputStream;

import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.installer.AbstractInstaller;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;
import edu.wpi.first.wpilib.plugins.cpp.preferences.PreferenceConstants;

public class ToolchainInstaller extends AbstractInstaller {

	public ToolchainInstaller(String version) {
		super(version, 
				WPILibCPPPlugin.getDefault().getPreferenceStore().getString(PreferenceConstants.TOOLCHAIN_INSTALLED), WPILibCPPPlugin.getDefault().getToolchain());
	}

	@Override
	protected String getFeatureName() {
		return "toolchains";
	}

	@Override
	protected void updateInstalledVersion(String version) {
		WPILibCPPPlugin.getDefault().getPreferenceStore().setValue(PreferenceConstants.TOOLCHAIN_INSTALLED,
				WPILibCPPPlugin.getDefault().getCurrentVersion());
	}

	@Override
	protected InputStream getInstallResourceStream() {
		return ToolchainInstaller.class.getResourceAsStream("/resources/toolchain.zip");
	}

}
