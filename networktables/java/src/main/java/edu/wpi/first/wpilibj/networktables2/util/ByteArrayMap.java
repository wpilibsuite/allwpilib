package edu.wpi.first.wpilibj.networktables2.util;

/**
 * An unsynchronized map which maps bytes to objects. This map is backed by a array and will only perform well the values are consecutive starting at 0
 * 
 * @author Mitchell
 *
 */
public class ByteArrayMap extends ResizeableArrayObject{
	/**
	 * Put a value with the given key
	 * @param key
	 * @param value
	 */
	public void put(final byte key, final Object value){
		int offsetKey = key+128;
		ensureSize(offsetKey+1);
		array[offsetKey] = value;
	}
	
	/**
	 * @param key
	 * @return the value associated with the given key or null if there is no value
	 */
	public Object get(final byte key){
		int offsetKey = key+128;
		if(offsetKey>=array.length)
			return null;
		return array[offsetKey];
	}

	/**
	 * clear all values
	 */
	public void clear() {
		for(int i = 0; i<array.length; ++i)
			array[i] = null;
	}

	/**
	 * Remove the value for the given key
	 * @param key
	 */
	public void remove(final char key) {
		int offsetKey = key+128;
		if(offsetKey>=array.length)
			return;
		array[offsetKey] = null;
	}
}
