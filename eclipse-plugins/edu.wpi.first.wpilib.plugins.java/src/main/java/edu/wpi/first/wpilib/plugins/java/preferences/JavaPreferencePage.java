package edu.wpi.first.wpilib.plugins.java.preferences;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Properties;

import org.eclipse.jface.preference.BooleanFieldEditor;
import org.eclipse.jface.preference.FieldEditorPreferencePage;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.IWorkbench;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.preferences.ComboFieldEditor;
import edu.wpi.first.wpilib.plugins.java.WPILibJavaPlugin;

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

public class JavaPreferencePage
	extends FieldEditorPreferencePage
	implements IWorkbenchPreferencePage {
	ComboFieldEditor wpiLibVersionEditor;
	BooleanFieldEditor autoUpdateEditor;

	public JavaPreferencePage() {
		super(GRID);
		setPreferenceStore(WPILibJavaPlugin.getDefault().getPreferenceStore());
		setDescription("Change workspace level settings for Java.");
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
	}

	private List<String> getInstalledVersions() {
		File[] dirs = new File(WPILibCore.getDefault().getWPILibBaseDir()+File.separator+"java")
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
		WPILibJavaPlugin.logInfo("Preferences initialized.");
		Properties props = WPILibCore.getDefault().getProjectProperties(null);
		getPreferenceStore().setValue(PreferenceConstants.LIBRARY_VERSION,
				props.getProperty("version", WPILibJavaPlugin.getDefault().getCurrentVersion()));
	}
	
	@Override public void performApply() {
		performOk();
	}
	
    @Override public boolean performOk() {
    	Properties props = WPILibCore.getDefault().getProjectProperties(null);
    	props.setProperty("version", wpiLibVersionEditor.getChoice());
    	WPILibCore.getDefault().saveGlobalProperties(props);
    	WPILibJavaPlugin.getDefault().updateProjects();
		return super.performOk();
	}
}
