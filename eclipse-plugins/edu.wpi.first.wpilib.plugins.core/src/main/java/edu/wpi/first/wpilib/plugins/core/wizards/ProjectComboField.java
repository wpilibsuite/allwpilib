package edu.wpi.first.wpilib.plugins.core.wizards;

import java.util.Arrays;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;

public class ProjectComboField {
	public static String NO_PROJECT = "No Project";
	
	private Combo combo;
	
	public ProjectComboField(Composite composite, int config, IProjectFilter filter) {
		combo = new Combo(composite, config);
		
		combo.add(NO_PROJECT);
		combo.select(0);
		IProject[] projects = ResourcesPlugin.getWorkspace().getRoot().getProjects();
		for (IProject project : projects) {
			if (filter == null || filter.accept(project))
				combo.add(project.getName());
		}
	}

	public void setProject(IProject project) {
		if (project != null)
			combo.select(Arrays.binarySearch(combo.getItems(), project.getName()));
		else combo.select(0);
	}

	public IProject getProject() {
		return ResourcesPlugin.getWorkspace().getRoot().getProject(combo.getItem(combo.getSelectionIndex()));
	}

	public boolean isValid() {
		return combo.getSelectionIndex() != 0;
	}

	public void setLayoutData(GridData gd) {
		combo.setLayoutData(gd);
	}

	public void addModifyListener(ModifyListener modifyListener) {
		combo.addModifyListener(modifyListener);
	}
	
}
