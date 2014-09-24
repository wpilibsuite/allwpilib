package edu.wpi.first.wpilib.plugins.cpp.wizards.newproject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.cdt.build.core.scannerconfig.ScannerConfigNature;
import org.eclipse.cdt.core.CCProjectNature;
import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.index.IIndexManager;
import org.eclipse.cdt.core.model.ICElement;
import org.eclipse.cdt.managedbuilder.core.ManagedCProjectNature;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;

import edu.wpi.first.wpilib.plugins.core.nature.FRCProjectNature;
import edu.wpi.first.wpilib.plugins.core.wizards.IProjectCreator;
import edu.wpi.first.wpilib.plugins.core.wizards.ProjectType;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;

public class WPIRobotCPPProjectCreator implements IProjectCreator {
	String projectName;
	ProjectType projectType;
	private String worldName;
	
	public WPIRobotCPPProjectCreator(String projectName, ProjectType projectType, String worldName) {
		this.projectName = projectName;
		this.projectType = projectType;
		this.worldName = worldName;
	}

	@Override
	public String getName() {
		return projectName;
	}

	@Override
	public String getPackageName() {
		return ""; // C++ doesn't have the equivalent of a project name
	}

	@Override
	public Map<String, String> getValues() {
		Map<String, String> vals = new HashMap<String, String>();
		vals.put("$project", projectName);
		vals.put("$cpp-location", WPILibCPPPlugin.getDefault().getCPPDir());
		vals.put("$world", worldName);
		return vals;
	}

	@Override
	public List<String> getNatures() {
		List<String> natures = new ArrayList<>();
		natures.add(CCProjectNature.C_NATURE_ID);
		natures.add(CCProjectNature.CC_NATURE_ID);
		natures.add(ManagedCProjectNature.MNG_NATURE_ID);
		natures.add(ScannerConfigNature.NATURE_ID);
		natures.add(FRCProjectNature.FRC_PROJECT_NATURE);
		return natures;
	}

	@Override
	public ProjectType getProjectType() {
		return projectType;
	}

	@Override
	public void initialize(IProject project) {
		try {
			CCorePlugin.getDefault().createCDTProject(project.getDescription(), project, null);
			project.open(null);
		} catch (CoreException e) {
            WPILibCPPPlugin.logError("Error intializing FRC C++ project.", e);
		}
	}

	@Override
	public void finalize(IProject project) throws CoreException {
		updateVariables(project);
		
		// Freshen the index because otherwise it's red and angry
		ICElement[] projects = {CCorePlugin.getDefault().getCoreModel().create(project)};
		CCorePlugin.getIndexManager().update(projects, IIndexManager.UPDATE_ALL | IIndexManager.UPDATE_EXTERNAL_FILES_FOR_PROJECT);
	}
	
	private void updateVariables(IProject project) throws CoreException {
		// TODO: implement C++ equivalent
	}
}
