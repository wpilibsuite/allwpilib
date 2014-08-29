package edu.wpi.first.wpilib.plugins.java.installer;

import java.io.InputStream;
import org.eclipse.jface.preference.IPreferenceStore;

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
		super(version, 
				WPILibJavaPlugin.getDefault().getPreferenceStore().getString(PreferenceConstants.LIBRARY_INSTALLED),
				WPILibJavaPlugin.getDefault().getJavaPath());
	}

	@Override
	protected String getFeatureName() {
		return "java";
	}

	@Override
	protected void updateInstalledVersion(String version) {
		IPreferenceStore prefs = WPILibJavaPlugin.getDefault().getPreferenceStore();
			prefs.setValue(PreferenceConstants.LIBRARY_INSTALLED, version);
	}

	@Override
	protected InputStream getInstallResourceStream() {
		return JavaInstaller.class.getResourceAsStream("/resources/java.zip");
	}
}
