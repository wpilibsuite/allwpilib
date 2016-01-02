/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 *
 * @author Greg
 */
class LinkedListElement {
  private LinkedListElement next;
  private LinkedListElement previous;
  private Command data;

  public LinkedListElement() {}

  public void setData(Command newData) {
    data = newData;
  }

  public Command getData() {
    return data;
  }

  public LinkedListElement getNext() {
    return next;
  }

  public LinkedListElement getPrevious() {
    return previous;
  }

  public void add(LinkedListElement l) {
    if (next == null) {
      next = l;
      next.previous = this;
    } else {
      next.previous = l;
      l.next = next;
      l.previous = this;
      next = l;
    }
  }

  public LinkedListElement remove() {
    if (previous == null && next == null) {

    } else if (next == null) {
      previous.next = null;
    } else if (previous == null) {
      next.previous = null;
    } else {
      next.previous = previous;
      previous.next = next;
    }
    LinkedListElement n = next;
    next = null;
    previous = null;
    return n;
  }
}
