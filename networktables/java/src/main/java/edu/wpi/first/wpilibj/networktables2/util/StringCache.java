package edu.wpi.first.wpilibj.networktables2.util;

import java.util.*;

/**
 * A simple cache that allows for caching the mapping of one string to another calculated one
 * 
 * @author Mitchell
 *
 */
public abstract class StringCache {
	private final Hashtable cache = new Hashtable();

	
	/**
	 * @param input
	 * @return the value for a given input
	 */
	public String get(final String input){
		String cachedValue = (String)cache.get(input);
		if(cachedValue==null)
			cache.put(input, cachedValue = calc(input));
		return cachedValue;
	}
	
	/**
	 * Will only be called if a value has not already been calculated
	 * @param input
	 * @return the calculated value for a given input
	 */
	public abstract String calc(String input);
}
