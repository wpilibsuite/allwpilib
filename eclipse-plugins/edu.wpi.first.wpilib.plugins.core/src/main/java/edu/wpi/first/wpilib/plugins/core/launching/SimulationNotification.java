package edu.wpi.first.wpilib.plugins.core.launching;

import java.awt.Desktop;
import java.io.File;
import java.io.IOException;
import java.lang.System;
import java.net.URI;
import java.net.URISyntaxException;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.jface.dialogs.MessageDialog;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;

public class SimulationNotification {
	private static final String URL = "https://wpilib.screenstepslive.com/s/4485/m/23353";
	private static final String SIMULATION_UNSUPPORTED_MSG =
			"Simulation may not be support on your operating system.\n" +
			"For more information see: " + URL;
	
	public static void showUnsupported() {
		String title = "Simulation Unsupported";
		String message = SIMULATION_UNSUPPORTED_MSG;
		MessageDialog.openError(null, title, message);
		try {
			Desktop.getDesktop().browse(new URI(URL));
		} catch (IOException e) {
			WPILibCore.logError("Can't open "+URL, e);
		} catch (URISyntaxException e) {
			WPILibCore.logError("Can't open "+URL, e);
		}
	}
	
	public static boolean supportsSimulation() {
		String os = System.getProperty("os.name");
		//for now this is good enough, but we still need better handling if they have this OS but have not installed FRCSim
		return (os.equals("Windows 8") || os.equals("Windows 7") || os.equals("Linux") || os.equals("Windows 8.1"));
	}
}
