/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.driverstation.robotcoms;

import edu.wpi.first.driverstation.DriverStation;
import edu.wpi.first.driverstation.fms.FMSInterface;
import edu.wpi.first.driverstation.fms.NoFMSController;

import java.net.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.zip.CRC32;

/**
 *
 * @author Brendan
 */
public class RobotComm extends Thread {

	private static final Logger log = Logger.getLogger(DriverStation.class
			.getName());

	private class RobotDataIn extends Thread {

		RobotComm theRobotComm;
		private DatagramSocket robotReceiveSocket;
		private int robotReceivePort = 1150;

		public RobotDataIn(RobotComm robotComm) {
			theRobotComm = robotComm;
			try {
				robotReceiveSocket = new DatagramSocket(robotReceivePort);
			} catch (SocketException ex) {
				log.log(Level.WARNING, "Error creating Robot Sockets", ex);
			}
		}

		public void run() {
			while (true) {
				try {
					byte[] receiveData = new byte[1024];// the size of the data
					DatagramPacket receivePacket = new DatagramPacket(
							receiveData, receiveData.length);
					robotReceiveSocket.receive(receivePacket);
					theRobotComm.receiveRobotpacket(receiveData);
					Thread.yield();
				} catch (Exception ex) {
				}
			}
		}
	}

	private DatagramSocket robotSendingSocket;
	private RobotDataIn inputSocket;
	private final int robotSendPortNormal = 1110;
	// private final int robotSendPortNormal = 1115;//actually for sending on
	// the field (used for robot sim)
	private final int robotSendPortFMS = 1115;
	private int sentPackets;// the current packet ID that is being sent to the
							// robot
	private int lastRobotPacket;// the last packet ID to have been recieved by
								// the robot
	private long lastRobotTime;// the time stamp when the robot was seen
	private boolean[] controlDataToSend;// {auto, test mode, enabled,
										// not-estopped, reboot}
	private boolean[] controlFromRobot;// {auto, test mode, enabled,
										// not-estopped}
	private byte[] stationID = new byte[2];
	private char[] batteryData = new char[2];
	private int robotTeam = 0;
	private long lastSentTime = 0;
	private int packetOffset = 0;
	private FMSInterface fmsController;

	public RobotComm(int team) {

		robotTeam = team;
		try {
			robotSendingSocket = new DatagramSocket();// robotSendPortNormal);
		} catch (SocketException ex) {
			log.log(Level.WARNING, "Error creating Robot Sockets", ex);
		}
		sentPackets = 0;
		controlFromRobot = new boolean[] { false, false, false, true };// {auto,
																		// test
																		// mode,
																		// enabled,
																		// not-estopped}

		controlDataToSend = new boolean[] { false, false, false, true, false };// {auto,
																				// test
																				// mode,
																				// enabled,
																				// not-estopped,
																				// reboot}
		fmsController = new NoFMSController();
		inputSocket = new RobotDataIn(this);
		inputSocket.setDaemon(true);
	}

	@Override
	public void start() {
		inputSocket.start();
		super.start();
	}

	@Override
	public void run() {
		while (true) {
			if ((lastSentTime + 20) < System.currentTimeMillis()) {
				lastSentTime = System.currentTimeMillis();
				sendRobotData();
			}

		}
	}

	public void rebootRobot() {
		setEnabled(false);
		controlDataToSend[4] = true;
	}

	public void setControlMode(ControlMode theMode) {
		if (theMode == ControlMode.AUTO) {// {auto, test mode, enabled,
											// not-estopped}
			controlDataToSend[0] = true;
			controlDataToSend[1] = false;
		} else if (theMode == ControlMode.TELEOP) {// {auto, test mode, enabled,
													// not-estopped}
			controlDataToSend[0] = false;
			controlDataToSend[1] = false;
		} else if (theMode == ControlMode.TEST_MODE) {// {auto, test mode,
														// enabled,
														// not-estopped}
			controlDataToSend[0] = false;
			controlDataToSend[1] = true;
		}
	}

	public void setEnabled(boolean enabled) {// {auto, test mode, enabled,
												// not-estopped, reboot}
		controlDataToSend[2] = enabled;
	}

	public void eStopRobot() {

		controlDataToSend[3] = false;
	}

	public void setAllianceInfo(byte allianceColor, byte allianceStation) {
		stationID = new byte[] { allianceColor, allianceStation };
	}

	public void setTeamNumber(int newTeam) {
		robotTeam = newTeam;
	}

	public void setFMSController(FMSInterface newFMS) {
		fmsController = newFMS;
	}

