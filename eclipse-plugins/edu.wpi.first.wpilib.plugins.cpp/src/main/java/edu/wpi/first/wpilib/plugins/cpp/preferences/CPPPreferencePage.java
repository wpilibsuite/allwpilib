package edu.wpi.first.wpilib.plugins.cpp.preferences;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Properties;

import org.eclipse.jface.preference.BooleanFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.preferences.ComboFieldEditor;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;

/**
 * This class represents a preference page that
 * is contributed to the Preferences dialog. By 
 * subclassing <samp>FieldEditorPreferencePage</samp>, we
 * can use the field support built into JFace that allows
 * us to create a page that is small and knows how to 
 * save, restore and apply itself.
 * <p>
 * This page is used to modify preferences only. They
 * are stored in the preference store that belongs to
 * the main plug-in class. That way, preferences can
 * be accessed directly via the preference store.
 */

public class CPPPreferencePage
	extends FieldEditorPreferencePage
	implements IWorkbenchPreferencePage {
	ComboFieldEditor toolchainVersionEditor;
	BooleanFieldEditor autoUpdateToolchainEditor;
	private ComboFieldEditor wpiLibVersionEditor;
	private BooleanFieldEditor autoUpdateEditor;

	public CPPPreferencePage() {
		super(GRID);
		setPreferenceStore(WPILibCPPPlugin.getDefault().getPreferenceStore());
		setDescription("Change workspace level settings for C++.");
	}
	
	/**
	 * Creates the field editors. Field editors are abstractions of
	 * the common GUI blocks needed to manipulate various types
	 * of preferences. Each field editor knows how to save and
	 * restore itself.
	 */
	public void createFieldEditors() {
		wpiLibVersionEditor = new ComboFieldEditor(PreferenceConstants.LIBRARY_VERSION,
				"&Library Version:", getFieldEditorParent(), getInstalledVersions());
		addField(wpiLibVersionEditor);
		autoUpdateEditor =	new BooleanFieldEditor(PreferenceConstants.UPDATE_LIBRARY_VERSION,
				"&Auto Update Library Version", getFieldEditorParent());
		addField(autoUpdateEditor);
		toolchainVersionEditor = new ComboFieldEditor(PreferenceConstants.TOOLCHAIN_VERSION,
				"&Toolchain Version:", getFieldEditorParent(), getInstalledToolchains());
		addField(toolchainVersionEditor);
		autoUpdateToolchainEditor =	new BooleanFieldEditor(PreferenceConstants.UPDATE_TOOLCHAIN_VERSION,
				"&Auto Update Toolchain Version", getFieldEditorParent());
		addField(autoUpdateToolchainEditor);
	}

	private List<String> getInstalledVersions() {
		File[] dirs = new File(WPILibCore.getDefault().getWPILibBaseDir()+File.separator+"cpp")
							.listFiles(new FileFilter() {
			@Override public boolean accept(File f) {
				return f.isDirectory();
			}
		});
		List<String> versions = new ArrayList<String>();
		for (File dir : dirs) {
			versions.add(dir.getName());
		}
		Collections.sort(versions);
		return versions;
	}
	
	private List<String> getInstalledToolchains() {
		File[] dirs = new File(WPILibCore.getDefault().getWPILibBaseDir()+File.separator+"toolchains")
							.listFiles(new FileFilter() {
			@Override public boolean accept(File f) {
				return f.isDirectory();
			}
		});
		List<String> versions = new ArrayList<String>();
		for (File dir : dirs) {
			versions.add(dir.getName());
		}
		Collections.sort(versions);
		return versions;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
	 */
	public void init(IWorkbench workbench) {
		System.out.println("Preferences initialized.");
		Properties props = WPILibCore.getDefault().getProjectProperties(null);
		getPreferenceStore().setValue(PreferenceConstants.LIBRARY_VERSION,
				props.getProperty("cpp-version", WPILibCPPPlugin.getDefault().getCurrentVersion()));
	}
	
	@Override public void performApply() {
		performOk();
	}
	
    @Override public boolean performOk() {
    	Properties props = WPILibCore.getDefault().getProjectProperties(null);
    	props.setProperty("cpp-version", wpiLibVersionEditor.getChoice());
    	WPILibCore.getDefault().saveGlobalProperties(props);
		return super.performOk();
	}
}