package edu.wpi.first.wpilib.plugins.java.wizards.newproject;

import java.lang.reflect.InvocationTargetException;
import java.util.Properties;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchWizard;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.wizards.NewProjectMainPage;
import edu.wpi.first.wpilib.plugins.core.wizards.ProjectCreationUtils;
import edu.wpi.first.wpilib.plugins.core.wizards.ProjectType;
import edu.wpi.first.wpilib.plugins.core.wizards.TeamNumberPage;

/**
 * 
 * Example Docs:
 * This is a sample new wizard. Its role is to create a new file 
 * resource in the provided container. If the container resource
 * (a folder or a project) is selected in the workspace 
 * when the wizard is opened, it will accept it as the target
 * container. The wizard creates one file with the extension
 * "mpe". If a sample multi-page editor (also available
 * as a template) is registered for the same extension, it will
 * be able to open it.
 */

public class NewJavaWizard extends Wizard implements INewWizard {
	private TeamNumberPage teamNumberPage;
	private NewProjectMainPage page;
	private ISelection selection;

	/**
	 * Constructor for SampleNewWizard.
	 */
	public NewJavaWizard() {
		super();
		setNeedsProgressMonitor(true);
	}
	
	/**
	 * Adding the page to the wizard.
	 */

	public void addPages() {
		if (TeamNumberPage.needsTeamNumberPage()) {
			teamNumberPage = new TeamNumberPage(selection);
			addPage(teamNumberPage);
		}
		page = new NewProjectMainPage(selection, teamNumberPage);
		page.setProjectTypes(JavaProjectType.TYPES);
		page.setTitle("Create New Robot Java Project");
		page.setDescription("This wizard creates a new Robot Java Project configured to use WPILib for programming FRC robots.");
		addPage(page);
	}

	/**
	 * This method is called when 'Finish' button is pressed in
	 * the wizard. We will create an operation and run it
	 * using wizard as execution context.
	 */
	public boolean performFinish() {
		final String projectName = page.getProjectName();
		final String teamNumber = TeamNumberPage.getTeamNumberFromPage(teamNumberPage);
		final String packageName = page.getPackage();
		final ProjectType projectType = page.getProjectType();
		final String worldName = page.getWorld();
		System.out.println("Project: "+projectName+" Package: "+packageName+" Project Type: "+projectType);
		IRunnableWithProgress op = new IRunnableWithProgress() {
			public void run(IProgressMonitor monitor) throws InvocationTargetException {
				try {
					doFinish(projectName, teamNumber, packageName, projectType, worldName, monitor);
				} catch (CoreException e) {
					throw new InvocationTargetException(e);
				} finally {
					monitor.done();
				}
			}
		};
		try {
			getContainer().run(true, false, op);
		} catch (InterruptedException e) {
			return false;
		} catch (InvocationTargetException e) {
			Throwable realException = e.getTargetException();
			MessageDialog.openError(getShell(), "Error", realException.getMessage());
			return false;
		}
		return true;
	}
	
	/**
	 * The worker method. It will find the container, create the
	 * file if missing or just replace its contents, and open
	 * the editor on the newly created file.
	 */

	private void doFinish(String projectName, String teamNumber, String packageName, ProjectType projectType, String worldName, IProgressMonitor monitor) throws CoreException {
    	Properties props = WPILibCore.getDefault().getProjectProperties(null);
    	props.setProperty("team-number", teamNumber);
    	WPILibCore.getDefault().saveGlobalProperties(props);
		ProjectCreationUtils.createProject(new WPIRobotJavaProjectCreator(projectName, packageName, projectType, worldName));
	}

	/**
	 * We will accept the selection in the workbench to see if
	 * we can initialize from it.
	 * @see IWorkbenchWizard#init(IWorkbench, IStructuredSelection)
	 */
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		this.selection = selection;
	}
}