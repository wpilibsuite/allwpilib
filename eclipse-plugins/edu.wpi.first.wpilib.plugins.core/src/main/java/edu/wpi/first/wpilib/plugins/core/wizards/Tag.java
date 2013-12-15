package edu.wpi.first.wpilib.plugins.core.wizards;


public class Tag {
	private String name, description;
	
	public Tag(String name, String description) {
		this.name = name;
		this.description = description;
	}

	public String getName() {
		return name;
	}
	
	public String getContent() {
		return "<h1>"+name+"</h1><p>"+description+"</p>";
	}
}
