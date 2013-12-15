package edu.wpi.first.wpilibj.networktables2.util;


/**
 * A queue designed to have things appended to it and then to be read directly from the backing array
 * 
 * @author mwills
 *
 */
public class HalfQueue {
	public final Object[] array;
	private int size = 0;
	public HalfQueue(int maxSize){
		array = new Object[maxSize];
	}
	
	
	/**
	 * Push an element onto the stack
	 * @param element
	 */
	public void queue(final Object element){
		array[size++] = element;
	}
	public boolean isFull(){
		return size==array.length;
	}
	public int size(){
		return size;
	}
	
	public void clear(){
		size = 0;
	}
}