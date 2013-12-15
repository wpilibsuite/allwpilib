package edu.wpi.first.wpilib.plugins.java.wizards.examples;

import java.util.List;
import java.util.Map;

import edu.wpi.first.wpilib.plugins.core.wizards.IExampleProject;
import edu.wpi.first.wpilib.plugins.java.wizards.newproject.JavaProjectType;

public class ExampleJavaProject extends JavaProjectType implements IExampleProject {
	private String name, description;
	private List<String> tags;
	private List<String> packages;
	private List<ExportFile> files;
	
	public ExampleJavaProject(String name, String description, List<String> tags, 
			List<String> packages, List<ExportFile> files) {
		this.name = name;
		this.description = description;
		this.tags = tags;
		this.packages = packages;
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
		String packageDir = packageName.replace(".", "/");
		for (int i = 0; i < packages.size(); i++) {
			packages.set(i, packages.get(i).replaceAll("\\$package-dir", packageDir));
		}
		return packages.toArray(new String[0]);
	}

	@Override
	public Map<String, String> getFiles(String packageName) {
		String packageDir = packageName.replace(".", "/");
		Map<String, String> files = super.getFiles(packageName);
		for (ExportFile file : this.files) {
			files.put(file.destination.replaceAll("\\$package-dir", packageDir), file.source);
		}
		return files;
	}
}
