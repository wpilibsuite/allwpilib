package edu.wpi.first.wpilibj.networktables2.util;


/**
 * 
 * A simple unsynchronized stack implementation
 * 
 * @author mwills
 *
 */
public class Stack extends List{

	/**
	 * Push an element onto the stack
	 * @param element
	 */
	public void push(final Object element){
		add(element);
	}
	/**
	 * @return the element on the top of the stack and remove it
	 */
	public Object pop(){
		if(size==0)
			return null;
		Object value = get(size-1);
		remove(size-1);
		return value;
	}
}