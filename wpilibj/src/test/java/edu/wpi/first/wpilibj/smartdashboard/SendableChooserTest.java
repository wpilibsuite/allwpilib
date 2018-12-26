/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import java.time.Duration;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.function.BiConsumer;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.networktables.NetworkTableInstance;

import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTimeoutPreemptively;

class SendableChooserTest {
  @BeforeEach
  void setup() {
    NetworkTableInstance.getDefault().deleteAllEntries();
  }

  @AfterEach
  void tearDown() {
    NetworkTableInstance.getDefault().deleteAllEntries();
  }

  @Test
  void testListenerWhenAddedBeforeInit() {
    final CompletableFuture<Integer> listenerCalled = new CompletableFuture<>();
    BiConsumer<String, Integer> listener = (key, value) -> listenerCalled.complete(value);
    SendableChooser<Integer> chooser = createSendableChooser();

    chooser.addSelectionChangedListener(listener);
    SmartDashboard.putData("TestChooser", chooser);
    NetworkTableInstance.getDefault()
        .getTable("SmartDashboard")
        .getSubTable("TestChooser")
        .getEntry("selected")
        .setString("Option 2");

    assertTimeoutPreemptively(Duration.ofSeconds(1), () -> {
      listenerCalled.get();
    }, "Listener was not called within alloted time");
  }

  @Test
  void testListenerWhenAddedAfterInit() {
    final CompletableFuture<Integer> listenerCalled = new CompletableFuture<>();
    BiConsumer<String, Integer> listener = (key, value) -> listenerCalled.complete(value);
    SendableChooser<Integer> chooser = createSendableChooser();

    SmartDashboard.putData("TestChooser", chooser);
    chooser.addSelectionChangedListener(listener);
    NetworkTableInstance.getDefault()
        .getTable("SmartDashboard")
        .getSubTable("TestChooser")
        .getEntry("selected")
        .setString("Option 2");

    assertTimeoutPreemptively(Duration.ofSeconds(1), () -> {
      listenerCalled.get();
    }, "Listener was not called within alloted time");
  }

  @Test
  void testListenerRemove() {
    final CompletableFuture<Integer> listenerCalled = new CompletableFuture<>();
    BiConsumer<String, Integer> listener = (key, value) -> listenerCalled.complete(value);
    SendableChooser<Integer> chooser = createSendableChooser();

    chooser.addSelectionChangedListener(listener);
    SmartDashboard.putData("TestChooser", chooser);
    chooser.removeSelectionChangedListener(listener);
    NetworkTableInstance.getDefault()
        .getTable("SmartDashboard")
        .getSubTable("TestChooser")
        .getEntry("selected")
        .setString("Option 2");

    assertThrows(TimeoutException.class, () -> {
      listenerCalled.get(1, TimeUnit.SECONDS);
    }, "Listener was called when it shouldn't have");
  }

  private static SendableChooser<Integer> createSendableChooser() {
    SendableChooser<Integer> chooser = new SendableChooser<>();
    chooser.setDefaultOption("Option 1 (default)", 1);
    chooser.addOption("Option 2", 2);
    chooser.addOption("Option 3", 3);
    return chooser;
  }
}
