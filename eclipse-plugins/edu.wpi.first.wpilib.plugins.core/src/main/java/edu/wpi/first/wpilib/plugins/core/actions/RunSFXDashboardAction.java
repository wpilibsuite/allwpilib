package edu.wpi.first.wpilib.plugins.core.actions;

import java.io.File;
import java.io.FilenameFilter;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;

import edu.wpi.first.wpilib.plugins.core.WPILibCore;

/**
 * Our sample action implements workbench action delegate.
 * The action proxy will be created by the workbench and
 * shown in the UI. When the user tries to use the action,
 * this delegate will be created and execution will be 
 * delegated to it.
 * @see IWorkbenchWindowActionDelegate
 */
public class RunSFXDashboardAction implements IWorkbenchWindowActionDelegate {
	/**
	 * The constructor.
	 */
	public RunSFXDashboardAction() {
	}

	/**
	 * The action has been activated. The argument of the
	 * method represents the 'real' action sitting
	 * in the workbench UI.
	 * @see IWorkbenchWindowActionDelegate#run
	 */
	public void run(IAction action) {
		File dir = new File(WPILibCore.getDefault().getWPILibBaseDir()+File.separator+"tools"+File.separator+"sfx-2014.11.01");
		File[] files = dir.listFiles(new FilenameFilter() {
			@Override public boolean accept(File dir, String name) {
				return name.startsWith("sfx") && name.endsWith(".jar");
			}
		});
		if (files == null || files.length < 1) return;
		String[] cmd = {"java", "-jar", files[0].getAbsolutePath()};
		try {
			DebugPlugin.exec(cmd, new File(System.getProperty("user.home")));
		} catch (CoreException e) {
            WPILibCore.logError("Error running SFX Dashboard.", e);
		}
	}

	/**
	 * Selection in the workbench has been changed. We 
	 * can change the state of the 'real' action here
	 * if we want, but this can only happen after 
	 * the delegate has been created.
	 * @see IWorkbenchWindowActionDelegate#selectionChanged
	 */
	public void selectionChanged(IAction action, ISelection selection) {
	}

	/**
	 * We can use this method to dispose of any system
	 * resources we previously allocated.
	 * @see IWorkbenchWindowActionDelegate#dispose
	 */
	public void dispose() {
	}

	/**
	 * We will cache window object in order to
	 * be able to provide parent shell for the message dialog.
	 * @see IWorkbenchWindowActionDelegate#init
	 */
	public void init(IWorkbenchWindow window) {
	}
}
