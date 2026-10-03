// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// Run with: node --test ntcore/src/test/js/WebServerPageTest.js
const assert = require("node:assert/strict");
const fs = require("node:fs");
const path = require("node:path");
const { test } = require("node:test");
const vm = require("node:vm");

const PAGE = fs.readFileSync(path.join(__dirname,
  "../../main/native/cpp/WebServerPage.hpp"), "utf8");
const CONTEXT = vm.createContext({ TextDecoder, Uint8Array, DataView });
vm.runInContext(PAGE.match(/<script>([\s\S]*?)<\/script>/)[1], CONTEXT);
const { decodeMessages, formatValue, buildTree, createStructDatabase,
  parseStructSchema, topicValueNode } = CONTEXT;

function decodeValue(bytes) {
  // [topic ID, timestamp, type, value]; value decoding is independent of type.
  return decodeMessages(Uint8Array.from([0x94, 1, 0, 0, ...bytes]).buffer)[0][3];
}

test("decodes NT scalar encodings and preserves 64-bit integers", () => {
  const cases = [
    [[0], 0], [[0x7f], 127], [[0xff], -1], [[0xe0], -32],
    [[0xc2], false], [[0xc3], true],
    [[0xcc, 255], 255], [[0xcd, 1, 0], 256],
    [[0xce, 1, 0, 0, 0], 16777216],
    [[0xd0, 0x80], -128], [[0xd1, 0x80, 0], -32768],
    [[0xd2, 0x80, 0, 0, 0], -2147483648],
    [[0xcf, 0x7f, ...Array(7).fill(255)], 9223372036854775807n],
    [[0xd3, 0x80, ...Array(7).fill(0)], -9223372036854775808n],
    [[0xca, 0x3f, 0xc0, 0, 0], 1.5],
    [[0xcb, 0x3f, 0xf8, 0, 0, 0, 0, 0, 0], 1.5],
    [[0xca, 0x7f, 0x80, 0, 0], Infinity],
    [[0xca, 0xff, 0x80, 0, 0], -Infinity],
    [[0xca, 0x7f, 0xc0, 0, 0], NaN],
  ];
  for (const [bytes, expected] of cases) {
    assert.equal(decodeValue(bytes), expected);
  }
});

test("decodes strings, raw data, and arrays with every length encoding", () => {
  for (const prefix of [[0xa2], [0xd9, 2], [0xda, 0, 2], [0xdb, 0, 0, 0, 2]]) {
    assert.equal(decodeValue([...prefix, 0xc2, 0xb5]), "µ");
  }
  for (const prefix of [[0xc4, 2], [0xc5, 0, 2], [0xc6, 0, 0, 0, 2]]) {
    assert.deepEqual(decodeValue([...prefix, 0, 255]), new Uint8Array([0, 255]));
  }
  for (const prefix of [[0x92], [0xdc, 0, 2], [0xdd, 0, 0, 0, 2]]) {
    assert.deepEqual(Array.from(decodeValue([...prefix, 0xc3, 0xc2])), [true, false]);
    assert.deepEqual(Array.from(decodeValue([...prefix, 0xa1, 65, 0xa1, 66])), ["A", "B"]);
  }
});

test("decodes concatenated messages, including timestamp replies", () => {
  const messages = decodeMessages(Uint8Array.from([
    0x94, 1, 0, 2, 42, 0x94, 0xff, 0, 2, 0
  ]).buffer);
  assert.deepEqual(structuredClone(messages), [[1, 0, 2, 42], [-1, 0, 2, 0]]);
});

test("rejects truncated, unsupported, and excessively nested messages", () => {
  for (const bytes of [
    [0xcb, 0], [0xd9, 5, 65], [0xc4, 255], [0xdd, 255, 255, 255, 255],
    [0x91, 0x91, 0], [0xc0], [0x80],
  ]) {
    assert.throws(() => decodeValue(bytes));
  }
  assert.throws(() => decodeMessages(new Uint8Array([0x91, 1]).buffer));
});

test("formats values without rounding integers or hiding non-finite numbers", () => {
  assert.equal(formatValue(9223372036854775807n), "9223372036854775807");
  assert.equal(formatValue([true, -9223372036854775808n, Infinity, NaN]),
    "[true, -9223372036854775808, Infinity, NaN]");
  assert.equal(formatValue("<script>\nµ"), '"<script>\\nµ"');
  assert.equal(formatValue(new Uint8Array([0, 127, 255])), "3 bytes: 00 7f ff");
  assert.match(formatValue(undefined), /Waiting/);
  assert.match(formatValue("x".repeat(10000)), / …$/);
  assert.match(formatValue(Array(1000).fill(1)), /, …\]$/);
  assert.match(formatValue(new Uint8Array(1000)), /^1000 bytes: .* …$/);
});

