package edu.wpi.first.wpilib.plugins.cpp.wizards.file_template;

import java.io.File;

import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.IDialogPage;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import edu.wpi.first.wpilib.plugins.core.nature.FRCProjectNature;
import edu.wpi.first.wpilib.plugins.core.wizards.IProjectFilter;
import edu.wpi.first.wpilib.plugins.core.wizards.ProjectComboField;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;

/**
 * The "New" wizard page allows setting the container for the new file as well
 * as the file name. The page will only accept file name without the extension
 * OR with the extension that matches the expected one (mpe).
 */

public class FileTemplateWizardMainPage extends WizardPage {
	private IProject project;
	private String ending;
	private ProjectComboField projectsCombo;
	private Text classNameText;
	private Text folderText;

	/**
	 * Constructor for SampleNewWizardPage.
	 * 
	 * @param pageName
	 */
	public FileTemplateWizardMainPage(String type, IProject project, String ending, ISelection selection) {
		super("wizardPage");
		setTitle("Create New "+type);
		setDescription("This wizard creates a new "+type.toLowerCase()+" from a template.");
		this.project = project;
		this.ending = ending;
	}

	/**
	 * @see IDialogPage#createControl(Composite)
	 */
	public void createControl(Composite parent) {
		Composite container = new Composite(parent, SWT.NULL);
		GridLayout layout = new GridLayout();
		container.setLayout(layout);
		layout.numColumns = 2;
		layout.verticalSpacing = 9;

		Label label = new Label(container, SWT.NULL);
		label.setText("Pro&ject:");

		projectsCombo = new ProjectComboField(container, SWT.BORDER | SWT.SINGLE,
				new IProjectFilter() {
			@Override public boolean accept(IProject project) {
				try {
					return project.hasNature(FRCProjectNature.FRC_PROJECT_NATURE)
							&& project.hasNature(CCProjectNature.C_NATURE_ID);
				} catch (CoreException e) {
                    WPILibCPPPlugin.logError("Error checking for FRC C++ project.", e);
					return false;
				}
			}
		});
		GridData gd = new GridData(GridData.FILL_HORIZONTAL);
		projectsCombo.setLayoutData(gd);
		projectsCombo.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				dialogChanged();
			}
		});
		
		label = new Label(container, SWT.NULL);
		label.setText("Class &Name:");

		classNameText = new Text(container, SWT.BORDER | SWT.SINGLE);
		gd = new GridData(GridData.FILL_HORIZONTAL);
		classNameText.setLayoutData(gd);
		classNameText.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				dialogChanged();
			}
		});

		label = new Label(container, SWT.NULL);
		label.setText("&Folder:");

		folderText = new Text(container, SWT.BORDER | SWT.SINGLE);
		gd = new GridData(GridData.FILL_HORIZONTAL);
		folderText.setLayoutData(gd);
		folderText.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				dialogChanged();
			}
		});
		
		initialize();
		dialogChanged();
		setControl(container);
	}

	/**
	 * Tests if the current workbench selection is a suitable container to use.
	 */

	private void initialize() {
		projectsCombo.setProject(project);
		folderText.setText(getDefaultFolder());
	}

	/**
	 * Ensures that both text fields are set.
	 */

	private void dialogChanged() {
		String className = getClassName();
		String folder = getFolder();

		// Update the default package if necessary
		if (project == null || !project.equals(projectsCombo.getProject())) {
			String oldDefault = getDefaultFolder();
			project = projectsCombo.getProject();
			if (folder.equals(oldDefault)) {
				folderText.setText(getDefaultFolder());
			}
		}

		if (!projectsCombo.isValid()) {
			updateStatus("Must select a project.");
			return;
		}

		if (className.length() == 0) {
			updateStatus("Class name must be specified");
			return;
		}
		if (!className.matches("^([a-zA-Z_]{1}[a-zA-Z0-9_]*)$")) {
			updateStatus("Must be a valid java class name");
			return;
		}
		if (folder.length() == 0) {
			updateStatus("Package must be specified");
			return;
		}
		updateStatus(null);
	}

	private void updateStatus(String message) {
		setErrorMessage(message);
		setPageComplete(message == null);
	}
	
	public IProject getProject() {
		return projectsCombo.getProject();
	}

	public String getClassName() {
		return classNameText.getText();
	}

	public String getFolder() {
		return folderText.getText();
	}
	
	public String getDefaultFolder() {
		WPILibCPPPlugin.logInfo("Project: "+project);
		return "src"+File.separator+ending;
	}
}
