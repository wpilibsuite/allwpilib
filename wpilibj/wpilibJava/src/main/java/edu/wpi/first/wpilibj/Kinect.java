/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;
import edu.wpi.first.wpilibj.communication.FRCControl;
import edu.wpi.first.wpilibj.communication.UsageReporting;

/**
 * @author bradmiller
 * Handles raw data input from the FRC Kinect Server
 * when used with a Kinect device connected to the Driver Station.
 * Each time a value is requested the most recent value is returned.
 * See Getting Started with Microsoft Kinect for FRC and the Kinect
 * for Windows SDK API reference for more information
 *
 */
public class Kinect {

    private static Kinect m_instance;

    /**
    * Gets an instance of the Kinect device
    *
    * @return The Kinect.
    */
    public static synchronized Kinect getInstance() {
        if(m_instance == null)
            m_instance = new Kinect();
        return m_instance;
    }

    /**
     * A set of 4 coordinates (x,y,z,w) bundled into one object
     */
    public class Point4 {
        public float x, y, z, w;

        public float getX() {
            return x;
        }
        public float getY() {
            return y;
        }
        public float getZ() {
            return z;
        }
        public float getW() {
            return w;
        }

        public int size() {
            return 16;
        }
    }

    static class header_t extends Structure {

        byte[] version = new byte[11];
        byte players;
        int flags;
        float[] floorClipPlane = new float[4];
        float[] gravityNormalVector = new float[3];

        final static int size = 44;

        header_t(Pointer backingMemory) {
            useMemory(backingMemory);
        }

        public void read() {
            backingNativeMemory.getBytes(0, version, 0, version.length);
            players = backingNativeMemory.getByte(11);
            flags = backingNativeMemory.getInt(12);
            backingNativeMemory.getFloats(16, floorClipPlane, 0, floorClipPlane.length);
            backingNativeMemory.getFloats(32, gravityNormalVector, 0, gravityNormalVector.length);
        }

        public void write() {
            backingNativeMemory.setBytes(0, version, 0, version.length);
            backingNativeMemory.setByte(11, players);
            backingNativeMemory.setInt(12, flags);
            backingNativeMemory.setFloats(16, floorClipPlane, 0, floorClipPlane.length);
            backingNativeMemory.setFloats(32, gravityNormalVector, 0, gravityNormalVector.length);
        }

        public int size() {
            return size;
        }
    }

    static class skeletonExtra_t extends Structure {
        byte[] trackingState = new byte[20];
        float[] position = new float[3];
        int quality;
        int trackState;

        final static int size = 40;

        skeletonExtra_t(Pointer backingMemory) {
            useMemory(backingMemory);
        }

        public void read() {
            backingNativeMemory.getBytes(0, trackingState, 0, trackingState.length);
            backingNativeMemory.getFloats(20, position, 0, position.length);
            quality = backingNativeMemory.getInt(32);
            trackState = backingNativeMemory.getInt(36);
        }

        public void write() {
            backingNativeMemory.setBytes(0, trackingState, 0, trackingState.length);
            backingNativeMemory.setFloats(20, position, 0, position.length);
            backingNativeMemory.setInt(32, quality);
            backingNativeMemory.setInt(36, trackState);
        }

        public int size() {
            return size;
        }
    }

    static class skeleton_t extends Structure {
        float[] vertices = new float[60];

        final static int size = 240;

        skeleton_t(Pointer backingMemory) {
            useMemory(backingMemory);
        }

        public void read() {
            backingNativeMemory.getFloats(0, vertices, 0, vertices.length);
        }

        public void write() {
            backingNativeMemory.setFloats(0, vertices, 0, vertices.length);
        }

        public int size() {
            return size;
        }
    }

    class header_block_t extends FRCControl.DynamicControlData {
        byte size = 45;
        byte id = kHeaderBlockID;
        header_t data;

        {
            allocateMemory();
            data = new header_t(
                new Pointer(backingNativeMemory.address().toUWord().toPrimitive() + 2,
                            header_t.size));
        }

        public void read() {

            size = backingNativeMemory.getByte(0);
            id = backingNativeMemory.getByte(1);
            data.read();
        }

        public void write() {
            backingNativeMemory.setByte(0, size);
            backingNativeMemory.setByte(1, id);
            data.write();
        }

