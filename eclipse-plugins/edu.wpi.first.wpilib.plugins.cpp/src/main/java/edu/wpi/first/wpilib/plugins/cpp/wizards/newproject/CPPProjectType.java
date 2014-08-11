package edu.wpi.first.wpilib.plugins.cpp.wizards.newproject;

import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import edu.wpi.first.wpilib.plugins.core.wizards.ProjectType;
import edu.wpi.first.wpilib.plugins.cpp.WPILibCPPPlugin;

public class CPPProjectType implements ProjectType {
	static ProjectType ITERATIVE = new CPPProjectType() {
		@Override public Map<String, String> getFiles(String packageName) {
			Map<String, String> files = super.getFiles(packageName);
			files.put("src/Robot.cpp", "iterative/Robot.cpp");
			return files;
		}
	};
	static ProjectType COMMAND_BASED = new CPPProjectType() {
		@Override public String[] getFolders(String packageName) {
			String[] paths = {"src",
					"src/Commands",
					"src/Subsystems",
					"src/Triggers"};
			return paths;
		}
		@Override public Map<String, String> getFiles(String packageName) {
			Map<String, String> files = super.getFiles(packageName);
			files.put("src/Robot.cpp", "command-based/Robot.cpp");
			files.put("src/OI.h", "command-based/OI.h");
			files.put("src/OI.cpp", "command-based/OI.cpp");
			files.put("src/RobotMap.h", "command-based/RobotMap.h");
			files.put("src/CommandBase.cpp", "command-based/CommandBase.cpp");
			files.put("src/CommandBase.h", "command-based/CommandBase.h");
			files.put("src/Subsystems/ExampleSubsystem.h", "command-based/ExampleSubsystem.h");
			files.put("src/Subsystems/ExampleSubsystem.cpp", "command-based/ExampleSubsystem.cpp");
			files.put("src/Commands/ExampleCommand.h", "command-based/ExampleCommand.h");
			files.put("src/Commands/ExampleCommand.cpp", "command-based/ExampleCommand.cpp");
			return files;
		}
	};
	@SuppressWarnings("serial")
	static Map<String, ProjectType> TYPES = new HashMap<String, ProjectType>() {{
		put(ProjectType.ITERATIVE, ITERATIVE);
		put(ProjectType.COMMAND_BASED, COMMAND_BASED);
	}};

	@Override
	public String[] getFolders(String packageName) {
		String[] paths = {"src"};
		return paths;
	}

	@Override
	public Map<String, String> getFiles(String packageName) {
		HashMap<String, String> files = new HashMap<String, String>();
		files.put("build.xml", "build.xml");
		files.put("build.properties", "build.properties");
		files.put(".cproject", ".cproject");
		return files;
	}


	@Override
	public URL getBaseURL() {
		return WPILibCPPPlugin.getDefault().getBundle().getEntry("/resources/templates/");
	}
}
