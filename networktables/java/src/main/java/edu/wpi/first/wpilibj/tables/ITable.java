package edu.wpi.first.wpilibj.tables;

import java.util.NoSuchElementException;


/**
 * A table whose values can be read and written to
 * 
 * @author Mitchell
 * 
 */
public interface ITable {

	/**
	 * @param key the key to search for
	 * @return true if the table as a value assigned to the given key
	 */
	public boolean containsKey(String key);

	/**
	 * @param key the key to search for
	 * @return true if there is a subtable with the key which contains at least one key/subtable of its own
	 */
	public boolean containsSubTable(String key);
	
    /**
     * @param key the name of the table relative to this one
     * @return a sub table relative to this one
     */
    public ITable getSubTable(String key);
	
    
	/**
	 * Gets the value associated with a key as an object
	 * @param key the key of the value to look up
	 * @return the value associated with the given key
	 * @throws TableKeyNotDefinedException if there is no value associated with the given key
	 */
	public Object getValue(String key) throws TableKeyNotDefinedException;
	/**
	 * Put a value in the table
	 * @param key the key to be assigned to
	 * @param value the value that will be assigned
	 * @throws IllegalArgumentException when the value is not supported by the table
	 */
	public void putValue(String key, Object value) throws IllegalArgumentException;
        
        public void retrieveValue(String key, Object externalValue);
	
	

	/**
	 * Put a number in the table
	 * @param key the key to be assigned to
	 * @param value the value that will be assigned
	 */
	public void putNumber(String key, double value);
	/**
	 * @param key the key to look up
	 * @return the value associated with the given key 
	 * @throws TableKeyNotDefinedException if there is no value associated with the given key
	 */
	public double getNumber(String key) throws TableKeyNotDefinedException;
	/**
	 * @param key the key to look up
	 * @param defaultValue the value to be returned if no value is found
	 * @return the value associated with the given key or the given default value if there is no value associated with the key
	 */
	public double getNumber(String key, double defaultValue);

	/**
	 * Put a string in the table
	 * @param key the key to be assigned to
	 * @param value the value that will be assigned
	 */
	public void putString(String key, String value);
	/**
	 * @param key the key to look up
	 * @return the value associated with the given key 
	 * @throws TableKeyNotDefinedException if there is no value associated with the given key
	 */
	public String getString(String key) throws TableKeyNotDefinedException;
	/**
	 * @param key the key to look up
	 * @param defaultValue the value to be returned if no value is found
	 * @return the value associated with the given key or the given default value if there is no value associated with the key
	 */
	public String getString(String key, String defaultValue);

	/**
	 * Put a boolean in the table
	 * @param key the key to be assigned to
	 * @param value the value that will be assigned
	 */
	public void putBoolean(String key, boolean value);
	/**
	 * @param key the key to look up
	 * @return the value associated with the given key 
	 * @throws TableKeyNotDefinedException if there is no value associated with the given key
	 */
	public boolean getBoolean(String key) throws TableKeyNotDefinedException;
	/**
	 * @param key the key to look up
	 * @param defaultValue the value to be returned if no value is found
	 * @return the value associated with the given key or the given default value if there is no value associated with the key
	 */
	public boolean getBoolean(String key, boolean defaultValue);

	
	
	
	
	/**
	 * Add a listener for changes to the table
	 * @param listener the listener to add
	 */
	public void addTableListener(ITableListener listener);
	/**
	 * Add a listener for changes to the table
	 * @param listener the listener to add
	 * @param immediateNotify if true then this listener will be notified of all current entries (marked as new)
	 */
	public void addTableListener(ITableListener listener, boolean immediateNotify);
	
	/**
	 * Add a listener for changes to a specific key the table
	 * @param key the key to listen for
	 * @param listener the listener to add
	 * @param immediateNotify if true then this listener will be notified of all current entries (marked as new)
	 */
	public void addTableListener(String key, ITableListener listener, boolean immediateNotify);
	/**
	 * This will immediately notify the listener of all current sub tables
	 * @param listener
	 */
	public void addSubTableListener(final ITableListener listener);
	/**
	 * Remove a listener from receiving table events
	 * @param listener the listener to be removed
	 */
	public void removeTableListener(ITableListener listener);
        
        
        
     /*
      * Depricated Methods
      */
     /**
      * @deprecated 
     * Maps the specified key to the specified value in this table.
     * The key can not be null.
     * The value can be retrieved by calling the get method with a key that is equal to the original key.
     * @param key the key
     * @param value the value
     * @throws IllegalArgumentException if key is null
     */
    public void putInt(String key, int value);

    /**
     * @deprecated 
     * Returns the value at the specified key.
     * @param key the key
     * @return the value
     * @throws TableKeyNotDefinedException if there is no value mapped to by the key
     * @throws IllegalArgumentException if the value mapped to by the key is not an int
     * @throws IllegalArgumentException if the key is null
     */
    public int getInt(String key) throws TableKeyNotDefinedException;

    /**
     * @deprecated 
     * Returns the value at the specified key.
     * @param key the key
     * @param defaultValue the value returned if the key is undefined
     * @return the value
     * @throws NetworkTableKeyNotDefined if there is no value mapped to by the key
     * @throws IllegalArgumentException if the value mapped to by the key is not an int
     * @throws IllegalArgumentException if the key is null
     */
    public int getInt(String key, int defaultValue) throws TableKeyNotDefinedException;

    /**
     * @deprecated 
     * Maps the specified key to the specified value in this table.
     * The key can not be null.
     * The value can be retrieved by calling the get method with a key that is equal to the original key.
     * @param key the key
     * @param value the value
     * @throws IllegalArgumentException if key is null
     */
    public void putDouble(String key, double value);

    /**
     * @deprecated 
     * Returns the value at the specified key.
     * @param key the key
     * @return the value
     * @throws NoSuchEleNetworkTableKeyNotDefinedmentException if there is no value mapped to by the key
     * @throws IllegalArgumentException if the value mapped to by the key is not a double
     * @throws IllegalArgumentException if the key is null
     */
    public double getDouble(String key) throws TableKeyNotDefinedException;

    /**
     * @deprecated 
     * Returns the value at the specified key.
     * @param key the key
     * @param defaultValue the value returned if the key is undefined
     * @return the value
     * @throws NoSuchEleNetworkTableKeyNotDefinedmentException if there is no value mapped to by the key
     * @throws IllegalArgumentException if the value mapped to by the key is not a double
     * @throws IllegalArgumentException if the key is null
     */
    public double getDouble(String key, double defaultValue);
        
}
