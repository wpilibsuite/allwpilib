package edu.wpi.first.wpilibj.networktables2.util;

/**
 * A helper super class for collections that use a resizing array
 * 
 * @author Mitchell
 *
 */
public class ResizeableArrayObject {
	private static final int GROW_FACTOR = 3;
	
	protected Object[] array;
	
	protected ResizeableArrayObject(){
		this(10);
	}
	protected ResizeableArrayObject(int initialSize){
		array = new Object[initialSize];
	}
	protected int arraySize(){
		return array.length;
	}
	protected void ensureSize(int size){
		if(size>array.length){
			int newSize = array.length;
			while(size>newSize)
				newSize *= GROW_FACTOR;
			Object[] newArray = new Object[newSize];
			System.arraycopy(array, 0, newArray, 0, array.length);
			array = newArray;
		}
	}
}
