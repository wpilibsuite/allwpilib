package edu.wpi.first.wpilibj.tables;

/**
 * A simple interface to provide tables
 * 
 * @author Mitchell
 *
 */
public interface ITableProvider {
	/**
	 * Get a table by name
	 * @param name the name of the table
	 * @return a Table with the given name
	 */
	public ITable getTable(String name);
}
