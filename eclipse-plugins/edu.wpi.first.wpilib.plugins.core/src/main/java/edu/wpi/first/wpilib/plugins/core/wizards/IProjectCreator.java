package edu.wpi.first.wpilib.plugins.core.wizards;

import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;

/**
 * An interface for providing hooks into the project creation 
 * process. It provides necessary information and can perform
 * both initialization and finalization of the project
 * creation process.
 * 
 * @author Alex Henning
 */
public interface IProjectCreator {
	
	/**
	 * @return The name of the project to create.
	 */
	String getName();

	/**
	 * @return The main package to be created. Should be an
	 *         empty string if irrelevant.
	 */
	String getPackageName();
	
	/**
	 * @return Map of keys to be substituted with values
	 *         during the creation process.
	 */
	Map<String, String> getValues();

	/**
	 * @return The natures that this project should have.
	 */
	List<String> getNatures();

	/**
	 * @return A project type object that provides files
	 *         and directories to create.
	 */
	ProjectType getProjectType();

	/**
	 * Run extra initialization code on the project.
	 * @param project The project being created
	 * @throws CoreException
	 */
	void initialize(IProject project) throws CoreException;

	/**
	 * Run extra finalization code on the project.
	 * @param project The project being created
	 * @throws CoreException
	 */
	void finalize(IProject project) throws CoreException;
}
