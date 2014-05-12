package edu.wpi.first.driverstation;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ReadThread implements Runnable {

	private final Logger log = Logger.getLogger(DriverStation.class.getName());
	Socket sock;
	DriverStation ds;

	public ReadThread(Socket sock, DriverStation ds) {
		this.sock = sock;
		this.ds = ds;
	}

	@Override
	public void run() {
		try {
			BufferedReader netIn = new BufferedReader(new InputStreamReader(
					sock.getInputStream()));
			while (!Thread.interrupted()) {
				String com = netIn.readLine();
				log.log(Level.FINE, "Received command " + com);
				ds.addCommand(com);
			}
		} catch (IOException e) {
			log.log(Level.WARNING, "IO Exception in net thread", e);
		} finally {
			try {
				sock.close();
			} catch (IOException | NullPointerException ex) {
				log.log(Level.SEVERE, "Error when closing the socket", ex);
			}
		}
	}

}
