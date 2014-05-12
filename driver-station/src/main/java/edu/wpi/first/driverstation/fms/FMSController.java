package edu.wpi.first.driverstation.fms;

import edu.wpi.first.driverstation.DriverStation;
import edu.wpi.first.driverstation.fms.FMSInterface;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.zip.CRC32;

import edu.wpi.first.driverstation.robotcoms.ControlMode;

/**
 *
 * @author Brendan
 */
public class FMSController extends Thread implements FMSInterface {

	private static final Logger log = Logger.getLogger(DriverStation.class
			.getName());

	private class FMSDataIn extends Thread {

		private FMSController fmsController;
		private int FMSreceivePort = 1120;
		private DatagramSocket FMSreceiveSocket;

		public FMSDataIn(FMSController fmsController) {
			this.fmsController = fmsController;
			try {
				FMSreceiveSocket = new DatagramSocket(FMSreceivePort);
			} catch (SocketException ex) {
				log.log(Level.WARNING, "Error creating Robot Sockets", ex);
			}
		}

		@Override
		public void run() {
			while (true) {
				try {
					byte[] receiveData = new byte[74];// the size of the data
					DatagramPacket receivePacket = new DatagramPacket(
							receiveData, receiveData.length);
					FMSreceiveSocket.receive(receivePacket);
					fmsController
							.setIP(receivePacket.getAddress().getAddress());
					fmsController.receiveFMSpacket(receiveData);
				} catch (Exception ex) {
					log.log(Level.INFO, "Receive error", ex);
				}
			}
		}
	}

	private FMSDataIn dataInput;
	private DatagramSocket FMSsendingSocket;
	private int teamNumber;
	private int FMSsendPort = 1160;
	private byte[] ipFMS = new byte[4];
	private boolean[] controlState = new boolean[8];
	// {auto, test, mode, enabled, not-estopped}
	private boolean[] fromRobot = new boolean[] { false, false, false, true };
	private char[] batteryData = new char[] { 13, 13 };
	private long lastFMS = 0;
	private long lastFMSSent = 0;
	char[] stationID = new char[2];
	private boolean isRobotConnected;

	/*
	 * controlState[0] //null controlState[1] //WAP link controlState[2] //check
	 * version #s controlState[3] //request DS info? controlState[4]
	 * //auto-teleop controlState[5] //enabled-disabled controlState[6] //not
	 * estop controlState[7] //null
	 */

	public FMSController(int team) {
		teamNumber = team;
		try {
			FMSsendingSocket = new DatagramSocket(FMSsendPort);
		} catch (SocketException ex) {
			log.log(Level.WARNING, "Error creating FMS Sockets", ex);
		}
		for (int i = 0; i < 4; i++) {
			ipFMS[i] = (byte) 0xFF;
		}
		dataInput = new FMSDataIn(this);
		dataInput.setDaemon(true);
		setDaemon(true);
		start();
	}

	@Override
	public void start() {
		dataInput.start();
		super.start();
	}

	@Override
	public void run() {
		while (true) {
			if ((System.currentTimeMillis() > (lastFMSSent + 100))) {
				lastFMSSent = System.currentTimeMillis();
				sendFMSData();

			}
		}
	}

	private void setIP(byte[] ipIn) {
		ipFMS = ipIn;
	}

	@Override
	public boolean isFMSConnected() {

		return ((lastFMS + 1000) > System.currentTimeMillis());
	}

	private void receiveFMSpacket(byte[] receiveData) {

		lastFMS = System.currentTimeMillis();
		// reveive data 0, 1 are useless
		// 2 contains control data
		for (int i = 0; i < 8; i++) {
			controlState[i] = (receiveData[2] & (1 << (i))) > 0;
		}
		stationID[0] = (char) receiveData[3];
		stationID[1] = (char) receiveData[4];

	}

	private void sendFMSData() {
		CRC32 crctest = new CRC32();
		byte[] sendData = new byte[50];

		for (int i = 0; i < 50; i++) {
			sendData[i] = 0x00;
		}

		if (isRobotConnected)// {auto, test mode, enabled, not-estopped}
		{
			sendData[2] += 0x02;
		}
		if (fromRobot[0])// 0 holds the mode (1=auto 0=teleop)
		{
			sendData[2] += 0x10;
		}
		if (fromRobot[2])// 1 holds enabled (1=enabled)
		{
			sendData[2] += 0x20;
		}
		if (fromRobot[3])// 2 holds not e-stoped (1=good, 0=bad)
		{
			sendData[2] += 0x40;
		}

		byte teamHigh = (byte) ((teamNumber - (teamNumber % 100)) / 100);
		byte teamLow = (byte) (teamNumber % 100);
		sendData[6] = 10;// IP address
		sendData[7] = teamHigh;
		sendData[8] = teamLow;
		sendData[9] = 5;// make this variable!!!!

		sendData[10] = (byte) stationID[0];// the alliance color
		sendData[11] = (byte) stationID[1]; // the station number

		sendData[26] = 0x00;// amount of dropped packets
		sendData[27] = 0x00;// amount of dropped packets

		sendData[28] = 0x00;// number of packets received
		sendData[29] = 0x00;// number of packets received

		sendData[30] = 0x00;// trip time
		sendData[31] = 0x00;// trip time
		sendData[32] = 0x00;// trip time
		sendData[33] = 0x00;// trip time

		// need to capture actual voltage
		sendData[40] = (byte) batteryData[0];
		sendData[41] = (byte) batteryData[1];
		/*
		 * get CRC
		 */
		crctest.reset();
		crctest.update(sendData);

		/*
		 * convert CRC to byte format
		 */
		long crccheck = crctest.getValue();
		sendData[46] = (byte) ((crccheck >> 24) & 0xff);
		sendData[47] = (byte) ((crccheck >> 16) & 0xff);
		sendData[48] = (byte) ((crccheck >> 8) & 0xff);
		sendData[49] = (byte) ((crccheck) & 0xff);

		log.log(Level.FINE, "Sending FMS Data");
		/*
		 * send packet
		 */
		try {
			DatagramPacket sendPacket = new DatagramPacket(sendData,
					sendData.length, InetAddress.getByAddress(ipFMS),
					FMSsendPort);
			FMSsendingSocket.send(sendPacket);
		} catch (Exception e) {
		}
	}

	@Override
	public char getAllianceColor() {
		return stationID[0];
	}

	@Override
	public char getDSNumber() {
		return stationID[1];
	}

	@Override
	public boolean isEnabled() {
		return controlState[5];
	}

	@Override
	public ControlMode getControlMode() {
		if (!controlState[4]) {
			return ControlMode.TELEOP;
		}
		return ControlMode.AUTO;
	}

	@Override
	public void updateBattery(char[] batteryData) {
		this.batteryData = batteryData;
	}

	@Override
	public void updateRobotFeedback(boolean[] fromRobot) {
		this.fromRobot = fromRobot;
	}

	@Override
	public void setRobotConnected(boolean connected) {
		isRobotConnected = connected;
	}
}
