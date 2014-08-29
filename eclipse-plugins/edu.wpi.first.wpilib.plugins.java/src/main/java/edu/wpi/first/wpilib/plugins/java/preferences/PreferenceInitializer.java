package edu.wpi.first.wpilib.plugins.java.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.java.WPILibJavaPlugin;

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
		IPreferenceStore store = WPILibJavaPlugin.getDefault().getPreferenceStore();
		if (!store.contains(PreferenceConstants.LIBRARY_INSTALLED))
			store.setValue(PreferenceConstants.LIBRARY_INSTALLED, "none");
	}
}
