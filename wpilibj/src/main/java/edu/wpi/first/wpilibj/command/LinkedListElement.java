/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * An element that is in a LinkedList.
 */
class LinkedListElement {
  private LinkedListElement m_next;
  private LinkedListElement m_previous;
  private Command m_data;

  public LinkedListElement() {
  }

  public void setData(Command newData) {
    m_data = newData;
  }

  public Command getData() {
    return m_data;
  }

  public LinkedListElement getNext() {
    return m_next;
  }

  public LinkedListElement getPrevious() {
    return m_previous;
  }

  public void add(LinkedListElement listElement) {
    if (m_next == null) {
      m_next = listElement;
      m_next.m_previous = this;
    } else {
      m_next.m_previous = listElement;
      listElement.m_next = m_next;
      listElement.m_previous = this;
      m_next = listElement;
    }
  }

  @SuppressWarnings("PMD.EmptyIfStmt")
  public LinkedListElement remove() {
    if (m_previous == null && m_next == null) {
      // no-op
    } else if (m_next == null) {
      m_previous.m_next = null;
    } else if (m_previous == null) {
      m_next.m_previous = null;
    } else {
      m_next.m_previous = m_previous;
      m_previous.m_next = m_next;
    }
    LinkedListElement returnNext = m_next;
    m_next = null;
    m_previous = null;
    return returnNext;
  }
}