test("builds a filtered hierarchy without conflating topic paths", () => {
  const topics = ["a", "/a", "/a/b", "/a/", "//a", "/__proto__/x", "/Other"]
    .map((name, id) => ({ name, id }));
  const root = buildTree(topics, "");
  assert.equal(root.children.get("a").topic.name, "a");
  const absolute = root.children.get("");
  assert.equal(absolute.children.get("a").topic.name, "/a");
  assert.equal(absolute.children.get("a").children.get("b").topic.name, "/a/b");
  assert.equal(absolute.children.get("a").children.get("").topic.name, "/a/");
  assert.equal(absolute.children.get("").children.get("a").topic.name, "//a");
  assert.equal(absolute.children.get("__proto__").children.get("x").topic.name,
    "/__proto__/x");
  const filtered = buildTree(topics, "OTHER").children.get("");
  assert.equal(filtered.children.size, 1);
  assert.equal(filtered.children.get("Other").topic.name, "/Other");
  assert.equal(buildTree(topics, "missing").children.size, 0);
});

function database(entries) {
  const schemas = createStructDatabase();
  for (const [name, schema] of entries) schemas.set(name, schema);
  return schemas;
}

function packed(...fields) {
  const bytes = new Uint8Array(fields.reduce((size, [, length]) => size + length, 0));
  const view = new DataView(bytes.buffer);
  let offset = 0;
  for (const [method, length, value] of fields) {
    view[method](offset, value, true);
    offset += length;
  }
  return bytes;
}

function structValue(schemas, name, value) {
  return topicValueNode({ type: `struct:${name}`, value }, schemas);
}

function field(node, name) {
  return node.children().find(child => child.name === name).node;
}

test("decodes nested structs and arrays using unaligned little-endian layouts", () => {
  const schemas = database([
    ["Pose", "Translation translation; Rotation rotation"],
    ["Translation", "double x; double y;"],
    ["Rotation", "double value"],
    ["Sample", "bool valid; Pose poses[2]; int16 ids[2]"],
  ]);
  const poseBytes = packed(...[1.25, -2.5, 3].map(value => ["setFloat64", 8, value]));
  const bytes = Uint8Array.from([1, ...poseBytes, ...poseBytes, 0xff, 0xff, 0, 0x80]);
  const sample = structValue(schemas, "Sample", bytes);
  assert.equal(sample.text, "Sample");
  assert.equal(schemas.get("Sample").size, 53);
  assert.equal(field(sample, "valid").text, "true");
  const poses = field(sample, "poses");
  assert.equal(poses.text, "Pose[2]");
  for (const { node } of poses.children()) {
    assert.equal(field(field(node, "translation"), "x").text, "1.25");
    assert.equal(field(field(node, "translation"), "y").text, "-2.5");
    assert.equal(field(field(node, "rotation"), "value").text, "3");
  }
  assert.equal(field(field(sample, "ids"), "[0]").text, "-1");
  assert.equal(field(field(sample, "ids"), "[1]").text, "-32768");
  for (const type of ["Pose[]", "Pose[2]"]) {
    const array = structValue(schemas, type, Uint8Array.from([...poseBytes, ...poseBytes]));
    assert.equal(array.text, "Pose[2]");
    assert.equal(array.children().length, 2);
  }
  assert.equal(structValue(schemas, "Pose[]", new Uint8Array()).children().length, 0);
});

test("decodes all numeric struct primitives without losing integer precision", () => {
  const cases = [
    ["int8", "setInt8", 1, -128], ["uint8", "setUint8", 1, 255],
    ["int16", "setInt16", 2, -32768], ["uint16", "setUint16", 2, 65535],
    ["int32", "setInt32", 4, -2147483648], ["uint32", "setUint32", 4, 4294967295],
    ["int64", "setBigInt64", 8, -9223372036854775808n],
    ["uint64", "setBigUint64", 8, 18446744073709551615n],
    ["float", "setFloat32", 4, 1.25], ["float32", "setFloat32", 4, Infinity],
    ["double", "setFloat64", 8, -2.5], ["float64", "setFloat64", 8, NaN],
  ];
  const schemas = database([["Numbers", cases.map(([type], i) => `${type} v${i}`).join(";")]]);
  const node = structValue(schemas, "Numbers", packed(...cases.map(([, ...rest]) => rest)));
  cases.forEach(([, , , value], i) => assert.equal(field(node, `v${i}`).text, String(value)));
});

test("packs bitfields by storage width and sign extends signed fields", () => {
  const schemas = database([["Bits",
    "uint16 a:4; int16 b:5; bool c:1; int16 d:7; uint8 tail; bool ready"]]);
  const node = structValue(schemas, "Bits", new Uint8Array([0xfa, 3, 0x7e, 0, 255, 128]));
  const expected = { a: "10", b: "-1", c: "true", d: "-2", tail: "255", ready: "true" };
  for (const [name, value] of Object.entries(expected)) assert.equal(field(node, name).text, value);
  schemas.set("Bits", "bool a:1; bool b:1; int8 c:2; int16 d:1");
  const mixed = structValue(schemas, "Bits", new Uint8Array([13, 1, 0]));
  assert.equal(field(mixed, "a").text, "true");
  assert.equal(field(mixed, "b").text, "false");
  assert.equal(field(mixed, "c").text, "-1");
  assert.equal(field(mixed, "d").text, "-1");
  schemas.set("Bits", "int64 a:63; bool b:1; uint64 c:64");
  const wide = structValue(schemas, "Bits", new Uint8Array(16).fill(255));
  assert.equal(field(wide, "a").text, "-1");
  assert.equal(field(wide, "b").text, "true");
  assert.equal(field(wide, "c").text, "18446744073709551615");
});

