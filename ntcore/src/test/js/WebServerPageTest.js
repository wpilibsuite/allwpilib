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
const { decodeMessages, formatValue, buildTree } = CONTEXT;

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
