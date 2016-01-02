/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package org.gazebosim.transport;

public interface PublisherRecord {
	String getTopic();
	String getHost();
	int getPort();
	String getMsgType();
	void connect(Connection conn);
}
