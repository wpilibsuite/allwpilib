package edu.wpi.first.wpilib.plugins.simulation.installer;

/**
 * @author peter mitrano <pdmitrano@wpi.edu>
 */

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Paths;

import javax.net.ssl.HttpsURLConnection;

import org.eclipse.jface.preference.IPreferenceStore;
import org.osgi.service.log.LogEntry;

import edu.wpi.first.wpilib.plugins.core.installer.AbstractInstaller;
import edu.wpi.first.wpilib.plugins.simulation.WPILibSimulationPlugin;
import edu.wpi.first.wpilib.plugins.simulation.preferences.PreferenceConstants;

public class SimulationInstaller extends AbstractInstaller {

	public SimulationInstaller(String version) {
		// copy gazebo plugins wpilib/simulation/plugins
		super(version, WPILibSimulationPlugin.getDefault().getPreferenceStore()
				.getString(PreferenceConstants.LIBRARY_INSTALLED),
				WPILibSimulationPlugin.getDefault().getPluginsDir());
	}

	public void installModels() {
		// download models
		// temporarily hardcoding link, but should be smarter

		String inputFilePath = WPILibSimulationPlugin.getDefault()
				.getSimulationDir() + File.separator + "models.zip";
		File inputFile = new File(inputFilePath);
		
		try {
			if (inputFile.exists()) {
				WPILibSimulationPlugin.logInfo("models doesn't need to be downloaded again");
			} else {
				URL url = new URL("https://usfirst.collab.net/sf/frs/do/downloadFile/projects.wpilib/frs.simulation.frcsim_gazebo_models/frs1162?dl=1");
				HttpsURLConnection con = (HttpsURLConnection) url.openConnection();
				try (InputStream stream = con.getInputStream()) {
					WPILibSimulationPlugin.logInfo("downloading models zip");
					Files.copy(stream, Paths.get(inputFilePath));
				}
			}
			// unzip
      WPILibSimulationPlugin.logInfo("unzipping to "+ WPILibSimulationPlugin.getDefault().getSimulationDir());
			super.installIfNecessary(inputFilePath, WPILibSimulationPlugin.getDefault().getSimulationDir());
		} catch (MalformedURLException mue) {
			WPILibSimulationPlugin
					.logInfo("Malformed URL Exception when downloading models");
			WPILibSimulationPlugin.logInfo(mue.getLocalizedMessage());
		} catch (IOException ioe) {
			WPILibSimulationPlugin
					.logInfo("IO Exception when downloading models");
			WPILibSimulationPlugin.logInfo(ioe.getLocalizedMessage());
		}

	}

	public void installPlugins() {
		super.installIfNecessary();
	}

	@Override
	protected String getFeatureName() {
		return "simulation";
	}

	@Override
	protected void updateInstalledVersion(String version) {
		IPreferenceStore prefs = WPILibSimulationPlugin.getDefault()
				.getPreferenceStore();
		prefs.setValue(PreferenceConstants.LIBRARY_INSTALLED, version);

	}

	@Override
	protected InputStream getInstallResourceStream() {
		return SimulationInstaller.class
				.getResourceAsStream("/resources/simulation.zip");
	}
}
