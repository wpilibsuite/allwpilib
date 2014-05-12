package edu.wpi.first.driverstation;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.logging.ConsoleHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

import edu.wpi.first.driverstation.fms.FMSController;
import edu.wpi.first.driverstation.fms.FMSInterface;
import edu.wpi.first.driverstation.robotcoms.ControlMode;
import edu.wpi.first.driverstation.robotcoms.RobotComm;

/**
 * Driver station implementation for the test stand. This is based heavily on
 * code done by Brendan McLeod, modified for command line argument and local
 * socket input.
 * 
 * @author Fredric Silberberg
 */
public class DriverStation {

	protected static final Logger log = Logger.getLogger(DriverStation.class
			.getName());
	protected static DriverStation station;

	public static void main(String[] args) {
		int team = 190, port = 6190;
		List<String> commandQueue = new LinkedList<>();

		// If there are command line arguments, then input them
		if (args.length > 0) {
			for (int i = 0; i < args.length; i++) {
				String command = args[i];
				switch (command) {
				case "-t":
					// Get the team number if there is still an argument
					if (i + 1 < args.length) {
						try {
							team = Integer.parseInt(args[++i]);
						} catch (NumberFormatException ex) {
							log.log(Level.WARNING,
									"Error, non-integer team number " + args[i]);
							displayUsage();
							System.exit(-1);
						}
					} else {
						log.log(Level.WARNING,
								"Error, no team number provided.");
						displayUsage();
						System.exit(-1);
					}
					log.log(Level.INFO, "Team number is set to " + team);
					break;
				case "-h":
					// Display help
					displayUsage();
					return;
					// TODO: Support networked commands
					// case "-p":
					// // Get the port number
					// // Get the team number if there is still an argument
					// if (i + 1 < args.length) {
					// try {
					// port = Integer.parseInt(args[++i]);
					// } catch (NumberFormatException ex) {
					// log.log(Level.WARNING,
					// "Error, non-integer port number " + args[i]);
					// displayUsage();
					// System.exit(-1);
					// }
					// } else {
					// log.log(Level.WARNING,
					// "Error, no port number provided.");
					// displayUsage();
					// System.exit(-1);
					// }
					// log.log(Level.INFO, "Port number is set to " + port);
					// break;
				case "enable":
				case "disable":
				case "teleop":
				case "auto":
				case "estop":
				case "reboot":
				case "test":
					// Add the command to the queue of commands
					commandQueue.add(command);
					break;
				default:
					log.log(Level.SEVERE, "Unknown input " + command);
					displayUsage();
					System.exit(-1);
				}
			}
		}
		station = new DriverStation(team, port);
		for (String command : commandQueue) {
			station.addCommand(command);
		}

		station.run();
	}

	public static void displayUsage() {
		System.out.println("The Java FRC Driverstation");
		System.out.println("To Use: java -jar <driverstation jar> [commands]");
		System.out.println("-h: Print this display and exit");
		System.out.println("-t [team]: Specify the team number (default 190)");
		System.out
				.println("enable, disable, teleop, auto, test, estop, reboot");
		System.out.println("Run the given commands on startup");
	}

	RobotComm robot;
	FMSInterface fms;
	LinkedBlockingQueue<String> queue;
	Thread netThread, sysThread;

	public DriverStation(final int team, final int port) {
		queue = new LinkedBlockingQueue<>();
		robot = new RobotComm(team);
		fms = new FMSController(team);
		robot.setFMSController(fms);
		robot.setDaemon(true);
		sysThread = new Thread(new Runnable() {
			public void run() {
				BufferedReader sysIn = new BufferedReader(
						new InputStreamReader(System.in));
				while (!Thread.interrupted()) {
					try {
						String com = sysIn.readLine();
						log.log(Level.FINE, "Recieved stdin command " + com);
						station.addCommand(com);
					} catch (IOException ex) {
						log.log(Level.WARNING, "Error in system read thread",
								ex);
					}
				}
			}
		});
		sysThread.setDaemon(true);

		// TODO: Test the networking protocol
		// netThread = new Thread(new Runnable() {
		// public void run() {
		// ServerSocket servSock = null;
		// List<Thread> connections = new LinkedList<>();
		// try {
		// servSock = new ServerSocket(port);
		// while (!Thread.interrupted()) {
		// Socket sock = servSock.accept();
		// Thread newCon = new Thread(new ReadThread(sock, station));
		// newCon.run();
		// connections.add(newCon);
		// log.log(Level.INFO, "Accepted new network connection");
		// }
		// } catch (IOException ex) {
		// log.log(Level.WARNING, "Error with the network thread", ex);
		// } finally {
		// try {
		// for (Thread t : connections) {
		// t.interrupt();
		// }
		// servSock.close();
		// } catch (IOException | NullPointerException e) {
		// log.log(Level.SEVERE,
		// "Error when closing the server socket", e);
		// }
		// }
		// }
		// });
		// netThread.setDaemon(true);
	}

	public void run() {
		System.out.println("Activating driver station");
		robot.start();
		System.out
				.println("The driverstation supports the following commands:");
		System.out
				.println("enable, disable, teleop, auto, test, reboot, estop, quit");
		sysThread.start();
		// netThread.start();
		while (true) {
			try {
				String command = queue.take();
				log.log(Level.FINE, "Received command " + command);
				switch (command) {
				case "enable":
					log.log(Level.INFO, "Enabling robot");
					robot.setEnabled(true);
					break;
				case "disable":
					log.log(Level.INFO, "Disabling robot");
					robot.setEnabled(false);
					break;
				case "teleop":
					log.log(Level.INFO, "Setting robot to teleop mode");
					robot.setControlMode(ControlMode.TELEOP);
					break;
				case "auto":
					log.log(Level.INFO, "Setting robot to auto mode");
					robot.setControlMode(ControlMode.AUTO);
					break;
				case "test":
					log.log(Level.INFO, "Setting robot to test mode");
					robot.setControlMode(ControlMode.TEST_MODE);
					break;
				case "estop":
					log.log(Level.INFO, "Estoping robot");
					robot.eStopRobot();
					break;
				case "reboot":
					log.log(Level.INFO, "Rebooting robot");
					robot.rebootRobot();
					break;
				case "quit":
					log.log(Level.INFO, "Shutting down");
					sysThread.interrupt();
					// netThread.interrupt();
					return;
				default:
					log.log(Level.WARNING, "Unknown input " + command);
					break;
				}
			} catch (InterruptedException e) {
				log.log(Level.WARNING, "Error when taking command", e);
			}
		}
	}

	public void addCommand(String command) {
		try {
			queue.put(command);
		} catch (InterruptedException e) {
			log.log(Level.WARNING, "Error when added an element to the queue",
					e);
		}
	}
}
