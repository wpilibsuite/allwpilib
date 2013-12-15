package edu.wpi.first.wpilib.plugins.cpp.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;

/**
 * Class used to initialize default preference values.
 */
public class PreferenceInitializer extends AbstractPreferenceInitializer {

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer#initializeDefaultPreferences()
	 */
	public void initializeDefaultPreferences() {
		IPreferenceStore store = WPILibCPPPlugin.getDefault().getPreferenceStore();
		store.setDefault(PreferenceConstants.LIBRARY_VERSION,
				WPILibCore.getDefault().getProjectProperties(null)
				.getProperty("cpp-version", WPILibCPPPlugin.getDefault().getCurrentVersion()));
		store.setDefault(PreferenceConstants.UPDATE_LIBRARY_VERSION, true);
		store.setDefault(PreferenceConstants.TOOLCHAIN_VERSION, WPILibCPPPlugin.getDefault().getDefaultToolchainVersion());
		store.setDefault(PreferenceConstants.UPDATE_TOOLCHAIN_VERSION, true);
	}
}
