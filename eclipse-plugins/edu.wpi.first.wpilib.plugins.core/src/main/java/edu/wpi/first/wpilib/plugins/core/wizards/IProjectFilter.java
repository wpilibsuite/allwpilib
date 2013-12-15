package edu.wpi.first.wpilib.plugins.core.wizards;

import org.eclipse.core.resources.IProject;

/**
 * A filter that can be used to select projects.
 * 
 * @author alex
 *
 */
public interface IProjectFilter {
	/**
	 * @param project The project to check.
	 * @return Whether or to accept the project.
	 */
	boolean accept(IProject project);
}
