package edu.wpi.first.wpilibj.networktables2.util;



/**
 * 
 * A simple unsynchronized list implementation
 * 
 * @author mwills
 *
 */
public class List extends ResizeableArrayObject{

	protected int size = 0;
	/**
	 * create a new list
	 */
	public List() {
	}
	/**
	 * Create a new list with a given initial size of the backing array
	 * @param initialSize the initial size of the backing array
	 */
	public List(final int initialSize) {
		super(initialSize);
	}

	/**
	 * @return true if the list is empty
	 */
	public boolean isEmpty() {
		return size == 0;
	}

	/**
	 * @return the size of the list
	 */
	public int size(){
		return size;
	}

	/**
	 * Add an object to the end of the list
	 * @param o the object to be added
	 */
	public void add(final Object o){
		ensureSize(size+1);
		array[size++] = o;
	}

	/**
	 * Remove an element from the list
	 * @param index the index of the element to be removed
	 * @throws IndexOutOfBoundsException if the index is not in the list
	 */
	public void remove(final int index) throws IndexOutOfBoundsException{
		if(index<0 || index>=size)
			throw new IndexOutOfBoundsException();
		if(index < size-1)
			System.arraycopy(array, index+1, array, index, size-index-1);
		size--;
	}

	/**
	 * clear all elements from the list
	 */
	public void clear(){
		size = 0;
	}

	/**
	 * @param index
	 * @return a given element from the list
	 */
	public Object get(final int index) {
		if(index<0 || index>=size)
			throw new IndexOutOfBoundsException();
		return array[index];
	}

	/**
	 * remove the first element in the list where {@link Object#equals(Object)} is true
	 * @param object
	 * @return true if the element was removed
	 */
	public boolean remove(final Object object) {
		for(int i = 0; i<size; ++i){
			Object value = array[i];
			if(object==null ? value==null : object.equals(value)){
				remove(i);
				return true;
			}
		}
		return false;
	}

	/**
	 * @param object
	 * @return true if the list contains the given element (where {@link Object#equals(Object)} is true)
	 */
	public boolean contains(final Object object) {
		for(int i = 0; i<size; ++i){
			Object value = array[i];
			if(object==null ? value==null : object.equals(value)){
				return true;
			}
		}
		return false;
	}

	/**
	 * set the value at a given index in the list
	 * @param index the index where the object should be put
	 * @param obj the new value at the index
	 */
	public void set(final int index, final Object obj) {
		if(index<0 || index>=size)
			throw new IndexOutOfBoundsException();
		array[index] = obj;
	}
}