        public int size() {
            return 46;
        }

        public void copy(header_block_t dest) {
            write();
            Pointer.copyBytes(backingNativeMemory, 0, dest.backingNativeMemory, 0, size());
            dest.read();
        }
    }

    class skeletonExtra_block_t extends FRCControl.DynamicControlData {
        byte size = 41;
        byte id = kSkeletonExtraBlockID;
        skeletonExtra_t data;

        {
            allocateMemory();
            data = new skeletonExtra_t(
                new Pointer(backingNativeMemory.address().toUWord().toPrimitive() + 2,
                            skeletonExtra_t.size));
        }

        public void read() {

            size = backingNativeMemory.getByte(0);
            id = backingNativeMemory.getByte(1);
            data.read();
        }

        public void write() {
            backingNativeMemory.setByte(0, size);
            backingNativeMemory.setByte(1, id);
            data.write();
        }

        public int size() {
            return 42;
        }

        public void copy(skeletonExtra_block_t dest) {
            write();
            Pointer.copyBytes(backingNativeMemory, 0, dest.backingNativeMemory, 0, size());
            dest.read();
        }
    }

    class skeleton_block_t extends FRCControl.DynamicControlData {
        byte size = -15;    //temporary hack for 241
        byte id = kSkeletonBlockID;
        skeleton_t data;

        {
            allocateMemory();
            data = new skeleton_t(
                new Pointer(backingNativeMemory.address().toUWord().toPrimitive() + 2,
                            skeleton_t.size));
        }

        public void read() {
            size = backingNativeMemory.getByte(0);
            id = backingNativeMemory.getByte(1);
            data.read();
        }

        public void write() {
            backingNativeMemory.setByte(0, size);
            backingNativeMemory.setByte(1, id);
            data.write();
        }

        public int size() {
            return 242;
        }

        public void copy(skeleton_block_t dest) {
            write();
            Pointer.copyBytes(backingNativeMemory, 0, dest.backingNativeMemory, 0, size());
            dest.read();
        }
    }

    static final byte kHeaderBlockID = 19;
    static final byte kSkeletonExtraBlockID = 20;
    static final byte kSkeletonBlockID = 21;

    header_block_t m_headerData;
    skeletonExtra_block_t m_skeletonExtraData;
    skeleton_block_t m_skeletonData;
    boolean m_headerValid = false;
    boolean m_skeletonExtraValid = false;
    boolean m_skeletonValid = false;
    final Object m_headerDataSemaphore;
    final Object m_skeletonExtraDataSemaphore;
    final Object m_skeletonDataSemaphore;
    int m_recentPacketNumber = 0;

    /**
    * Kinect constructor.
    *
    * This is only called once on the first call of getInstance()
    */
    Kinect() {
        m_headerData = new header_block_t();
        m_skeletonExtraData = new skeletonExtra_block_t();
        m_skeletonData = new skeleton_block_t();
        m_headerDataSemaphore = new Object();
        m_skeletonExtraDataSemaphore = new Object();
        m_skeletonDataSemaphore = new Object();

        UsageReporting.report(UsageReporting.kResourceType_Kinect, 0);
    }
    header_block_t tempHeaderData = new header_block_t();
    skeletonExtra_block_t tempSkeletonExtraData = new skeletonExtra_block_t();
    skeleton_block_t tempSkeletonData = new skeleton_block_t();


    /**
    * Called by the other Kinect functions to check for the latest data
    * This function will update the internal data structures
    * with the most recent Kinect input
    */
    void updateData() {
        int retVal;

        if (m_recentPacketNumber !=  DriverStation.getInstance().getPacketNumber()) {
            m_recentPacketNumber = DriverStation.getInstance().getPacketNumber();
            synchronized (m_headerDataSemaphore) {
                retVal = FRCControl.getDynamicControlData(kHeaderBlockID, tempHeaderData, tempHeaderData.size(), 5);
                if (retVal == 0) {
                    tempHeaderData.copy(m_headerData);
                    m_headerValid = true;
                } else {
                    m_headerValid = false;
                }
            }

            synchronized (m_skeletonExtraDataSemaphore) {
                retVal = FRCControl.getDynamicControlData(kSkeletonExtraBlockID, tempSkeletonExtraData, tempSkeletonExtraData.size(), 5);
                if (retVal == 0) {
                    tempSkeletonExtraData.copy(m_skeletonExtraData);
                    m_skeletonExtraValid = true;
                } else {
                    m_skeletonExtraValid = false;
                }
            }

            synchronized (m_skeletonDataSemaphore) {
                retVal = FRCControl.getDynamicControlData(kSkeletonBlockID, tempSkeletonData, tempSkeletonData.size(), 5);
                if (retVal == 0) {
                    tempSkeletonData.copy(m_skeletonData);
                    m_skeletonValid = true;
                } else {
                    m_skeletonValid = false;
                }
            }
        }
    }

