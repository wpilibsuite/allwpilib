package edu.wpi.first.wpilib.plugins.core.wizards;

import java.net.URL;
import java.util.List;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchWizard;

public abstract class ExampleWizard extends Wizard implements INewWizard {
	private ExampleWizardChoicePage page1;
	private IWizardPage page2;
	protected ISelection selection;
	private TeamNumberPage teamNumberPage;

	/**
	 * Constructor for SampleNewWizard.
	 */
	public ExampleWizard() {
		super();
		setNeedsProgressMonitor(true);
	}
	
	/**
	 * The worker method. It will find the container, create the
	 * file if missing or just replace its contents, and open
	 * the editor on the newly created file.
	 */
	protected abstract void doFinish(IExampleProject ex, String teamNumber) throws CoreException;

	protected abstract IWizardPage getDetailsPage();
	
	public abstract IExampleProject makeExampleProject(String name, String description,
			List<String> tags, List<String> folders, List<IExampleProject.ExportFile> files);
	
	public abstract URL getResourceURL();
	
	public abstract String getXMLFile();
	
	protected TeamNumberPage getTeamNumberPage() {
		return teamNumberPage;
	}
	
	/**
	 * Adding the page to the wizard.
	 */

	public void addPages() {
		if (TeamNumberPage.needsTeamNumberPage()) {
			teamNumberPage = new TeamNumberPage(selection);
			addPage(teamNumberPage);
		}
		page1 = new ExampleWizardChoicePage(this, selection);
		addPage(page1);
		page2 = getDetailsPage();
		addPage(page2);
	}

	/**
	 * This method is called when 'Finish' button is pressed in
	 * the wizard. We will create an operation and run it
	 * using wizard as execution context.
	 */
	public boolean performFinish() {
		try {
			doFinish(page1.getExampleProject(), TeamNumberPage.getTeamNumberFromPage(teamNumberPage));
		} catch (CoreException e) {
			e.printStackTrace();
			MessageDialog.openError(getShell(), "Error", e.getMessage());
			return false;
		}
		return true;
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
