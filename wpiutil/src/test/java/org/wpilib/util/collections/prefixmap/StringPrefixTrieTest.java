// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.collections.prefixmap;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;

import org.junit.jupiter.api.Test;

class StringPrefixTrieTest {
  @Test
  void removePrunesEmptyBranches() {
    var trie = new StringPrefixTrie<String>();
    trie.add("abc", "removed");
    trie.add("abd", "kept");

    assertEquals("removed", trie.remove("abc"));
    assertNull(trie.get("abc"));
    assertEquals("kept", trie.get("abd"));

    assertEquals("kept", trie.remove("abd"));
    assertFalse(trie.hasChildNodes());
  }
}
