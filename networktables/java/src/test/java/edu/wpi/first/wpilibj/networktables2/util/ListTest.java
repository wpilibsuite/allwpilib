package edu.wpi.first.wpilibj.networktables2.util;

import static org.hamcrest.CoreMatchers.*;
import static org.junit.Assert.*;

import org.hamcrest.*;
import org.junit.*;

/**
 * @author mwills
 * @author Fred
 */
public class ListTest {

	@Test
	public void testIsEmpty() {
		List list = new List();
		assertThat(list, isEmpty());
		list.add(new Integer(42));
		assertThat(list, not(isEmpty()));
	}

	@Test
	public void testSize() {
		List list = new List();
		assertThat(list, isEmpty());
		for (int i = 0; i < 42; i++) {
			list.add(new Integer(i));
			assertThat(list, hasSize(i+1));
		}
	}
	
	@Test
	public void testSizeGrowth() {
		List list = new List(10);
		assertThat(list, isEmpty());
		for (int i = 0; i < 100; i++) {
			list.add(new Integer(i));
		}
		for (int i = 0; i < 100; i++) {
			assertThat(list, hasItem(i, equalTo(new Integer(i))));
		}
	}

	@Test
	public void testContains() {
		List list = new List();
		Integer testInt1 = new Integer(42);
		Integer testInt2 = new Integer(43);
		assertThat(list, not(contains(testInt1)));
		list.add(testInt1);
		assertThat(list, contains(testInt1));
		assertThat(list, not(contains(testInt2)));
		list.add(testInt2);
		assertThat(list, contains(testInt2));
	}

	@Test
	public void testAddRemoveObject() {
		List list = new List();
		Integer testInt1 = new Integer(42);
		Integer testInt2 = new Integer(43);
		Integer testInt3 = new Integer(44);
		list.add(testInt1);
		list.add(testInt2);
		list.add(testInt3);
		assertThat(list, contains(testInt1));
		assertThat(list, contains(testInt2));
		assertThat(list, contains(testInt3));
		assertThat(list, hasSize(3));
		list.remove(testInt1);
		list.remove(testInt2);
		list.remove(testInt3);
		assertThat(list, not(contains(testInt1)));
		assertThat(list, not(contains(testInt2)));
		assertThat(list, not(contains(testInt3)));
	}

	@Test
	public void testAddRemoveIndex() {
		List list = new List();
		Integer testInt1 = new Integer(42);
		Integer testInt2 = new Integer(43);
		Integer testInt3 = new Integer(44);
		list.add(testInt1);
		list.add(testInt2);
		list.add(testInt3);
		assertThat(list, hasItem(0, equalTo(testInt1)));
		assertThat(list, hasItem(1, equalTo(testInt2)));
		assertThat(list, hasItem(2, equalTo(testInt3)));
		assertThat(list, hasSize(3));
		list.remove(2);
		list.remove(1);
		list.remove(0);
		assertThat(list, isEmpty());
	}

	@Test
	public void testClear() {
		List list = new List();
		Integer testInt1 = new Integer(42);
		Integer testInt2 = new Integer(43);
		Integer testInt3 = new Integer(44);
		list.add(testInt1);
		list.add(testInt2);
		list.add(testInt3);
		assertThat(list, not(isEmpty()));
		list.clear();
		assertTrue(list.isEmpty());
	}

	@Test
	public void testOrdering(){
		Integer obj1 = new Integer(42);
		Boolean obj2 = new Boolean(false);
		Double obj3 = new Double(42.42);
		List list = new List();
		list.add(obj1);
		list.add(obj2);
		list.add(obj3);
		assertThat(list, hasItem(0, equalTo(obj1)));
		assertThat(list, hasItem(1, equalTo(obj2)));
		assertThat(list, hasItem(2, equalTo(obj3)));
		list.remove(obj2);
		assertThat(list, hasItem(0, equalTo(obj1)));
		assertThat(list, hasItem(1, equalTo(obj3)));
	}

	@Test
	public void testSet(){
		Integer obj1 = new Integer(42);
		Boolean obj2 = new Boolean(false);
		Double obj3 = new Double(42.42);
		String obj4 = "MyString";
		String obj5 = "MyString2";
		String obj6 = "MyString3";
		List list = new List();
		list.add(obj1);
		list.add(obj2);
		list.add(obj3);
		assertThat(list, hasItem(0, equalTo(obj1)));
		assertThat(list, hasItem(1, equalTo(obj2)));
		assertThat(list, hasItem(2, equalTo(obj3)));
		list.set(1, obj4);
		assertThat(list, hasItem(0, equalTo(obj1)));
		assertThat(list, hasItem(1, equalTo(obj4)));
		assertThat(list, hasItem(2, equalTo(obj3)));
		assertThat(list, hasSize(3));
		list.set(2, obj5);
		assertThat(list, hasItem(0, equalTo(obj1)));
		assertThat(list, hasItem(1, equalTo(obj4)));
		assertThat(list, hasItem(2, equalTo(obj5)));
		assertThat(list, hasSize(3));
		list.set(0, obj6);
		assertThat(list, hasItem(0, equalTo(obj6)));
		assertThat(list, hasItem(1, equalTo(obj4)));
		assertThat(list, hasItem(2, equalTo(obj5)));
		assertThat(list, hasSize(3));
	}
	
	
	
	
	
	
	public Matcher<List> contains(final Object object){
		return new TypeSafeMatcher<List>(){

			@Override
			public void describeTo(Description description) {
				description.appendText("contains ").appendValue(object);
			}

			@Override
			public boolean matchesSafely(List item) {
				return item.contains(object);
			}
			
		};
	}
	public Matcher<List> isEmpty(){
		return new TypeSafeMatcher<List>(){

			@Override
			public void describeTo(Description description) {
				description.appendText("is empty");
			}

			@Override
			public boolean matchesSafely(List item) {
				return item.isEmpty();
			}
			
		};
	}
	public Matcher<List> hasSize(final int size){
		return new TypeSafeMatcher<List>(){

			@Override
			public void describeTo(Description description) {
				description.appendText("has size ").appendValue(size);
			}

			@Override
			public boolean matchesSafely(List item) {
				return item.size()==size;
			}
			
		};
	}
	public Matcher<List> hasItem(final int index, final Matcher<?> matcher){
		return new TypeSafeMatcher<List>(){

			@Override
			public void describeTo(Description description) {
				description.appendText("has ").appendDescriptionOf(matcher).appendText(" at index ").appendValue(index);
			}

			@Override
			public boolean matchesSafely(List item) {
				return matcher.matches(item.get(index));
			}
			
		};
	}
}
