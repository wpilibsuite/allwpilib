package edu.wpi.first.wpilib.plugins.core.wizards;

import java.util.List;

public interface IExampleProject extends ProjectType {
	public String getName();
	public String getContent();
	public List<String> getTags();
	
	static class ExportFile {
		public String source, destination;
		public ExportFile(String source, String destination) {
			this.source = source;
			this.destination = destination;
		}
	}
}