	/**
	 * Packs up all the data and sends the packet to the robot
	 */
	private void sendRobotData() {
		sentPackets++;
		sentPackets = sentPackets % 0xFFFF;// makes it overflow properly
		packetOffset = sentPackets - lastRobotPacket;
		CRC32 crctest = new CRC32();
		byte[] sendData = new byte[1024];

		for (int i = 0; i < 1024; i++) {
			sendData[i] = 0x00;
		}

		// Construct the pacet counts
		sendData[0] = (byte) ((sentPackets >> 8) & 0xFF);
		sendData[1] = (byte) (sentPackets & 0xFF);
		// {auto, test mode, enabled, not-estopped, reboot}
		if (controlDataToSend[4]) {// reboot bit
			sendData[2] += 0x80;

			controlDataToSend[4] = false;
		}
		if (controlDataToSend[3]) {// not e-stop bit
			sendData[2] += 0x40;
		} else {
			controlDataToSend[3] = true;
		}
		if (controlDataToSend[2])// if enabled
		{
			sendData[2] += 0x20;
		}
		if (controlDataToSend[0])// 1 holds the mode (1=auto 0=teleop)
		{
			sendData[2] += 0x10;
		}
		if (fmsController.isFMSConnected()) {
			sendData[2] += 0x08;
		}

		if (!isConnected()) {
			sendData[2] += 0x04;
		}

		sendData[3] = (byte) 0xFF;// make all digial inputs high

		sendData[4] = (byte) ((robotTeam >> 8) & 0xFF);// packs up and sends
														// team number
		sendData[5] = (byte) ((robotTeam) & 0xFF);

		sendData[6] = stationID[0];// the alliance color
		sendData[7] = stationID[1]; // the station number

		sendData[72] = (byte) 0x31;
		sendData[73] = (byte) 0x31;
		sendData[74] = (byte) 0x33;
		sendData[75] = (byte) 0x30;
		sendData[76] = (byte) 0x21;
		sendData[77] = (byte) 0x21;
		sendData[78] = (byte) 0x30;
		sendData[79] = (byte) 0x30;

		crctest.reset();
		crctest.update(sendData);
		long crccheck = crctest.getValue();
		sendData[1020] = (byte) ((crccheck >> 24) & 0xff);
		sendData[1021] = (byte) ((crccheck >> 16) & 0xff);
		sendData[1022] = (byte) ((crccheck >> 8) & 0xff);
		sendData[1023] = (byte) ((crccheck >> 0) & 0xff);

		byte teamHigh = (byte) ((robotTeam - robotTeam % 100) / 100);
		byte teamLow = (byte) (robotTeam % 100);
		/*
		 * send packet
		 */
		try {
			DatagramPacket sendPacket;
			if (!fmsController.isFMSConnected()) {
				sendPacket = new DatagramPacket(sendData, sendData.length,
						InetAddress.getByAddress(new byte[] { 10, teamHigh,
								teamLow, 2 }), robotSendPortNormal);
			} else {
				sendPacket = new DatagramPacket(sendData, sendData.length,
						InetAddress.getByAddress(new byte[] { 10, teamHigh,
								teamLow, 2 }), robotSendPortFMS);
			}
			robotSendingSocket.send(sendPacket);
		} catch (Exception e) {
			log.log(Level.WARNING, "Failed to send", e);
		}

		fmsController.setRobotConnected(isConnected());

	}

	public boolean isConnected() {

		if (!(lastRobotTime + 100 > System.currentTimeMillis())) {
			return (false);
		}

		if (packetOffset > 25) {
			return (false);
		}

		return (true);
	}

	public char[] getBatteryData() {
		return batteryData;
	}

	/**
	 * Return the control data that it has gathered from the robot in the
	 * format: {auto, test mode, enabled, not-estopped}
	 *
	 * @return The data from the robot
	 */
	public boolean[] getControlDataFromRobot() {
		return controlFromRobot;
	}

	public double getBatteryVoltage() {
		return Double.parseDouble(getBatteryString());
	}

	public String getBatteryString() {
		return (String.format("%x", getBatteryData()[0]) + "." + String.format(
				"%x", getBatteryData()[1]));
	}

	protected void receiveRobotpacket(byte[] dataIn) {// this gets called
														// whenever a new robot
														// packet is recieved
		int teamNumber = 0;
		teamNumber += dataIn[8] & 0xFF;
		teamNumber = ((teamNumber << 8) + (dataIn[9] & 0xFF)) & 0xFFFF;
		if (robotTeam == teamNumber) {
			lastRobotTime = System.currentTimeMillis();
			batteryData[0] = (char) dataIn[1];
			batteryData[1] = (char) dataIn[2];
			lastRobotPacket = 0;
			lastRobotPacket += dataIn[30] & 0xFF;
			lastRobotPacket = ((lastRobotPacket << 8) + (dataIn[31] & 0xFF)) & 0xFFFF;
			controlFromRobot[0] = ((0x10 & dataIn[0]) > 0);// auto
			controlFromRobot[2] = ((0x20 & dataIn[0]) > 0);// enabled
			controlFromRobot[3] = true;// robot isn't estopped yet

		} else {
			log.log(Level.INFO, "Data from different team...");
		}
		fmsController.updateRobotFeedback(controlFromRobot);
		fmsController.updateBattery(batteryData);

	}
}
