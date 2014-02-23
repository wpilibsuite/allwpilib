package edu.wpi.first.wpilib.plugins.core.installer;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import javax.swing.JOptionPane;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.debug.core.DebugPlugin;
import org.omg.CORBA.Environment;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;

public abstract class AbstractInstaller {
	protected File installLocation;
	protected String version;

	public AbstractInstaller(String version) {
		this.installLocation = new File(WPILibCore.getDefault().getWPILibBaseDir()
				+ File.separator + getFeatureName() + File.separator + version);
		this.version = version;
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
		System.out.println("Installing "+getFeatureName()+" if necessary");
		try {
			System.out.println("Install Location: "
					+ installLocation.getCanonicalPath());
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}


		if (!isInstalled()) {
			System.out.println("Install necessary");
			try {
				install();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		updateInstalledVersion(version);
		System.out.println("Installed");
	}

	/**
	 * This function has been updated to guarantee that the wpilib folder date
	 * is older than the jar file being run, which ensures up to date tools.
	 * 
	 * @return True for is there and newer, false otherwise.
	 */
	public boolean isInstalled() {
		if(installLocation.exists())
		{
			File coreJar = new File(AbstractInstaller.class.getProtectionDomain()
					.getCodeSource().getLocation().getPath());
			if(installLocation.lastModified() <= coreJar.lastModified())
			{
				return false;
			}
			else return true;
		}
		else return false;
	}

	/**
	 * This function will delete an old wpilib subfolder if necessary and then copy
	 * the resource stream to the intended directory.
	 * 
	 * @throws IOException if bad things happen ...
	 */
	public void install() throws IOException {
		if(installLocation.exists()) {
			if(!removeFileHandler(installLocation, true)) {
				JOptionPane.showMessageDialog(null, 
					String.format("Could not update the old wpilib folder.%n"
							+ "Please close any WPILib tools and restart Eclipse."));
			}
			else
				removeFileHandler(installLocation, false);
		}
			
		installLocation.mkdirs();
		final String osName = System.getProperty("os.name");
		if (osName.startsWith("Mac OS X") || osName.startsWith("Linux")) { // Unix-like OSes must preserve the executable bit; call unzip
			InputStream zip = getInstallResourceStream();
			File tmpFile = File.createTempFile(getFeatureName()+"-", ".zip");
			FileOutputStream fout = new FileOutputStream(tmpFile);
			copyStreams(zip, fout);
			zip.close();
			fout.close();
			String[] cmd = {"unzip", tmpFile.getAbsolutePath(), "-d", installLocation.getAbsolutePath()};
			System.out.println("unzip "+tmpFile.getAbsolutePath()+" -d "+installLocation.getAbsolutePath());
			try {
				InputStream is = DebugPlugin.exec(cmd, installLocation).getInputStream();
				copyStreams(is, System.out);
			} catch (CoreException e) {
				e.printStackTrace();
			}
		} else {
			ZipInputStream zip = new ZipInputStream(getInstallResourceStream());
			ZipEntry entry = zip.getNextEntry();
			while (entry != null) {
				System.out.println("\tZipEntry " + entry + ": " + entry.getSize());
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
}
