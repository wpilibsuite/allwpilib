package edu.wpi.first.wpilib.plugins.core.installer;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.jface.dialogs.MessageDialog;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;

public abstract class AbstractInstaller {
	protected File installLocation;
	protected String version, installedVersion;

	public AbstractInstaller(String version, String installedVersion, String path) {
		this.installLocation = new File(path);
		this.version = version;
		this.installedVersion = installedVersion;
	}

	/**
	 * @return The name of the feature being installed.
	 */
	protected abstract String getFeatureName();

	/**
	 * Update the installed version to the latest version.
	 * @param version The latest version installed.
	 */
	protected abstract void updateInstalledVersion(String version);

	/**
	 * @return The input stream to the zip file being installed.
	 */
	protected abstract InputStream getInstallResourceStream();

	public void installIfNecessary() {
		final Job installJob = new Job("Install " + getFeatureName()) {

			@Override
			protected IStatus run(IProgressMonitor monitor) {
				monitor.beginTask(getTaskMessage(), IProgressMonitor.UNKNOWN);
				WPILibCore.logInfo("Installing "+getFeatureName()+" if necessary");

				if (!isInstalled()) {
					WPILibCore.logInfo("Install necessary for " + getFeatureName());
					try {
						install();
					} catch (InstallException e) {
                        WPILibCore.logError("Error installing "+getFeatureName(), e);
						return new Status(IStatus.ERROR, WPILibCore.PLUGIN_ID,
								getErrorMessage(e));
					}
				}
				
				updateInstalledVersion(version);
				WPILibCore.logInfo("Installed" + getFeatureName());

				return Status.OK_STATUS;
			}

			private String getErrorMessage(InstallException ex) {
				String message = "Unable to install " + getFeatureName();
				if (ex.getCause() != null) {
					message += ": " + ex.getCause().getMessage();
				} else if (ex.getMessage() != null) {
					message += ": " + ex.getMessage();
				}

				message += ". See console for details.";
				return message;
			}

			private String getTaskMessage() {
				try {
					return "Extracting to " + installLocation.getCanonicalPath();
				} catch (IOException ex) {
                    WPILibCore.logError("installIfNecessary().getTaskMessage()", ex);
					return "Extracting";
				}
			}
		};

		installJob.setUser(true);
		installJob.setRule(ResourcesPlugin.getWorkspace().getRuleFactory().buildRule());
		installJob.schedule();
	}

	/**
	 * This function has been updated to guarantee that the wpilib folder date
	 * is older than the jar file being run, which ensures up to date tools.
	 *
	 * @return True for is there and newer, false otherwise.
	 */
	protected boolean isInstalled() {
		return installLocation.exists() && !version.contains("DEVELOPMENT") && version.equals(installedVersion);
	}

	/**
	 * This function will delete an old wpilib subfolder if necessary and then copy
	 * the resource stream to the intended directory.
	 *
	 * @throws InstallException if bad things happen ...
	 */
	protected void install() throws InstallException {
		if(installLocation.exists()) {
			if(!removeFileHandler(installLocation, true)) {
				MessageDialog.openError(null, "Error",
					String.format("Could not update the old wpilib folder.%n"
							+ "Please close any WPILib tools and restart Eclipse."));
			}
			
				//removeFileHandler(installLocation, false);
		}

		installLocation.mkdirs();
		final String osName = System.getProperty("os.name");
		try {
			if (osName.startsWith("Mac OS X") || osName.startsWith("Linux")) { // Unix-like OSes must preserve the executable bit; call unzip
				final File tmpFile = File.createTempFile(getFeatureName()+"-", ".zip");
				try {
					// Copy to temporary file
					try (final InputStream zip = getInstallResourceStream();
							final FileOutputStream fout = new FileOutputStream(tmpFile)) {
						copyStreams(zip, fout);
					}

					// Call 'unzip'
					final String[] cmd = {"unzip" , "-o", tmpFile.getAbsolutePath(), "-d", installLocation.getAbsolutePath()};
					WPILibCore.logInfo("unzip "+tmpFile.getAbsolutePath()+" -d "+installLocation.getAbsolutePath());
					final Process unzipProcess = DebugPlugin.exec(cmd, installLocation);
					try (final InputStream is = unzipProcess.getInputStream()) {
						copyStreams(is, System.out); // Copy output to console
					}

					// Check result
					final int exitCode = unzipProcess.waitFor();
					if (exitCode > 1 || exitCode < 0) { // Exit code 1 indicates success with warnings
						throw new InstallException("Unzip process failed with code " + exitCode);
					}
				} finally {
					tmpFile.delete();
				}
			} else {
				ZipInputStream zip = new ZipInputStream(getInstallResourceStream());
				ZipEntry entry = zip.getNextEntry();
				while (entry != null) {
					WPILibCore.logInfo("\tZipEntry " + entry + ": " + entry.getSize());
					File f = new File(installLocation, entry.getName());
					if (entry.isDirectory()) {
						f.mkdirs();
					} else {
						FileOutputStream fo = new FileOutputStream(f);
						copyStreams(zip, fo);
						fo.close();
					}

					zip.closeEntry();
					entry = zip.getNextEntry();
				}
				zip.close();
			}
		} catch (IOException | CoreException | InterruptedException ex) {
			throw new InstallException("Install encountered a problem", ex);
		}
	}

	private static void copyStreams(InputStream source, OutputStream destination) throws IOException {
		byte[] buffer = new byte[1024];
		int len;

		while((len = source.read(buffer)) >= 0){
			destination.write(buffer,0,len);
		}
	}

	/**
	 * Recursively remove all of the files and folders described by this file handler.
	 *
	 * @param file The file to remove
	 * @param testRun True to just test if the files can be deleted
	 * @return True if this and all subFiles were removed, false otherwise.
	 */
	private static boolean removeFileHandler(File file, boolean testRun) {
		// if normal files (data files and the like)
		if(file.isFile()) {
			if(testRun) return file.getParentFile().canWrite();
			else return file.delete();
		}
		// if folders
		else if(file.isDirectory()) {
			for(File f : file.listFiles()) {
				if(!removeFileHandler(f, testRun))
					return false;
			}
			if(testRun) return file.getParentFile().canWrite();
			else return file.delete();
		}
		// I'm not sure what to do if the file is not normal or a directory ...
		else return false;
	}

	/**
	 * Indicates that an attempt to install a resource failed.
	 */
	private static class InstallException extends Exception {
		private static final long serialVersionUID = 4883122446098399588L;

		/**
		 * @see Exception#Exception()
		 */
		@SuppressWarnings("unused")
		public InstallException() {
			super();
		}

		/**
		 * @see Exception#Exception(String)
		 */
		public InstallException(String message) {
			super(message);
		}

		/**
		 * @see Exception#Exception(String, Throwable)
		 */
		public InstallException(String message, Throwable cause) {
			super(message, cause);
		}
	}
}
