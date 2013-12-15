package edu.wpi.first.wpilibj.networktables2.util;



/**
 * 
 * A unsynchronized set
 * 
 * @author mwills
 *
 */
public class Set extends List{

	/**
	 * Add an element to the set
	 * @param o
	 */
	public void add(final Object o){
		if(!contains(o))
			super.add(o);
	}
}
