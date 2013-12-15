package edu.wpi.first.wpilib.plugins.core.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;

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
		IPreferenceStore store = WPILibCore.getDefault().getPreferenceStore();
		store.setDefault(PreferenceConstants.TEAM_NUMBER,
				WPILibCore.getDefault().getProjectProperties(null).getProperty("team-number", "0"));
		store.setDefault(PreferenceConstants.TOOLS_VERSION, WPILibCore.getDefault().getDefaultVersion());
		store.setDefault(PreferenceConstants.UPDATE_TOOLS_VERSION, true);
	}
}
