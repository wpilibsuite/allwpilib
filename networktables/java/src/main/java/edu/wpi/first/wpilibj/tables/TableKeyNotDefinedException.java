package edu.wpi.first.wpilibj.tables;

import edu.wpi.first.wpilibj.networktables.NetworkTableKeyNotDefined;

/**
 * An exception throw when the lookup a a key-value fails in a {@link ITable}
 * 
 * @author Mitchell
 *
 */
public class TableKeyNotDefinedException extends NetworkTableKeyNotDefined {

	/**
	 * @param key the key that was not defined in the table
	 */
	public TableKeyNotDefinedException(String key) {
		super(key);
	}

}
