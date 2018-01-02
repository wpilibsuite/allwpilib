/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.util.ArrayList;
import java.util.List;
import junit.framework.TestCase;

public class NetworkTableTest extends TestCase {
  public void testBasenameKey() {
    assertEquals("simple", NetworkTable.basenameKey("simple"));
    assertEquals("simple", NetworkTable.basenameKey("one/two/many/simple"));
    assertEquals("simple",
                 NetworkTable.basenameKey("//////an/////awful/key////simple"));
  }

  public void testNormalizeKeySlash() {
    assertEquals("/", NetworkTable.normalizeKey("///"));
    assertEquals("/no/normal/req", NetworkTable.normalizeKey("/no/normal/req"));
    assertEquals("/no/leading/slash",
                 NetworkTable.normalizeKey("no/leading/slash"));
    assertEquals(
        "/what/an/awful/key/",
        NetworkTable.normalizeKey("//////what////an/awful/////key///"));
  }

  public void testNormalizeKeyNoSlash() {
    assertEquals("a", NetworkTable.normalizeKey("a", false));
    assertEquals("a", NetworkTable.normalizeKey("///a", false));
    assertEquals("leading/slash",
                 NetworkTable.normalizeKey("/leading/slash", false));
    assertEquals("no/leading/slash",
                 NetworkTable.normalizeKey("no/leading/slash", false));
    assertEquals(
        "what/an/awful/key/",
        NetworkTable.normalizeKey("//////what////an/awful/////key///", false));
  }

  public void testGetHierarchyEmpty() {
    List<String> expected = new ArrayList<>();
    expected.add("/");
    assertEquals(expected, NetworkTable.getHierarchy(""));
  }

  public void testGetHierarchyRoot() {
    List<String> expected = new ArrayList<>();
    expected.add("/");
    assertEquals(expected, NetworkTable.getHierarchy("/"));
  }

  public void testGetHierarchyNormal() {
    List<String> expected = new ArrayList<>();
    expected.add("/");
    expected.add("/foo");
    expected.add("/foo/bar");
    expected.add("/foo/bar/baz");
    assertEquals(expected, NetworkTable.getHierarchy("/foo/bar/baz"));
  }

  public void testGetHierarchyTrailingSlash() {
    List<String> expected = new ArrayList<>();
    expected.add("/");
    expected.add("/foo");
    expected.add("/foo/bar");
    expected.add("/foo/bar/");
    assertEquals(expected, NetworkTable.getHierarchy("/foo/bar/"));
  }
}
