package edu.wpi.first.wpilib.plugins.cpp.installer;

import java.io.InputStream;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.variables.IStringVariableManager;
import org.eclipse.core.variables.IValueVariable;
import org.eclipse.core.variables.VariablesPlugin;

import edu.wpi.first.wpilib.plugins.core.installer.AbstractInstaller;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;
import edu.wpi.first.wpilib.plugins.cpp.preferences.PreferenceConstants;

/**
 * Installs the given version of WPILib into the correct location. Where the
 * install directory is usually ~/wpilib/cpp/version.
 * 
 * @author alex
 */
public class CPPInstaller extends AbstractInstaller {

	public CPPInstaller(String version) {
		super(version, 
				WPILibCPPPlugin.getDefault().getPreferenceStore().getString(PreferenceConstants.LIBRARY_INSTALLED), WPILibCPPPlugin.getDefault().getCPPDir());
	}
	@Override
	protected void updateInstalledVersion(String version) {
		WPILibCPPPlugin.getDefault().getPreferenceStore().setValue(PreferenceConstants.LIBRARY_INSTALLED,
				version);
		
		IStringVariableManager vm = VariablesPlugin.getDefault().getStringVariableManager();
		try
		{
			if (System.getProperty("os.name").startsWith("Windows"))
			{
				IValueVariable vv = vm.getValueVariable("HOME");
				if (vv == null)
					vm.addVariables(new IValueVariable[]{vm.newValueVariable("HOME", "user.home directory", false,System.getProperty("user.home"))});
				else
				{
					if (!System.getProperty("user.home").equals(vm.performStringSubstitution("${HOME}")))
						vv.setValue(System.getProperty("user.home"));
				}
			}
		} catch (CoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw new RuntimeException(e);
		}
	}
	
	@Override
	protected InputStream getInstallResourceStream() {
		return CPPInstaller.class.getResourceAsStream("/resources/cpp.zip");
	}

	@Override
	protected String getFeatureName()
	{
		return "cpp";
	}
}
