package edu.wpi.first.wpilibj.simulation.ds;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class EnableAction implements ActionListener {
	private DS ds;
	
	public EnableAction(DS ds) {
		this.ds = ds;
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		ds.toggleEnable();
	}

}