test("decodes enums, UTF-8 char arrays, and schema values as text", () => {
  const schemas = database([["Text",
    "enum {Off=0, On=1,} uint8 mode; {Negative=-1} int8 codes[2]; char name[6]; char c"]]);
  const node = structValue(schemas, "Text", new Uint8Array([1, 255, 7, 0xc2, 0xb5, 0, 65, 0, 0, 90]));
  assert.equal(field(node, "mode").text, "On");
  assert.equal(field(field(node, "codes"), "[0]").text, "Negative");
  assert.equal(field(field(node, "codes"), "[1]").text, "<7>");
  assert.equal(field(node, "name").text, '"µ\\u0000A"');
  assert.equal(field(node, "c").text, '"Z"');
  schemas.set("Text", "char text[2]");
  assert.equal(field(structValue(schemas, "Text", new Uint8Array([65, 0xc2])), "text").text, '"A"');
  const schema = topicValueNode({ type: "structschema", value: new TextEncoder().encode("double x; double y;") }, schemas);
  assert.equal(schema.text, '"double x; double y;"');
  assert.equal(schema.children, undefined);
});

test("invalidates dependent layouts for late, changed, and removed schemas", () => {
  const schemas = database([["Parent", "Child child"]]);
  const bytes = new Uint8Array([42]);
  assert.match(structValue(schemas, "Parent", bytes).text, /Missing schema 'Child'/);
  schemas.set("Child", "uint8 value");
  assert.equal(field(field(structValue(schemas, "Parent", bytes), "child"), "value").text, "42");
  schemas.set("Child", "uint16 value");
  assert.match(structValue(schemas, "Parent", bytes).text, /Invalid struct size/);
  const changed = structValue(schemas, "Parent", new Uint8Array([0, 1]));
  assert.equal(field(field(changed, "child"), "value").text, "256");
  schemas.remove("Child");
  assert.match(structValue(schemas, "Parent", bytes).text, /Missing schema/);
  schemas.clear();
  assert.match(structValue(schemas, "Parent", bytes).text, /Missing schema 'Parent'/);
});

test("rejects invalid schemas and cyclic definitions without decoding bad data", () => {
  for (const schema of ["double x:2", "bool x:2", "int8 x:9", "int8 x:0",
    "int8 x[-1]", "int8 x[0]", "int8 x[2]:1", "int8 x[9007199254740992]",
    "int8 x; int8 x", "enum int8 x", "enum {a=1 b=2} int8 x",
    "enum {a=9223372036854775808} int8 x", "enum{a=1} double x", "int8 x@", "int8"]) {
    assert.throws(() => parseStructSchema(schema), undefined, schema);
  }
  const schemas = database([["A", "B b"], ["B", "A a"]]);
  const cycle = structValue(schemas, "A", new Uint8Array(1));
  assert.equal(cycle.children, undefined);
  assert.match(cycle.text, /Circular schema/);
  schemas.set("B", "double x");
  for (const type of ["A", "A[]", "A[2]"]) {
    const bad = structValue(schemas, type, new Uint8Array(7));
    assert.equal(bad.children, undefined);
    assert.match(bad.text, /7 bytes:.*Invalid struct/);
  }
  assert.match(structValue(schemas, "A[2]", new Uint8Array(8)).text, /Invalid struct array size/);
  assert.match(structValue(schemas, "A", new Uint8Array(9)).text, /Invalid struct size/);
  schemas.set("Huge", "A values[9007199254740991]");
  assert.match(structValue(schemas, "Huge", new Uint8Array()).text, /too large/);
});

test("handles empty structs and bounds large previews without eager decoding", () => {
  const schemas = database([["Empty", " ; ; "], ["Item", "uint8 value"],
    ["Holder", "Empty empty[1000000]; Item items[100]"]]);
  assert.equal(structValue(schemas, "Empty", new Uint8Array()).children().length, 0);
  assert.match(structValue(schemas, "Empty[]", new Uint8Array()).text, /Invalid struct array size/);
  const large = structValue(schemas, "Holder", new Uint8Array(100));
  assert.equal(field(large, "empty").text, "Empty[1000000]");
  assert.equal(field(large, "empty").children().length, 65);
  assert.equal(field(large, "items").children().length, 65);
  assert.equal(field(large, "items").children()[64].node.text, "36 more items");
  schemas.set("Unicode", "int8 µ; uint8 __proto__");
  const names = structValue(schemas, "Unicode", new Uint8Array([255, 42]));
  assert.equal(field(names, "µ").text, "-1");
  assert.equal(field(names, "__proto__").text, "42");
});
