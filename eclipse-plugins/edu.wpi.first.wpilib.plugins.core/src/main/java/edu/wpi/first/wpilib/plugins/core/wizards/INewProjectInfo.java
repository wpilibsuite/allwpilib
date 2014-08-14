package edu.wpi.first.wpilib.plugins.core.wizards;

import javax.swing.event.ChangeListener;

public interface INewProjectInfo {
	String getName();
	String getWorld();
	void registerChangeListener(ChangeListener changeListener);
	
	public static INewProjectInfo Null = new INewProjectInfo() {
		@Override
		public String getWorld() {
			return "";
		}
		
		@Override
		public String getName() {
			return "";
		}

		@Override
		public void registerChangeListener(ChangeListener changeListener) {}
	};
}
