// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib;

import com.google.protobuf.compiler.PluginProtos.CodeGeneratorResponse;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import com.google.protobuf.DescriptorProtos.FileDescriptorProto;
import com.google.protobuf.Descriptors;
import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.DescriptorValidationException;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.google.protobuf.Descriptors.FieldDescriptor.Type;
import com.google.protobuf.compiler.PluginProtos.CodeGeneratorRequest;

public final class ProtoCDllGenerator {
  private ProtoCDllGenerator() {
  }

  public static void main(String[] args) throws IOException, DescriptorValidationException {
    CodeGeneratorRequest request = CodeGeneratorRequest.parseFrom(System.in);

    Map<String, Descriptors.FileDescriptor> descriptors = new HashMap<>();

    Descriptors.FileDescriptor[] tmpArray = new Descriptors.FileDescriptor[0];

    for (FileDescriptorProto proto : request.getProtoFileList()) {
      // Make array of file descriptors that exists
      Descriptors.FileDescriptor[] depArray = descriptors.values().toArray(tmpArray);
      Descriptors.FileDescriptor desc = Descriptors.FileDescriptor.buildFrom(proto, depArray);
      descriptors.put(proto.getName(), desc);
    }

    // Filter to generated descriptors
    CodeGeneratorResponse.Builder response = CodeGeneratorResponse.newBuilder();

    for (String genFile : request.getFileToGenerateList()) {
      Descriptors.FileDescriptor desc = descriptors.get(genFile);

      for (Descriptor msg : desc.getMessageTypes()) {
        for (FieldDescriptor field : msg.getFields()) {
          if (field.getType() == Type.MESSAGE && !field.isRepeated()) {
            // If we have a nested non repeated field, we need a custom accessor
            String type = field.getMessageType().getFullName();
            type = "::" + type.replaceAll("\\.", "::");

            // Add definition
            response.addFileBuilder()
                .setName(desc.getName().replace("proto", "pb.h"))
                .setInsertionPoint("class_scope:" + msg.getFullName())
                .setContent("// Custom WPILib Accessor\n"
                    + "bool wpi_has_" + field.getName() + "() const;\n"
                    + "const " + type + "& wpi_" + field.getName() + "() const;\n");

            // Add implementation. As were in the cc file for the proto
            // we can just call straight through to the inline definitions
            response.addFileBuilder()
                .setName(desc.getName().replace("proto", "pb.cc"))
                .setInsertionPoint("namespace_scope")
                .setContent("// Custom WPILib Accessor\n"
                    + "bool " + msg.getName() + "::wpi_has_" + field.getName() + "() const { return has_"
                    + field.getName() + "(); }\n"
                    + "const " + type + "& " + msg.getName() + "::wpi_" + field.getName() + "() const { return "
                    + field.getName() + "(); }\n");
          }
        }
      }
    }
    response.build().writeTo(System.out);
  }
}
