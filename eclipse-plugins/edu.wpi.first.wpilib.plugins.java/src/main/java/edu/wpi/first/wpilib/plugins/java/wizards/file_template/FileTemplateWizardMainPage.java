package edu.wpi.first.wpilib.plugins.java.wizards.file_template;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jdt.core.IJavaElement;
import org.eclipse.jdt.core.IPackageFragmentRoot;
import org.eclipse.jdt.core.JavaCore;
import org.eclipse.jdt.core.JavaModelException;
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
import edu.wpi.first.wpilib.plugins.java.WPILibJavaPlugin;

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
	private Text packageText;

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
									&& project.hasNature(JavaCore.NATURE_ID);
						} catch (CoreException e) {
                            WPILibJavaPlugin.logError("Error looking for FRCJava project.", e);
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
		label.setText("&Package:");

		packageText = new Text(container, SWT.BORDER | SWT.SINGLE);
		gd = new GridData(GridData.FILL_HORIZONTAL);
		packageText.setLayoutData(gd);
		packageText.addModifyListener(new ModifyListener() {
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
		packageText.setText(getDefaultPackage());
	}

	/**
	 * Ensures that both text fields are set.
	 */

	private void dialogChanged() {
		String className = getClassName();
		String packageString = getPackage();

		// Update the default package if necessary
		if (project == null || !project.equals(projectsCombo.getProject())) {
			String oldDefault = getDefaultPackage();
			project = projectsCombo.getProject();
			if (packageString.equals(oldDefault)) {
				packageText.setText(getDefaultPackage());
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
		if (packageString.length() == 0) {
			updateStatus("Package must be specified");
			return;
		}
		if (!packageString.matches("^([a-zA-Z_]{1}[a-zA-Z0-9_]*(\\.[a-zA-Z_]{1}[a-zA-Z0-9_]*)*)$")) {
			updateStatus("Must be valid java package");
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

	public String getPackage() {
		return packageText.getText();
	}
	
	public String getDefaultPackage() {
		WPILibJavaPlugin.logInfo("Project: "+project);
		String defaultPackage = null;
		if (project != null) {
			try {
				IPackageFragmentRoot root = JavaCore.create(project)
						.getPackageFragmentRoot(project.getFolder("src"));
				String backupPackage = "";
				for (IJavaElement child : root.getChildren()) {
					if (child.getElementType()==IJavaElement.PACKAGE_FRAGMENT
							&& child.getElementName().endsWith("."+ending)) {
						defaultPackage = child.getElementName();
					}
					backupPackage =  child.getElementName();
				}
				if (defaultPackage == null) defaultPackage = backupPackage;
			} catch (JavaModelException e) {
                WPILibJavaPlugin.logError("Error getting default package.", e);
			}
		}
		if (defaultPackage != null) return defaultPackage;
		else return "";
	}
}
