package edu.wpi.first.wpilibj.networktables2.util;

/**
 * An unsynchronized map which maps characters to objects. This map is backed by a array and will only perform well the values are consecutive starting at 0
 * 
 * @author Mitchell
 *
 */
public class CharacterArrayMap extends ResizeableArrayObject{
	/**
	 * Put a value with the given key
	 * @param key
	 * @param value
	 */
	public void put(final char key, final Object value){
		ensureSize(key+1);
		array[key] = value;
	}
	
	/**
	 * @param key
	 * @return the value associated with the given key or null if there is no value
	 */
	public Object get(final char key){
		if(key>=array.length)
			return null;
		return array[key];
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
		if(key>=array.length)
			return;
		array[key] = null;
	}
}
