/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Kinect.h"

#include "DriverStation.h"
//#include "NetworkCommunication/FRCComm.h"
//#include "NetworkCommunication/UsageReporting.h"
#include "Skeleton.h"
#include "HAL/cpp/Synchronized.h"
#include "WPIErrors.h"
#include <cstring>

#define kHeaderBundleID HALFRC_NetworkCommunication_DynamicType_Kinect_Header
#define kSkeletonExtraBundleID HALFRC_NetworkCommunication_DynamicType_Kinect_Extra1
#define kSkeletonBundleID HALFRC_NetworkCommunication_DynamicType_Kinect_Vertices1

Kinect *Kinect::_instance = NULL;

Kinect::Kinect() :
	m_recentPacketNumber(0),
	m_numberOfPlayers(0)
{
	AddToSingletonList();
	m_dataLock = initializeMutexNormal();

	HALReport(HALUsageReporting::kResourceType_Kinect, 0);
}

Kinect::~Kinect()
{
	takeMutex(m_dataLock);
	deleteMutex(m_dataLock);
}

/**
 * Get the one and only Kinect object
 * @returns pointer to a Kinect
 */
Kinect *Kinect::GetInstance()
{
	if (_instance == NULL)
		_instance = new Kinect();
	return _instance;
}

/**
 * Get the number of tracked players on the Kinect
 * @return the number of players being actively tracked
 */
int Kinect::GetNumberOfPlayers()
{
	UpdateData();
	return m_numberOfPlayers;
}

/**
 * Get the floor clip plane as defined in the Kinect SDK
 * @return The floor clip plane
 */
Kinect::Point4 Kinect::GetFloorClipPlane()
{
	UpdateData();
	return m_floorClipPlane;
}

/**
 * Get the gravity normal from the kinect as defined in the Kinect SDK
 * @return The gravity normal (w is ignored)
 */
Kinect::Point4 Kinect::GetGravityNormal()
{
	UpdateData();
	return m_gravityNormal;
}

/**
 * Get the skeleton data
 * Returns the detected skeleton data from the kinect as defined in the Kinect SDK
 * @param skeletonIndex Which of (potentially 2) skeletons to return. This is ignored in this implementation and
 * only a single skeleton is supported for the FRC release default gesture interpretation.
 * @return The current version of the skeleton object.
 */
Skeleton Kinect::GetSkeleton(int skeletonIndex)
{
	if (skeletonIndex <= 0 || skeletonIndex > kNumSkeletons)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "Skeleton index must be 1");
		return Skeleton();
	}
	UpdateData();
	return m_skeletons[skeletonIndex-1];
}

/**
 * Get the current position of the skeleton
 * @param skeletonIndex the skeleton to read from
 * @return the current position as defined in the Kinect SDK (w is ignored)
 */
Kinect::Point4 Kinect::GetPosition(int skeletonIndex)
{
	if (skeletonIndex <= 0 || skeletonIndex > kNumSkeletons)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "Skeleton index must be 1");
		return Point4();
	}
	UpdateData();
	return m_position[skeletonIndex-1];
}

/**
 * Get the quality of the skeleton.
 * Quality masks are defined in the SkeletonQuality enum
 * @param skeletonIndex the skeleton to read from
 * @return the quality value as defined in the Kinect SDK
 */
uint32_t Kinect::GetQuality(int skeletonIndex)
{
	if (skeletonIndex <= 0 || skeletonIndex > kNumSkeletons)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "Skeleton index must be 1");
		return kClippedRight | kClippedLeft | kClippedTop | kClippedBottom;
	}
	UpdateData();
	return m_quality[skeletonIndex-1];
}

/**
 * Get the TrackingState of the skeleton.
 * Tracking states are defined in the SkeletonTrackingState enum
 * @param skeletonIndex the skeleton to read from
 * @return the tracking state value as defined in the Kinect SDK
 */
Kinect::SkeletonTrackingState Kinect::GetTrackingState(int skeletonIndex)
{
	if (skeletonIndex <= 0 || skeletonIndex > kNumSkeletons)
	{
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "Skeleton index must be 1");
		return kNotTracked;
	}
	UpdateData();
	return m_trackingState[skeletonIndex-1];
}

/**
 * Check for an update of new data from the Driver Station
 * This will read the new values and update the data structures in this class.
 */
void Kinect::UpdateData()
{
	Synchronized sync(m_dataLock);
	uint32_t packetNumber = DriverStation::GetInstance()->GetPacketNumber();
	if (m_recentPacketNumber != packetNumber)
	{
		m_recentPacketNumber = packetNumber;
		int retVal = HALGetDynamicControlData(kHeaderBundleID, m_rawHeader, sizeof(m_rawHeader), 5);
		if(retVal == 0)
		{
			m_numberOfPlayers = (int)m_rawHeader[13];
			memcpy(&m_floorClipPlane.x, &m_rawHeader[18], 4);
			memcpy(&m_floorClipPlane.y, &m_rawHeader[22], 4);
			memcpy(&m_floorClipPlane.z, &m_rawHeader[26], 4);
			memcpy(&m_floorClipPlane.w, &m_rawHeader[30], 4);
			memcpy(&m_gravityNormal.x, &m_rawHeader[34], 4);
			memcpy(&m_gravityNormal.y, &m_rawHeader[38], 4);
			memcpy(&m_gravityNormal.z, &m_rawHeader[42], 4);
		}

		retVal = HALGetDynamicControlData(kSkeletonExtraBundleID, m_rawSkeletonExtra, sizeof(m_rawSkeletonExtra), 5);
		if(retVal == 0)
		{
			memcpy(&m_position[0].x, &m_rawSkeletonExtra[22], 4);
			memcpy(&m_position[0].y, &m_rawSkeletonExtra[26], 4);
			memcpy(&m_position[0].z, &m_rawSkeletonExtra[30], 4);
			memcpy(&m_quality[0], &m_rawSkeletonExtra[34], 4);
			memcpy(&m_trackingState[0], &m_rawSkeletonExtra[38], 4);
		}

		retVal = HALGetDynamicControlData(kSkeletonBundleID, m_rawSkeleton, sizeof(m_rawSkeleton), 5);
		if(retVal == 0)
		{
			for(int i=0; i < Skeleton::JointCount; i++)
			{
				memcpy(&m_skeletons[0].m_joints[i].x, &m_rawSkeleton[i*12+2], 4);
				memcpy(&m_skeletons[0].m_joints[i].y, &m_rawSkeleton[i*12+6], 4);
				memcpy(&m_skeletons[0].m_joints[i].z, &m_rawSkeleton[i*12+10], 4);
				m_skeletons[0].m_joints[i].trackingState = (Skeleton::JointTrackingState)m_rawSkeletonExtra[i+2];
			}
		}

		// TODO: Read skeleton #2
	}
}
