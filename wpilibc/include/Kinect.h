/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __KINECT_H__
#define __KINECT_H__

#include "SensorBase.h"
#include "Skeleton.h"

#include "HAL/Semaphore.h"

#define kNumSkeletons 1

/**
 * Handles raw data input from the FRC Kinect Server
 * when used with a Kinect device connected to the Driver Station.
 * Each time a value is requested the most recent value is returned.
 * See Getting Started with Microsoft Kinect for FRC and the Kinect
 * for Windows SDK API reference for more information
 */
class Kinect : public SensorBase
{
public:
	typedef enum {kNotTracked, kPositionOnly, kTracked} SkeletonTrackingState;
	typedef enum {kClippedRight = 1, kClippedLeft = 2, kClippedTop = 4, kClippedBottom = 8} SkeletonQuality;
	typedef struct
	{
		float x;
		float y;
		float z;
		float w;
	} Point4;

	int GetNumberOfPlayers();
	Point4 GetFloorClipPlane();
	Point4 GetGravityNormal();
	Skeleton GetSkeleton(int skeletonIndex = 1);
	Point4 GetPosition(int skeletonIndex = 1);
	uint32_t GetQuality(int skeletonIndex = 1);
	SkeletonTrackingState GetTrackingState(int skeletonIndex = 1);

	static Kinect *GetInstance();

private:
	Kinect();
	~Kinect();
	void UpdateData();

	DISALLOW_COPY_AND_ASSIGN(Kinect);

	uint32_t m_recentPacketNumber;
	MUTEX_ID m_dataLock;
	int m_numberOfPlayers;
	Point4 m_floorClipPlane;
	Point4 m_gravityNormal;
	Point4 m_position[kNumSkeletons];
	uint32_t m_quality[kNumSkeletons];
	SkeletonTrackingState m_trackingState[kNumSkeletons];
	Skeleton m_skeletons[kNumSkeletons];

	// TODO: Include structs for this data format (would be clearer than 100 magic numbers)
	char m_rawHeader[46];
	char m_rawSkeletonExtra[42];
	char m_rawSkeleton[242];

	static Kinect *_instance;
};

#endif

