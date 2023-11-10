package edu.wpi.first.wpilibj.glass;

import java.nio.ByteBuffer;

import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.wpilibj.util.Color8Bit;

public class CanvasLine2d {
    public final double m_x1;
    public final double m_y1;
    public final double m_x2;
    public final double m_y2;
    public final double m_weight;
    public final Color8Bit m_color;
    public final int m_zOrder;

    public CanvasLine2d(double x1, double y1, double x2, double y2, double weight, Color8Bit color, int zOrder) {
        m_x1 = x1;
        m_y1 = y1;
        m_x2 = x2;
        m_y2 = y2;

        m_weight = weight;
        m_color = color;
        m_zOrder = zOrder;
    }

    public static class AStruct implements Struct<CanvasLine2d> {
        @Override
        public Class<CanvasLine2d> getTypeClass() {
            return CanvasLine2d.class;
        }

        @Override
        public String getTypeString() {
            return "struct:CanvasLine2d";
        }

        @Override
        public int getSize() {
            return kSizeDouble * 5 + kSizeInt8 * 3;
        }

        @Override
        public String getSchema() {
            return "double x1;double y1;double x2;double y2;double weight;int8 r;int8 g;int8 b; int32 zOrder";
        }

        @Override
        public CanvasLine2d unpack(ByteBuffer bb) {
            double x1 = bb.getDouble();
            double y1 = bb.getDouble();
            double x2 = bb.getDouble();
            double y2 = bb.getDouble();
            double weight = bb.getDouble();
            int r = bb.get();
            int g = bb.get();
            int b = bb.get();
            int zOrder = bb.getInt();
            return new CanvasLine2d(x1, y1, x2, y2, weight, new Color8Bit(r, g, b), zOrder);
        }

        @Override
        public void pack(ByteBuffer bb, CanvasLine2d value) {
            bb.putDouble(value.m_x1);
            bb.putDouble(value.m_y1);
            bb.putDouble(value.m_x2);
            bb.putDouble(value.m_y2);
            bb.putDouble(value.m_weight);
            bb.put((byte) value.m_color.red);
            bb.put((byte) value.m_color.blue);
            bb.put((byte) value.m_color.green);
            bb.putInt(value.m_zOrder);
        }
    }

    public static final AStruct struct = new AStruct();
}
