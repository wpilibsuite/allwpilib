package edu.wpi.first.wpilibj.tables;

import java.util.NoSuchElementException;

/**
 * An exception throw when the lookup a a key-value fails in a {@link ITable}
 * 
 * @author Mitchell
 *
 */
public class TableKeyNotDefinedException extends NoSuchElementException {

	/**
	 * @param key the key that was not defined in the table
	 */
	public TableKeyNotDefinedException(String key) {
		super("Unknown Table Key: "+key);
	}

}
