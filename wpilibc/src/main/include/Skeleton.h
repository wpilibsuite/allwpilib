/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SKELETON_H__
#define __SKELETON_H__

/**
 * Represents Skeleton data from a Kinect device connected to the 
 * Driver Station. See Getting Started with Microsoft Kinect for 
 * FRC and the Kinect for Windows SDK API reference for more information
 */
class Skeleton
{
	friend class Kinect;
public:
	typedef enum
	{
		HipCenter = 0,
		Spine = 1,
		ShoulderCenter = 2,
		Head = 3,
		ShoulderLeft = 4,
		ElbowLeft = 5,
		WristLeft = 6,
		HandLeft = 7,
		ShoulderRight = 8,
		ElbowRight = 9,
		WristRight = 10,
		HandRight = 11,
		HipLeft = 12,
		KneeLeft = 13,
		AnkleLeft = 14,
		FootLeft = 15,
		HipRight = 16,
		KneeRight = 17,
		AnkleRight = 18,
		FootRight = 19,
		JointCount = 20
	} JointTypes;

	typedef enum {kNotTracked, kInferred, kTracked} JointTrackingState;

	typedef struct
	{
		float x;
		float y;
		float z;
		JointTrackingState trackingState;
	} Joint;

	Joint GetHandRight() { return m_joints[HandRight]; }
	Joint GetHandLeft() { return m_joints[HandLeft]; }
	Joint GetWristRight() { return m_joints[WristRight]; }
	Joint GetWristLeft() { return m_joints[WristLeft]; }
	Joint GetElbowLeft() { return m_joints[ElbowLeft]; }
	Joint GetElbowRight() { return m_joints[ElbowRight]; }
	Joint GetShoulderLeft() { return m_joints[ShoulderLeft]; }
	Joint GetShoulderRight() { return m_joints[ShoulderRight]; }
	Joint GetShoulderCenter() { return m_joints[ShoulderCenter]; }
	Joint GetHead() { return m_joints[Head]; }
	Joint GetSpine() { return m_joints[Spine]; }
	Joint GetHipCenter() { return m_joints[HipCenter]; }
	Joint GetHipRight() { return m_joints[HipRight]; }
	Joint GetHipLeft() { return m_joints[HipLeft]; }
	Joint GetKneeLeft() { return m_joints[KneeLeft]; }
	Joint GetKneeRight() { return m_joints[KneeRight]; }
	Joint GetAnkleLeft() { return m_joints[AnkleLeft]; }
	Joint GetAnkleRight() { return m_joints[AnkleRight]; }
	Joint GetFootLeft() { return m_joints[FootLeft]; }
	Joint GetFootRight() { return m_joints[FootRight]; }
	Joint GetJointValue(JointTypes index) { return m_joints[index]; }

private:
	Joint m_joints[20];
};

#endif

