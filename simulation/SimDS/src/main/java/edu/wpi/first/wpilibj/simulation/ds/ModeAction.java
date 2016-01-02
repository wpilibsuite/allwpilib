/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
