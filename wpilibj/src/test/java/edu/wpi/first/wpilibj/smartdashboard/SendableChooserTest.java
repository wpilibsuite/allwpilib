/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                             */
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
import org.junit.jupiter.api.function.Executable;

import edu.wpi.first.networktables.NetworkTableInstance;

import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTimeoutPreemptively;

@SuppressWarnings({"LawOfDemeter", "UnnecessaryFinalOnLocalVariableOrParameter"})
class SendableChooserTest {
  @BeforeEach
  void setup() {
    NetworkTableInstance.getDefault().stopClient();
    NetworkTableInstance.getDefault().stopServer();
    NetworkTableInstance.getDefault().startLocal();
    NetworkTableInstance.getDefault().getTable("SmartDashboard").getKeys()
            .forEach(SmartDashboard::delete);
    NetworkTableInstance.getDefault().deleteAllEntries();
  }

  @AfterEach
  void tearDown() {
    NetworkTableInstance.getDefault().getTable("SmartDashboard").getKeys()
            .forEach(SmartDashboard::delete);
    NetworkTableInstance.getDefault().deleteAllEntries();
    NetworkTableInstance.getDefault().stopLocal();
  }

  @Test
  void testListenerWhenAddedBeforeInit() {
    final CompletableFuture<Integer> listenerCalled = new CompletableFuture<>();
    BiConsumer<String, Integer> listener = (key, value) -> listenerCalled.complete(value);
    SendableChooser<Integer> chooser = createSendableChooser();

    SmartDashboard.delete("TestChooser");
    chooser.addSelectionListener(listener);
    SmartDashboard.putData("TestChooser2", chooser);


    NetworkTableInstance.getDefault()
            .getTable("SmartDashboard")
            .getSubTable("TestChooser2")
            .getEntry("selected")
            .setString("Option 3");

    SmartDashboard.updateValues();


    assertTimeoutPreemptively(Duration.ofSeconds(1),
            (Executable) listenerCalled::get,
            "Listener was not called within allotted time");
  }

  @Test
  void clearListeners() {
    final CompletableFuture<Integer> listenerCalled = new CompletableFuture<>();
    BiConsumer<String, Integer> listener = (key, value) -> listenerCalled.complete(value);
    SendableChooser<Integer> chooser = createSendableChooser();

    SmartDashboard.putData("TestChooser5", chooser);
    chooser.addSelectionListener(listener);
    chooser.addSelectionListener(listener);
    chooser.addSelectionListener(listener);
    chooser.addSelectionListener(listener);
    chooser.addSelectionListener(listener);
    chooser.removeAllSelectionListeners();

    NetworkTableInstance.getDefault()
            .getTable("SmartDashboard")
            .getSubTable("TestChooser5")
            .getEntry("selected")
            .setString("Option 2");

    SmartDashboard.updateValues();

    assertThrows(TimeoutException.class, () -> {
      listenerCalled.get(1, TimeUnit.SECONDS);
    }, "Listener was called when it shouldn't have");

  }

  @Test
  void testListenerWhenAddedAfterInit() {
    final CompletableFuture<Integer> listenerCalled = new CompletableFuture<>();
    BiConsumer<String, Integer> listener = (key, value) -> listenerCalled.complete(value);
    SendableChooser<Integer> chooser = createSendableChooser();
    SmartDashboard.delete("TestChooser");

    SmartDashboard.putData("TestChooser3", chooser);
    chooser.addSelectionListener(listener);

    NetworkTableInstance.getDefault()
            .getTable("SmartDashboard")
            .getSubTable("TestChooser3")
            .getEntry("selected")
            .setString("Option 2");

    SmartDashboard.updateValues();

    assertTimeoutPreemptively(Duration.ofSeconds(1),
            (Executable) listenerCalled::get,
            "Listener was not called within allotted time");
  }

  @Test
  void testListenerRemove() {
    final CompletableFuture<Integer> listenerCalled = new CompletableFuture<>();
    BiConsumer<String, Integer> listener = (key, value) -> listenerCalled.complete(value);
    SendableChooser<Integer> chooser = createSendableChooser();

    chooser.addSelectionListener(listener);
    SmartDashboard.putData("TestChooser", chooser);
    chooser.removeSelectionListener(listener);
    NetworkTableInstance.getDefault()
            .getTable("SmartDashboard")
            .getSubTable("TestChooser")
            .getEntry("selected")
            .setString("Option 2");

    SmartDashboard.updateValues();

    assertThrows(TimeoutException.class, () -> {
      listenerCalled.get(1, TimeUnit.SECONDS);
    }, "Listener was called when it shouldn't have");
  }

  private static SendableChooser<Integer> createSendableChooser() {
    SendableChooser<Integer> chooser = new SendableChooser<>();
    SendableRegistry.remove(chooser);
    SmartDashboard.updateValues();
    chooser.setDefaultOption("Option 1 (default)", 1);
    chooser.addOption("Option 2", 2);
    chooser.addOption("Option 3", 3);
    return chooser;
  }
}
