package edu.wpi.first.wpilibj.simulation.ds;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class ModeAction implements ActionListener {
	private DS ds;
	
	public ModeAction(DS ds) {
		this.ds = ds;
	}
	
	@Override
	public void actionPerformed(ActionEvent e) {
		ds.setState(DS.State.valueOf(e.getActionCommand()));
	}

}
