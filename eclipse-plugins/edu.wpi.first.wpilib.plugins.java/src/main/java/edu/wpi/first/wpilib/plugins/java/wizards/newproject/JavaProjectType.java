package edu.wpi.first.wpilib.plugins.java.wizards.newproject;

import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import edu.wpi.first.wpilib.plugins.core.wizards.ProjectType;
import edu.wpi.first.wpilib.plugins.java.WPILibJavaPlugin;

public class JavaProjectType implements ProjectType {
	static ProjectType SIMPLE = new JavaProjectType() {
		@Override public Map<String, String> getFiles(String packageName) {
			Map<String, String> files = super.getFiles(packageName);
			files.put("src/"+packageName.replace(".", "/")+"/Robot.java", "simple/Robot.java");
			return files;
		}
	};
	static ProjectType ITERATIVE = new JavaProjectType() {
		@Override public Map<String, String> getFiles(String packageName) {
			Map<String, String> files = super.getFiles(packageName);
			files.put("src/"+packageName.replace(".", "/")+"/Robot.java", "iterative/Robot.java");
			return files;
		}
	};
	static ProjectType COMMAND_BASED = new JavaProjectType() {
		@Override public String[] getFolders(String packageName) {
			String[] paths = {"src/"+packageName.replace(".", "/"),
					"src/"+packageName.replace(".", "/")+"/commands",
					"src/"+packageName.replace(".", "/")+"/subsystems",
					"src/"+packageName.replace(".", "/")+"/triggers"};
			return paths;
		}
		@Override public Map<String, String> getFiles(String packageName) {
			Map<String, String> files = super.getFiles(packageName);
			files.put("src/"+packageName.replace(".", "/")+"/Robot.java", "command-based/Robot.java");
			files.put("src/"+packageName.replace(".", "/")+"/commands/ExampleCommand.java", "command-based/ExampleCommand.java");
			files.put("src/"+packageName.replace(".", "/")+"/subsystems/ExampleSubsystem.java", "command-based/ExampleSubsystem.java");
			return files;
		}
	};
	@SuppressWarnings("serial")
	static Map<String, ProjectType> TYPES = new HashMap<String, ProjectType>() {{
		put(ProjectType.SIMPLE, SIMPLE);
		put(ProjectType.ITERATIVE, ITERATIVE);
		put(ProjectType.COMMAND_BASED, COMMAND_BASED);
	}};

	@Override
	public String[] getFolders(String packageName) {
		String[] paths = {"src/"+packageName.replace(".", "/")};
		return paths;
	}

	@Override
	public Map<String, String> getFiles(String packageName) {
		HashMap<String, String> files = new HashMap<String, String>();
		files.put("build.xml", "build.xml");
		files.put("build.properties", "build.properties");
		files.put(".classpath", ".classpath");
		return files;
	}

	@Override
	public URL getBaseURL() {
		return WPILibJavaPlugin.getDefault().getBundle().getEntry("/resources/templates/");
	}
}
