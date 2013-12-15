package edu.wpi.first.wpilib.plugins.cpp.wizards.examples;

import java.util.List;
import java.util.Map;

import edu.wpi.first.wpilib.plugins.core.wizards.IExampleProject;
import edu.wpi.first.wpilib.plugins.cpp.wizards.newproject.CPPProjectType;

public class ExampleCPPProject extends CPPProjectType implements IExampleProject {
	private String name, description;
	private List<String> tags;
	private List<String> directories;
	private List<ExportFile> files;
	
	public ExampleCPPProject(String name, String description, List<String> tags, 
			List<String> directories, List<ExportFile> files) {
		this.name = name;
		this.description = description;
		this.tags = tags;
		this.directories = directories;
		this.files = files;
	}

	public String getName() {
		return name;
	}
	
	public String getContent() {
		return "<h1>"+name+"</h1><p>"+description+"</p>";
	}

	public List<String> getTags() {
		return tags;
	}

	@Override
	public String[] getFolders(String packageName) {
		return directories.toArray(new String[0]);
	}

	@Override
	public Map<String, String> getFiles(String packageName) {
		Map<String, String> files = super.getFiles(packageName);
		for (ExportFile file : this.files) {
			files.put(file.destination, file.source);
		}
		return files;
	}
}
