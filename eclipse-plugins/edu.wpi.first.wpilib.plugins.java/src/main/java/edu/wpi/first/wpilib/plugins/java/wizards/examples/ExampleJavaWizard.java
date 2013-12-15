package edu.wpi.first.wpilib.plugins.java.wizards.examples;

import java.net.URL;
import java.util.List;
import java.util.Properties;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.wizard.IWizardPage;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;
import edu.wpi.first.wpilib.plugins.core.wizards.ExampleWizard;
import edu.wpi.first.wpilib.plugins.core.wizards.IExampleProject;
import edu.wpi.first.wpilib.plugins.core.wizards.IExampleProject.ExportFile;
import edu.wpi.first.wpilib.plugins.core.wizards.NewProjectMainPage;
import edu.wpi.first.wpilib.plugins.core.wizards.ProjectCreationUtils;
import edu.wpi.first.wpilib.plugins.java.WPILibJavaPlugin;
import edu.wpi.first.wpilib.plugins.java.wizards.newproject.WPIRobotJavaProjectCreator;

public class ExampleJavaWizard extends ExampleWizard {
	private NewProjectMainPage detailsPage;

	/**
	 * Constructor for SampleNewWizard.
	 */
	public ExampleJavaWizard() {
		super();
		setNeedsProgressMonitor(true);
	}

	@Override
	protected void doFinish(IExampleProject ex, String teamNumber) throws CoreException {
		Properties props = WPILibCore.getDefault().getProjectProperties(null);
    	props.setProperty("team-number", teamNumber);
    	WPILibCore.getDefault().saveGlobalProperties(props);
    	
		final String projectName = detailsPage.getProjectName();
		final String packageName = detailsPage.getPackage();
		ProjectCreationUtils.createProject(new WPIRobotJavaProjectCreator(projectName, packageName, ex));
	}

	@Override
	protected IWizardPage getDetailsPage() {
		if (detailsPage != null) return detailsPage;
		detailsPage = new NewProjectMainPage(selection, getTeamNumberPage());
		detailsPage.setTitle("Create Example Robot Java Project");
		detailsPage.setDescription("This wizard creates a new example project based on your selection.");
		return detailsPage;
	}

	@Override
	public IExampleProject makeExampleProject(String name, String description,
			List<String> tags, List<String> folders, List<ExportFile> files) {
		return new ExampleJavaProject(name, description, tags, folders, files);
	}

	@Override
	public URL getResourceURL() {
		return WPILibJavaPlugin.getDefault().getBundle().getEntry("/resources/templates/examples");
	}

	@Override
	public String getXMLFile() {
		return "examples.xml";
	}
}
