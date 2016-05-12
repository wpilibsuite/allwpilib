package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.tables.TableKeyNotDefinedException;

/**
 * An exception throw when the lookup a a key-value fails in a {@link NetworkTable}
 * 
 * @deprecated to provide backwards compatability for new api
 * 
 * @author Mitchell
 *
 */
@Deprecated
public class NetworkTableKeyNotDefined extends TableKeyNotDefinedException {

	/**
	 * @param key the key that was not defined in the table
	 */
	public NetworkTableKeyNotDefined(String key) {
		super(key);
	}

}
