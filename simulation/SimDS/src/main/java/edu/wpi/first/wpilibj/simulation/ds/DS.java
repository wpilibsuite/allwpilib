/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation.ds;

import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionListener;
import java.util.List;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.DropMode;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import org.gazebosim.transport.Node;
import org.gazebosim.transport.Publisher;

import edu.wpi.first.wpilibj.simulation.ds.FakeJoystick;
import edu.wpi.first.wpilibj.simulation.ds.ISimJoystick;
import edu.wpi.first.wpilibj.simulation.ds.JoystickProvider;
import gazebo.msgs.GzDriverStation.DriverStation;

public class DS {
	private JoystickProvider joystickProvider;
	private JoystickList joysticks;
	
	private JFrame mainframe;
	private JPanel modePanel;
	private ActionListener modeListener;
	private ButtonGroup modes;
	private JButton enable, refresh;
	
	public enum State {
		Disabled, Teleop, Autonomous, Test;
	}
	private boolean enabled = false;
	private State state = State.Teleop;
	private DriverStation.State protoState = DriverStation.State.TELEOP;
	private Publisher<DriverStation> pub;
	
	public DS(JoystickProvider joystickProvider) {
		this.joystickProvider = joystickProvider;
		mainframe = new JFrame();
		mainframe.setTitle("FRC Simulation DriverStation");
		mainframe.setLayout(new GridBagLayout());
		GridBagConstraints constraints = new GridBagConstraints();
		
		makeModeButtons(constraints);
        mainframe.pack();
		constraints.gridy = 1;
		makeEnableButton(constraints);

		constraints.gridx = 1;
		constraints.gridy = 0;
		makeJoystickUI(constraints);
        mainframe.pack();
		constraints.gridy = 1;
		makeRefreshButton(constraints);
        
        mainframe.pack();
        mainframe.setVisible(true);
	}
	
	private void makeModeButtons(GridBagConstraints constraints) {
		modePanel = new JPanel();
		modePanel.setLayout(new BoxLayout(modePanel, BoxLayout.PAGE_AXIS));
		
		modeListener = new ModeAction(this);
		JRadioButton teleop = new JRadioButton("Teleop");
		teleop.setActionCommand(State.Teleop.toString());
		teleop.addActionListener(modeListener);
		JRadioButton auto = new JRadioButton("Autonomous");
		auto.setActionCommand(State.Autonomous.toString());
		auto.addActionListener(modeListener);
		JRadioButton test = new JRadioButton("Test");
		test.setActionCommand(State.Test.toString());
		test.addActionListener(modeListener);
		teleop.setSelected(true);
		
		modes = new ButtonGroup();
		modes.add(teleop);
		modes.add(auto);
		modes.add(test);
		modePanel.add(teleop);
		modePanel.add(auto);
		modePanel.add(test);
		mainframe.add(modePanel, constraints);
	}
		
	private void makeEnableButton(GridBagConstraints constraints) {
		enable = new JButton("Enable");
		enable.addActionListener(new EnableAction(this));
		enable.setPreferredSize(new Dimension(modePanel.getSize().width, 50));
		mainframe.add(enable, constraints);
	}
	
	private void makeJoystickUI(GridBagConstraints constraints) {
		joysticks = new JoystickList(joystickProvider);
		mainframe.add(joysticks, constraints);
		scanForJoysticks();
	}

	public void scanForJoysticks() {
		joysticks.removeAll();
		List<ISimJoystick> sticks = joystickProvider.scanForJoysticks();
		while (sticks.size() < 6) {
			sticks.add(new FakeJoystick());
		}
		joysticks.setListData(sticks);
	}
	
	private void makeRefreshButton(GridBagConstraints constraints) {
		refresh = new JButton("Refresh Joysticks");
		refresh.addActionListener(new RefreshAction(this));
		refresh.setPreferredSize(new Dimension(joysticks.getSize().width, 50));
		mainframe.add(refresh, constraints);
	}
	
	public void setEnabled(boolean enabled) {
		this.enabled = enabled;
		enable.setText(enabled ? "Disable" : "Enable");
	}
	
	public State getState() {
		return enabled ? state : State.Disabled;
	}

	public void setState(State state) {
		setEnabled(false);
		this.state = state;
		switch (state) {
		case Autonomous: protoState = DriverStation.State.AUTO; break;
		case Teleop: protoState = DriverStation.State.TELEOP; break;
		case Test: protoState = DriverStation.State.TEST; break;
		default: break;
		}
	}

	public void toggleEnable() {
		setEnabled(!enabled);
	}

	public void advertise(Node node) {
		pub = node.advertise("ds/state", DriverStation.getDefaultInstance());
	}

	public void publish() {
		pub.publish(DriverStation.newBuilder().setEnabled(enabled).setState(protoState).build());
	}
}
