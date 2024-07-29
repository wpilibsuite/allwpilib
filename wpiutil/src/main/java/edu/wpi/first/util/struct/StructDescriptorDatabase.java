// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import edu.wpi.first.util.struct.parser.ParseException;
import edu.wpi.first.util.struct.parser.ParsedDeclaration;
import edu.wpi.first.util.struct.parser.ParsedSchema;
import edu.wpi.first.util.struct.parser.Parser;
import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

/** Database of raw struct dynamic descriptors. */
public class StructDescriptorDatabase {
  /** Default constructor. */
  public StructDescriptorDatabase() {}

  /**
   * Adds a structure schema to the database. If the struct references other structs that have not
   * yet been added, it will not be valid until those structs are also added.
   *
   * @param name structure name
   * @param schema structure schema
   * @return Added struct dynamic descriptor
   * @throws BadSchemaException if schema invalid
   */
  public StructDescriptor add(String name, String schema) throws BadSchemaException {
    Parser parser = new Parser(schema);
    ParsedSchema parsed;
    try {
      parsed = parser.parse();
    } catch (ParseException e) {
      throw new BadSchemaException("parse error", e);
    }

    // turn parsed schema into descriptors
    StructDescriptor theStruct = m_structs.computeIfAbsent(name, StructDescriptor::new);
    theStruct.m_schema = schema;
    theStruct.m_fields.clear();
    boolean isValid = true;
    for (ParsedDeclaration decl : parsed.declarations) {
      StructFieldType type = StructFieldType.fromString(decl.typeString);
      int size = type.size;

      // bitfield checks
      if (decl.bitWidth != 0) {
        // only integer or boolean types are allowed
        if (!type.isInt && !type.isUint && type != StructFieldType.kBool) {
          throw new BadSchemaException(
              decl.name, "type " + decl.typeString + " cannot be bitfield");
        }

        // bit width cannot be larger than field size
        if (decl.bitWidth > (size * 8)) {
          throw new BadSchemaException(
              decl.name, "bit width " + decl.bitWidth + " exceeds type size");
        }

        // bit width must be 1 for booleans
        if (type == StructFieldType.kBool && decl.bitWidth != 1) {
          throw new BadSchemaException(decl.name, "bit width must be 1 for bool type");
        }

        // cannot combine array and bitfield (shouldn't parse, but double-check)
        if (decl.arraySize > 1) {
          throw new BadSchemaException(decl.name, "cannot combine array and bitfield");
        }
      }

      // struct handling
      StructDescriptor structDesc = null;
      if (type == StructFieldType.kStruct) {
        // recursive definitions are not allowed
        if (decl.typeString.equals(name)) {
          throw new BadSchemaException(decl.name, "recursive struct reference");
        }

        // cross-reference struct, creating a placeholder if necessary
        StructDescriptor aStruct =
            m_structs.computeIfAbsent(decl.typeString, StructDescriptor::new);

        // if the struct isn't valid, we can't be valid either
        if (aStruct.isValid()) {
          size = aStruct.getSize();
        } else {
          isValid = false;
        }

        // add to cross-references for when the struct does become valid
        aStruct.m_references.add(theStruct);
        structDesc = aStruct;
      }

      // create field
      StructFieldDescriptor fieldDesc =
          new StructFieldDescriptor(
              theStruct,
              decl.name,
              type,
              size,
              decl.arraySize,
              decl.bitWidth,
              decl.enumValues,
              structDesc);
      if (theStruct.m_fieldsByName.put(decl.name, fieldDesc) != null) {
        throw new BadSchemaException(decl.name, "duplicate field name");
      }

      theStruct.m_fields.add(fieldDesc);
    }

    theStruct.m_valid = isValid;
    Stack<StructDescriptor> stack = new Stack<>();
    if (isValid) {
      // we have all the info needed, so calculate field offset & shift
      theStruct.calculateOffsets(stack);
    } else {
      // check for circular reference
      if (!theStruct.checkCircular(stack)) {
        StringBuilder builder = new StringBuilder("circular struct reference: ");
        boolean first = true;
        for (StructDescriptor elem : stack) {
          if (!first) {
            builder.append(" <- ");
          } else {
            first = false;
          }
          builder.append(elem.getName());
        }
        throw new BadSchemaException(builder.toString());
      }
    }

    return theStruct;
  }

  /**
   * Finds a structure in the database by name.
   *
   * @param name structure name
   * @return struct descriptor, or null if not found
   */
  public StructDescriptor find(String name) {
    return m_structs.get(name);
  }

  private final Map<String, StructDescriptor> m_structs = new HashMap<>();
}
