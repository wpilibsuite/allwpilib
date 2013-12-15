package edu.wpi.first.wpilib.plugins.core.wizards;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Map;
import java.util.Map.Entry;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

/**
 * Utilities for creating a new project and files from templates. Uses
 * IProjectCreator to provide hooks for generating the directory
 * structure, initial files, initializing and finalizing the creation
 * of the new project.
 * 
 * @author Alex Henning
 **/
public class ProjectCreationUtils {

	/**
	 * Create a project using the given IProjectCreator.
	 * 
	 * @param creator The creator that provides the necessary information
	 *                to create the project.
	 * @return The newly created project.
	 */
	public static IProject createProject(IProjectCreator creator) {
		IProject project = createBaseProject(creator.getName(), null);
		
		try {
			creator.initialize(project);
			for (String nature : creator.getNatures()) {
				addNature(project, nature);	
			}
			addToProjectStructure(project, creator);
			addFilesToProject(project, creator);
			creator.finalize(project);
		} catch (CoreException e) {
            e.printStackTrace();
            project = null;
        }
		
        return project;
	}

	private static IProject createBaseProject(String projectName, IPath location) {
		IProject newProject = ResourcesPlugin.getWorkspace().getRoot().getProject(projectName);
		
		if (!newProject.exists()) {
			IPath projectLocation = location;
			IProjectDescription desc = newProject.getWorkspace().newProjectDescription(newProject.getName());
			if (location != null &&
					ResourcesPlugin.getWorkspace().getRoot().getLocation().equals(location)) {
				projectLocation = null;
			}
			desc.setLocation(projectLocation);
			try {
				newProject.create(desc, null);
				if (!newProject.isOpen()) {
					newProject.open(null);
				}
			} catch (CoreException e) {
	            e.printStackTrace();
			}
		}
		
		return newProject;
	}

	private static void addNature(IProject project, String nature_id) throws CoreException {
		if (!project.hasNature(nature_id)) {
			IProjectDescription desc = project.getDescription();
			String[] prevNatures = desc.getNatureIds();
			String[] newNatures = new String[prevNatures.length + 1];
			System.arraycopy(prevNatures, 0, newNatures, 0, prevNatures.length);
			newNatures[prevNatures.length] = nature_id;
			desc.setNatureIds(newNatures);
			
			project.setDescription(desc, null);
		}
	}

	private static void addToProjectStructure(IProject project, IProjectCreator creator) throws CoreException {
		String[] paths = creator.getProjectType().getFolders(creator.getPackageName());
		for (String path : paths) {
			IFolder etcFolders = project.getFolder(path);
			createFolder(etcFolders);
		}
	}

	private static void createFolder(IFolder folder) throws CoreException {
		IContainer parent = folder.getParent();
		if (parent instanceof IFolder) {
			createFolder((IFolder) parent);
		}
		if (!folder.exists()) {
			folder.create(false, true, null);
		}
		folder.refreshLocal(IResource.DEPTH_INFINITE, null);
	}

	private static void addFilesToProject(IProject project, IProjectCreator creator) throws CoreException {
		Map<String, String> files = creator.getProjectType().getFiles(creator.getPackageName());
		for (Entry<String, String> e : files.entrySet()) {
			try {
				URL url = new URL(creator.getProjectType().getBaseURL(), e.getValue());
				createTemplateFile(project, e.getKey(), url, creator.getValues());
			} catch (MalformedURLException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
	}
	
	/**
	 * Create a file in the project from a template. Substituting as required.
	 * 
	 * @param project The project to use create the file in.
	 * @param filepath The path of the created file.
	 * @param filesource The source of the template to use.
	 * @param vals The map of values to use for substitution.
	 * @throws CoreException
	 */
	public static void createTemplateFile(IProject project, String filepath, URL url, Map<String, String> vals) throws CoreException {
		IFile template = project.getFile(new Path(filepath));
		if (!template.exists()) {
			InputStream in = openTemplateContentStream(project, url, vals);
			template.create(in, true, null);
		}
	}

	private static InputStream openTemplateContentStream(IProject project, URL url, Map<String, String> vals) {
		//http://eclipse-javacc.cvs.sourceforge.net/viewvc/eclipse-javacc/sf.eclipse.javacc/src-plugin/sf/eclipse/javacc/wizards/JJNewWizard.java?view=markup
		//eclipse plugin distributing template files
		try {
			return makeTemplateInputStream(url.openStream(), vals);
		} catch (final MalformedURLException e) {
			e.printStackTrace();
		} catch (final IOException e) {
			e.printStackTrace();
		}
		return null;
	}
	
	private static InputStream makeTemplateInputStream(InputStream stream, Map<String, String> vals) {
		String str;
		try {
			str = readInput(stream);
			stream.close();
		} catch (final IOException e) {
			e.printStackTrace();
			return null;
		}
		
		// Instantiate template
		for (Entry<String, String> e : vals.entrySet())
			str = str.replace(e.getKey(), e.getValue());

		return new ByteArrayInputStream(str.getBytes());
	}
	
	private static String readInput(InputStream stream) {
		StringBuffer buffer = new StringBuffer();
		try {
			InputStreamReader isr = new InputStreamReader(stream);
			Reader in = new BufferedReader(isr);
			int ch;
			while ((ch = in.read()) > -1) {
				buffer.append((char)ch);
			}
			System.out.print("\n");
			in.close();
			return buffer.toString();
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
	}

}
