package edu.wpi.first.wpilibj.simulation.ds;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class RefreshAction implements ActionListener {
	private DS ds;

	public RefreshAction(DS ds) {
		this.ds = ds;
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		ds.scanForJoysticks();
	}

}
