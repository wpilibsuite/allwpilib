package edu.wpi.first.wpilibj.networktables;

import java.io.IOException;

/**
 * An exception thrown when persistent load/save fails in a {@link NetworkTable}
 * 
 */
public class PersistentException extends IOException {

	/**
	 * @param message The error message
	 */
	public PersistentException(String message) {
		super(message);
	}

}
