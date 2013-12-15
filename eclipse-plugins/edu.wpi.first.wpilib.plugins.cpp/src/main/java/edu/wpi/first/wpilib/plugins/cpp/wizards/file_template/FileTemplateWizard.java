package edu.wpi.first.wpilib.plugins.cpp.wizards.file_template;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.cdt.core.model.ICContainer;
import org.eclipse.cdt.core.model.ISourceRoot;
import org.eclipse.cdt.core.model.ITranslationUnit;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchWizard;

import edu.wpi.first.wpilib.plugins.core.wizards.ProjectCreationUtils;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;

public class FileTemplateWizard extends Wizard implements INewWizard {
	private String type, source, ending;
	private FileTemplateWizardMainPage page;
	private ISelection selection;
	private IProject project;

	/**
	 * Constructor for SampleNewWizard.
	 */
	public FileTemplateWizard(String type, String source, String ending) {
		super();
		setNeedsProgressMonitor(true);
		this.type = type;
		this.source = source;
		this.ending = ending;
	}
	
	/**
	 * Adding the page to the wizard.
	 */

	public void addPages() {
		page = new FileTemplateWizardMainPage(type, project, ending, selection);
		addPage(page);
	}

	/**
	 * This method is called when 'Finish' button is pressed in
	 * the wizard. We will create an operation and run it
	 * using wizard as execution context.
	 */
	public boolean performFinish() {
		final IProject project = page.getProject();
		final String className = page.getClassName();
		final String folderName = page.getFolder();
		System.out.println("Class: "+className+" Folder: "+folderName);
		IRunnableWithProgress op = new IRunnableWithProgress() {
			public void run(IProgressMonitor monitor) throws InvocationTargetException {
				try {
					doFinish(project, className, folderName, monitor);
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
	private void doFinish(IProject project, String className, String folderName, IProgressMonitor monitor) throws CoreException {
		Map<String, String> map = new HashMap<String, String>();
		map.put("$classname", className);
		map.put("$folder", folderName);
		String filepath = folderName+File.separator+className;
		try {
			// Create Header
			URL url = new URL(WPILibCPPPlugin.getDefault().getBundle().getEntry("/resources/templates/"), source+".h");
			ProjectCreationUtils.createTemplateFile(project, filepath+".h", url, map);
			
			// Create CPP file
			url = new URL(WPILibCPPPlugin.getDefault().getBundle().getEntry("/resources/templates/"), source+".cpp");
			ProjectCreationUtils.createTemplateFile(project, filepath+".cpp", url, map);
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/**
	 * We will accept the selection in the workbench to see if
	 * we can initialize from it.
	 * @see IWorkbenchWizard#init(IWorkbench, IStructuredSelection)
	 */
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		this.selection = selection;
		System.out.println(selection);
		Object element = ((StructuredSelection) selection).getFirstElement();
		System.out.println(element.getClass());
		if (element instanceof IResource) {
			project = ((IResource) element).getProject();
		} else if (element instanceof ISourceRoot) {
			project = ((ISourceRoot) element).getCProject().getProject();
		} else if (element instanceof ITranslationUnit) {
			project = ((ITranslationUnit) element).getCProject().getProject();
		} else if (element instanceof ISourceRoot) {
			project = ((ISourceRoot) element).getCProject().getProject();
		} else if (element instanceof ICContainer) {
			project = ((ICContainer) element).getCProject().getProject();
		} else System.out.println("Element not instance of IResource: "+element.getClass());
	}
}
