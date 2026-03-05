package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.util.Optional;

import org.junit.jupiter.api.Test;

public class ConstructorMatchTest {
    public static class TestClass {
        public TestClass() {}

        public TestClass(String s) {}

        public TestClass(Optional<?> o) {}

        public TestClass(String s, Optional<?> o) {}
    }

    public static class TestInvalidParameterClass {
        public TestInvalidParameterClass(String s, Object o) {}
    }

    @Test
    public void testTooManyParameters() {
        var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class, Object.class);
        assertTrue(ctor.isEmpty());
    }

    @Test
    public void testUnassignableParameters() {
        var ctor = ConstructorMatch.findBestConstructor(TestInvalidParameterClass.class, String.class, Object.class);
        assertTrue(ctor.isEmpty());
    }

    @Test
    public void testValidConstructorNoArgs() throws ReflectiveOperationException {
        var ctor = ConstructorMatch.findBestConstructor(TestClass.class);
        assertTrue(ctor.isPresent());
        ctor.get().newInstance();
        ctor.get().newInstance("test", Optional.empty());
        ctor.get().newInstance(Optional.empty(), "test");
        ctor.get().newInstance("test");
        ctor.get().newInstance(Optional.empty());
    }

    @Test
    public void testValidConstructorString() throws ReflectiveOperationException {
        var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class);
        assertTrue(ctor.isPresent());
        ctor.get().newInstance("test", Optional.empty());
        ctor.get().newInstance(Optional.empty(), "test");
        ctor.get().newInstance("test");
    }

    @Test
    public void testInvalidConstructorString() throws ReflectiveOperationException {
        var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class);
        assertTrue(ctor.isPresent());
        assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance());
        assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance(Optional.empty()));
    }

    @Test
    public void testValidConstructorOptional() throws ReflectiveOperationException {
        var ctor = ConstructorMatch.findBestConstructor(TestClass.class, Optional.class);
        assertTrue(ctor.isPresent());
        ctor.get().newInstance("test", Optional.empty());
        ctor.get().newInstance(Optional.empty(), "test");
        ctor.get().newInstance(Optional.empty());
    }

    @Test
    public void testInvalidConstructorOptional() throws ReflectiveOperationException {
        var ctor = ConstructorMatch.findBestConstructor(TestClass.class, Optional.class);
        assertTrue(ctor.isPresent());
        assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance());
        assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance("test"));
    }

    @Test
    public void testValidConstructorStringOptional() throws ReflectiveOperationException {
        var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class, Optional.class);
        assertTrue(ctor.isPresent());
        ctor.get().newInstance("test", Optional.empty());
        ctor.get().newInstance(Optional.empty(), "test");
    }

    @Test
    public void testInvalidConstructorStringOptional() throws ReflectiveOperationException {
        var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class, Optional.class);
        assertTrue(ctor.isPresent());
        assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance());
        assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance("test"));
        assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance(Optional.empty()));
    }
}
