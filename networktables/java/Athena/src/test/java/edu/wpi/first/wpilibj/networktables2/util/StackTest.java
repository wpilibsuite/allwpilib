package edu.wpi.first.wpilibj.networktables2.util;

import static org.hamcrest.CoreMatchers.*;
import static org.junit.Assert.*;

import org.hamcrest.*;
import org.junit.*;

/**
 * @author mwills
 * @author Fred
 */
public class StackTest {

	@Test
	public void pushTest(){
		Integer obj1 = new Integer(0);
		Stack stack = new Stack();
		assertThat(stack, isEmpty());
		stack.push(obj1);
		assertThat(stack, not(isEmpty()));
	}
	
	@Test
	public void popTest(){
		Integer obj1 = new Integer(0);
		Object obj2;
		Stack stack = new Stack();
		assertThat(stack, isEmpty());
		obj2 = stack.pop();
		assertNull(obj2);
		stack.push(obj1);
		obj2 = stack.pop();
		assertSame(obj1, obj2);
		assertThat(stack, isEmpty());	
	}
	
	@Test
	public void orderingTest(){
		Integer obj1 = new Integer(42);
		Integer obj2 = new Integer(43);
		Integer obj3 = new Integer(44);
		Object obj4;
		Object obj5;
		Object obj6;
		Object obj7;
		Stack stack = new Stack();
		assertThat(stack, isEmpty());
		stack.push(obj1);
		assertThat(stack, hasSize(1));
		stack.push(obj2);
		stack.push(obj3);
		assertThat(stack, hasSize(3));
		obj4 = stack.pop();
		assertThat(stack, hasSize(2));
		obj5 = stack.pop();
		obj6 = stack.pop();
		assertSame(obj1, obj6);
		assertSame(obj2, obj5);
		assertSame(obj3, obj4);
		stack.push(obj1);
		stack.push(obj2);
		stack.push(obj3);
		stack.pop();
		stack.push(obj1);
		obj7 = stack.pop();
		assertSame(obj1, obj7);
	}
	


	public Matcher<Stack> isEmpty(){
		return new TypeSafeMatcher<Stack>(){

			@Override
			public void describeTo(Description description) {
				description.appendText("is empty");
			}

			@Override
			public boolean matchesSafely(Stack item) {
				return item.isEmpty();
			}
			
		};
	}
	public Matcher<Stack> hasSize(final int size){
		return new TypeSafeMatcher<Stack>(){

			@Override
			public void describeTo(Description description) {
				description.appendText("has size ").appendValue(size);
			}

			@Override
			public boolean matchesSafely(Stack item) {
				return item.size()==size;
			}
			
		};
	}
}
