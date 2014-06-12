package edu.wpi.first.wpilib.plugins.cpp.wizards.examples;

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
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;
import edu.wpi.first.wpilib.plugins.cpp.wizards.newproject.WPIRobotCPPProjectCreator;

public class ExampleCPPWizard extends ExampleWizard {
	private NewProjectMainPage detailsPage;

	/**
	 * Constructor for SampleNewWizard.
	 */
	public ExampleCPPWizard() {
		super();
		setNeedsProgressMonitor(true);
	}

	@Override
	protected void doFinish(IExampleProject ex, String teamNumber) throws CoreException {
		Properties props = WPILibCore.getDefault().getProjectProperties(null);
    	props.setProperty("team-number", teamNumber);
    	WPILibCore.getDefault().saveGlobalProperties(props);
    	
		final String projectName = detailsPage.getProjectName();
		ProjectCreationUtils.createProject(new WPIRobotCPPProjectCreator(projectName, ex, detailsPage.getWorld()));
	}

	@Override
	protected IWizardPage getDetailsPage() {
		if (detailsPage != null) return detailsPage;
		detailsPage = new NewProjectMainPage(selection, getTeamNumberPage());
		detailsPage.setTitle("Create Example Robot Java Project");
		detailsPage.setDescription("This wizard creates a new example project based on your selection.");
		detailsPage.setShowPackage(false);
		return detailsPage;
	}

	@Override
	public IExampleProject makeExampleProject(String name, String description,
			List<String> tags, List<String> folders, List<ExportFile> files) {
		return new ExampleCPPProject(name, description, tags, folders, files);
	}

	@Override
	public URL getResourceURL() {
		return WPILibCPPPlugin.getDefault().getBundle().getEntry("/resources/templates/examples");
	}

	@Override
	public String getXMLFile() {
		return "examples.xml";
	}
}