    /**
    * Query the number of players detected by the Kinect
    *
    * @return The current number of players
    */
    public int getNumberOfPlayers() {
        updateData();
        if (!m_headerValid) {
            return 0;
        }
        synchronized (m_headerDataSemaphore) {
            return (int)m_headerData.data.players;
        }
    }

    /**
    * Retrieve the FloorClipPlane from the Kinect device
    *
    * @return The FloorClipPlane
    */
    public Point4 getFloorClipPlane() {
        updateData();
        Point4 tempClipPlane = new Point4();

        if (!m_headerValid) {
            return tempClipPlane;
        }

        synchronized(m_headerDataSemaphore) {
            tempClipPlane.x = m_headerData.data.floorClipPlane[0];
            tempClipPlane.y = m_headerData.data.floorClipPlane[1];
            tempClipPlane.z = m_headerData.data.floorClipPlane[2];
            tempClipPlane.w = m_headerData.data.floorClipPlane[3];
        }
        return tempClipPlane;
    }

    /**
    * Retrieve the GravityNormal vector from the Kinect device
    * The w value returned from this method is always 0
    * @return The GravityNormal vector
    */
    public Point4 getGravityNormal() {
        updateData();
        Point4 tempGravityNormal = new Point4();

        if (!m_headerValid) {
            return tempGravityNormal;
        }

        synchronized(m_headerDataSemaphore) {
            tempGravityNormal.x = m_headerData.data.gravityNormalVector[0];
            tempGravityNormal.y = m_headerData.data.gravityNormalVector[1];
            tempGravityNormal.z = m_headerData.data.gravityNormalVector[2];
            tempGravityNormal.w = 0;
        }
        return tempGravityNormal;
    }

    /**
    * Query the position of the detected skeleton
    * The w value returned from this method is always 1
    * @return The position of the skeleton
    */
    public Point4 getPosition() {
        updateData();
        Point4 tempPosition = new Point4();

        if (!m_skeletonExtraValid) {
            return tempPosition;
        }

        synchronized(m_headerDataSemaphore) {
            tempPosition.x = m_skeletonExtraData.data.position[0];
            tempPosition.y = m_skeletonExtraData.data.position[1];
            tempPosition.z = m_skeletonExtraData.data.position[2];
            tempPosition.w = 1;
        }
        return tempPosition;
    }

    /**
    * Retrieve the detected skeleton from the Kinect device
    *
    * @return The skeleton
    */
    public Skeleton getSkeleton() {
        updateData();
        Skeleton tempSkeleton = new Skeleton();

        if (!m_skeletonValid) {
            return tempSkeleton;
        }

        synchronized (m_skeletonDataSemaphore) {
            for(int i=0; i<20; i++) {
                tempSkeleton.skeleton[i].x = m_skeletonData.data.vertices[i*3];
                tempSkeleton.skeleton[i].y = m_skeletonData.data.vertices[i*3+1];
                tempSkeleton.skeleton[i].z = m_skeletonData.data.vertices[i*3+2];
            }
        }

        synchronized (m_skeletonExtraDataSemaphore) {
            for(int i=0; i<20; i++) {
                tempSkeleton.skeleton[i].trackingState = m_skeletonExtraData.data.trackingState[i];
            }
            switch(m_skeletonExtraData.data.trackState) {
            case 0:
                tempSkeleton.trackState = Skeleton.tTrackState.kNotTracked;
                break;
            case 1:
                tempSkeleton.trackState = Skeleton.tTrackState.kPositionOnly;
                break;
            case 2:
                tempSkeleton.trackState = Skeleton.tTrackState.kTracked;
                break;
            }

        }
        return tempSkeleton;
    }
}
