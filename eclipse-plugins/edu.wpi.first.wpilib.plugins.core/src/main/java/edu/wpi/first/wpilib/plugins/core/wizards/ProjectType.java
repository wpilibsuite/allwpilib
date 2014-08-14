package edu.wpi.first.wpilib.plugins.core.wizards;

import java.net.URL;
import java.util.Map;

public interface ProjectType {
	String SAMPLE = "SAMPLE";
	String ITERATIVE = "ITERATIVE";
	String COMMAND_BASED = "COMMAND_BASED";
	
	String[] getFolders(String packageName);
	Map<String, String> getFiles(String packageName);
	URL getBaseURL();
}