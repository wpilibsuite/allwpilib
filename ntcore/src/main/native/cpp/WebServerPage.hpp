// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

namespace wpi::nt {

// Embedded so the viewer works without installed assets or internet access.
inline constexpr std::string_view WEB_SERVER_PAGE = R"html(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>NetworkTables Outline Viewer</title>
<style>
  :root { color-scheme: light dark; font: 15px system-ui, sans-serif; }
  body { margin: 0 auto; max-width: 100rem; padding: 1.5rem; }
  header { display: flex; align-items: baseline; flex-wrap: wrap; gap: 1rem; }
  h1 { font-size: 1.5rem; margin: 0; }
  #status { color: #a65b00; }
  #status[data-connected="true"] { color: #238346; }
  .muted, .type { opacity: .7; }
  .toolbar { display: flex; align-items: center; gap: .75rem; margin: 1.5rem 0; }
  input { font: inherit; padding: .5rem; width: min(25rem, 60vw); }
  .outline { border: 1px solid #8886; border-radius: .4rem; overflow: auto; }
  .columns, .row { display: grid; grid-template-columns: minmax(12rem, 2fr)
                  minmax(7rem, 1fr) minmax(15rem, 3fr); gap: 1rem; }
  .columns { min-width: 42rem; padding: .65rem 1rem; background: #8882; font-weight: 600; }
  #tree { min-width: 42rem; padding: .5rem 1rem; }
  .row { padding: .35rem 0; border-bottom: 1px solid #8882; }
  .name, .value { overflow-wrap: anywhere; white-space: pre-wrap; }
  .value { font-family: ui-monospace, monospace; }
  summary { cursor: pointer; list-style: none; position: relative; }
  summary::-webkit-details-marker { display: none; }
  summary::before { content: "\25b8"; position: absolute; top: .35rem;
                    left: calc(var(--depth) * 1.2rem); }
  details[open] > summary::before { content: "\25be"; }
  .struct-value > summary { padding-left: 1rem; }
  .struct-value > summary::before { left: 0; top: 0; }
  .struct-fields { padding-left: 1rem; border-left: 1px solid #8884; }
  .struct-field { display: grid; grid-template-columns: minmax(5rem, 1fr) minmax(0, 2fr);
                  gap: .75rem; padding: .2rem 0; }
  #empty { padding: 1rem; }
  @media (prefers-color-scheme: dark) {
    #status { color: #efba66; }
    #status[data-connected="true"] { color: #7bda97; }
  }
</style>
</head>
<body>
<header>
  <h1>NetworkTables</h1>
  <span class="muted">Outline viewer · View only</span>
  <span id="status" role="status">Connecting…</span>
</header>
<div class="toolbar">
  <label for="filter">Filter topics</label>
  <input id="filter" type="search" placeholder="Topic path" autocomplete="off">
  <span id="count" class="muted">0 topics</span>
</div>
<main class="outline" aria-label="NetworkTables topics">
  <div class="columns"><span>Topic</span><span>Type</span><span>Value</span></div>
  <div id="tree"></div>
  <p id="empty">Waiting for the server…</p>
</main>
<p class="muted">Values update live. Select a table or struct value to expand or collapse it.</p>
<noscript>JavaScript is required to view live NetworkTables values.</noscript>
<script>
"use strict";

// Decode the MessagePack subset used by NT4, including concatenated messages.
// Keep 64-bit integers as BigInt so values do not lose precision in JavaScript.
function decodeMessages(buffer) {
  const view = new DataView(buffer);
  const decoder = new TextDecoder();
  let offset = 0;
  function number(method, size) {
    const value = view[method](offset);
    offset += size;
    return value;
  }
  function bytes(length) {
    if (length > view.byteLength - offset) throw new Error("Truncated value");
    const value = new Uint8Array(buffer, offset, length);
    offset += length;
    return value;
  }
  function array(length, depth) {
    if (length > view.byteLength - offset) throw new Error("Truncated array");
    return Array.from({ length }, () => read(depth + 1));
  }
  function read(depth = 0) {
    if (depth > 2) throw new Error("Unexpected nested array");
    const tag = number("getUint8", 1);
    if (tag <= 0x7f) return tag;
    if (tag >= 0xe0) return tag - 256;
    if ((tag & 0xe0) === 0xa0) return decoder.decode(bytes(tag & 0x1f));
    if ((tag & 0xf0) === 0x90) return array(tag & 0x0f, depth);
    switch (tag) {
      case 0xc2: return false;
      case 0xc3: return true;
      case 0xc4: return bytes(number("getUint8", 1));
      case 0xc5: return bytes(number("getUint16", 2));
      case 0xc6: return bytes(number("getUint32", 4));
      case 0xca: return number("getFloat32", 4);
      case 0xcb: return number("getFloat64", 8);
      case 0xcc: return number("getUint8", 1);
      case 0xcd: return number("getUint16", 2);
      case 0xce: return number("getUint32", 4);
      case 0xcf: return number("getBigUint64", 8);
      case 0xd0: return number("getInt8", 1);
      case 0xd1: return number("getInt16", 2);
      case 0xd2: return number("getInt32", 4);
      case 0xd3: return number("getBigInt64", 8);
      case 0xd9: return decoder.decode(bytes(number("getUint8", 1)));
      case 0xda: return decoder.decode(bytes(number("getUint16", 2)));
      case 0xdb: return decoder.decode(bytes(number("getUint32", 4)));
      case 0xdc: return array(number("getUint16", 2), depth);
      case 0xdd: return array(number("getUint32", 4), depth);
      default: throw new Error("Unsupported MessagePack tag");
    }
  }
  const messages = [];
  while (offset < view.byteLength) {
    const message = read();
    if (!Array.isArray(message) || message.length !== 4) {
      throw new Error("Invalid NT4 value message");
    }
    messages.push(message);
  }
  return messages;
}

function formatValue(value) {
  if (value === undefined) return "Waiting for value…";
  if (value instanceof Uint8Array) {
    const hex = Array.from(value.subarray(0, 64),
      byte => byte.toString(16).padStart(2, "0")).join(" ");
    return `${value.length} bytes: ${hex}${value.length > 64 ? " …" : ""}`;
  }
  if (Array.isArray(value)) {
    const items = value.slice(0, 64).map(formatValue).join(", ");
    return `[${items}${value.length > 64 ? ", …" : ""}]`;
  }
  if (typeof value === "string") {
    return JSON.stringify(value.slice(0, 512)) + (value.length > 512 ? " …" : "");
  }
  return String(value);
}

const STRUCT_SCHEMA_PREFIX = "/.schema/struct:";
const STRUCT_TYPES = new Map([
  ["bool", [1, "getUint8"]], ["char", [1, "getUint8"]],
  ["int8", [1, "getInt8"]], ["int16", [2, "getInt16"]],
  ["int32", [4, "getInt32"]], ["int64", [8, "getBigInt64"]],
  ["uint8", [1, "getUint8"]], ["uint16", [2, "getUint16"]],
  ["uint32", [4, "getUint32"]], ["uint64", [8, "getBigUint64"]],
  ["float", [4, "getFloat32"]], ["float32", [4, "getFloat32"]],
  ["double", [8, "getFloat64"]], ["float64", [8, "getFloat64"]]
]);

// See wpiutil/doc/struct.adoc. Parse declarations before resolving nested types,
// since schemas and values can arrive in any order.
function parseStructSchema(schema) {
  const lexer = /\s+|[a-zA-Z_\u0080-\uFFFF][a-zA-Z_0-9\u0080-\uFFFF]*|-?\d+|[\[\]{}:;,=]/gy;
  const identifier = /^[a-zA-Z_\u0080-\uFFFF][a-zA-Z_0-9\u0080-\uFFFF]*$/;
  const tokens = [];
  let offset = 0;
  while (offset < schema.length) {
    const match = lexer.exec(schema);
    if (!match) throw new Error(`Invalid schema at character ${offset}`);
    offset = lexer.lastIndex;
    if (!/^\s+$/.test(match[0])) tokens.push(match[0]);
  }
  let index = 0;
  function expect(token) {
    if (tokens[index++] !== token) throw new Error(`Expected '${token}'`);
  }
  function name() {
    const token = tokens[index++];
    if (!token || !identifier.test(token)) throw new Error("Expected identifier");
    return token;
  }
  function integer() {
    const token = tokens[index++];
    if (!token || !/^-?\d+$/.test(token)) throw new Error("Expected integer");
    return BigInt(token);
  }
  function positive() {
    const value = Number(integer());
    if (!Number.isSafeInteger(value) || value <= 0) {
      throw new Error("Expected positive array size or bit width");
    }
    return value;
  }
  const fields = [];
  const names = new Set();
  while (index < tokens.length) {
    if (tokens[index] === ";") { ++index; continue; }
    const enums = new Map();
    if (tokens[index] === "enum" || tokens[index] === '{') {
      if (tokens[index] === "enum") ++index;
      expect('{');
      while (tokens[index] !== '}') {
        const label = name();
        expect("=");
        const value = integer();
        if (BigInt.asIntN(64, value) !== value) throw new Error("Enum value out of range");
        if (!enums.has(value)) enums.set(value, label);
        if (tokens[index] !== '}') expect(",");
      }
      expect('}');
    }
    const field = { type: name(), name: name(), count: 1, bits: 0, enums };
    if (names.has(field.name)) throw new Error(`Duplicate field '${field.name}'`);
    names.add(field.name);
    if (tokens[index] === "[") {
      ++index;
      field.count = positive();
      expect("]");
    } else if (tokens[index] === ":") {
      ++index;
      field.bits = positive();
    }
    if (index < tokens.length) expect(";");
    const integerType = /^(u?int)(8|16|32|64)$/.test(field.type);
    if (enums.size && !integerType) throw new Error("Enums require an integer type");
    if (field.bits && ((!integerType && field.type !== "bool") ||
        field.bits > (field.type === "bool" ? 1 : STRUCT_TYPES.get(field.type)[0] * 8))) {
      throw new Error(`Invalid bit width for '${field.name}'`);
    }
    fields.push(field);
  }
  return fields;
}

function createStructDatabase() {
  const schemas = new Map();
  const cache = new Map();
  function get(name, stack = []) {
    if (stack.includes(name)) throw new Error(`Circular schema '${name}'`);
    if (stack.length >= 32) throw new Error("Struct nesting is too deep");
    if (cache.has(name)) {
      const result = cache.get(name);
      if (result instanceof Error) throw result;
      return result;
    }
    try {
      if (!schemas.has(name)) throw new Error(`Missing schema '${name}'`);
      const fields = parseStructSchema(schemas.get(name));
      let offset = 0;
      let shift = 0;
      let bitSize = 0;
      for (const field of fields) {
        const primitive = STRUCT_TYPES.get(field.type);
        field.struct = primitive ? null : get(field.type, [...stack, name]);
        field.size = primitive ? primitive[0] : field.struct.size;
        field.read = primitive?.[1];
        if (!field.bits) {
          offset += bitSize;
          bitSize = 0;
          shift = 0;
          field.offset = offset;
          offset += field.size * field.count;
        } else {
          // A bool bitfield shares the preceding integer's storage if it fits.
          if (field.type === "bool" && bitSize && shift < bitSize * 8) {
            field.size = bitSize;
          } else if (field.size !== bitSize || shift + field.bits > field.size * 8) {
            offset += bitSize;
            shift = 0;
          }
          bitSize = field.size;
          field.offset = offset;
          field.shift = shift;
          shift += field.bits;
        }
        if (!Number.isSafeInteger(offset + bitSize)) throw new Error("Struct is too large");
      }
      const result = { name, fields, size: offset + bitSize };
      cache.set(name, result);
      return result;
    } catch (error) {
      cache.set(name, error);
      throw error;
    }
  }
  return {
    get,
    set(name, schema) { schemas.set(name, schema); cache.clear(); },
    remove(name) { schemas.delete(name); cache.clear(); },
    clear() { schemas.clear(); cache.clear(); }
  };
}

// Value nodes decode children only when expanded, with bounded array previews.
function structArrayNode(type, count, item) {
  return {
    text: `${type}[${count}]`,
    children: () => {
      const children = Array.from({ length: Math.min(count, 64) }, (_, i) =>
        ({ name: `[${i}]`, node: item(i) }));
      if (count > 64) children.push({ name: "…", node: { text: `${count - 64} more items` } });
      return children;
    }
  };
}

function structNode(desc, bytes, offset = 0) {
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  function fieldNode(field) {
    const start = offset + field.offset;
    if (field.type === "char") {
      let end = start + field.count;
      while (end > start && bytes[end - 1] === 0) --end;
      // Streaming decoding omits a partial UTF-8 character at the buffer end.
      const text = new TextDecoder().decode(bytes.subarray(start, end), { stream: true });
      return { text: formatValue(text) };
    }
    function item(index) {
      const position = start + index * field.size;
      if (field.struct) return structNode(field.struct, bytes, position);
      let value;
      if (field.bits) {
        value = 0n;
        for (let i = 0; i < field.size; ++i) {
          value |= BigInt(view.getUint8(position + i)) << BigInt(i * 8);
        }
        value = BigInt.asUintN(field.bits, value >> BigInt(field.shift));
        // Extend from the declared width, as Java DynamicStruct does.
        if (field.type.startsWith("int")) value = BigInt.asIntN(field.bits, value);
      } else {
        value = view[field.read](position, true);
      }
      if (field.type === "bool") value = Boolean(value);
      if (field.enums.size) {
        return { text: field.enums.get(BigInt(value)) ?? `<${value}>` };
      }
      return { text: formatValue(value) };
    }
    return field.count > 1 ? structArrayNode(field.type, field.count, item) : item(0);
  }
  return {
    text: desc.name,
    children: () => {
      const fields = desc.fields.slice(0, 64).map(field => ({
        name: field.name, type: field.type, node: fieldNode(field)
      }));
      if (desc.fields.length > 64) {
        fields.push({ name: "…", node: { text: `${desc.fields.length - 64} more fields` } });
      }
      return fields;
    }
  };
}

function topicValueNode(topic, schemas) {
  const value = topic.value;
  if (value instanceof Uint8Array) {
    if (topic.type === "structschema") {
      return { text: formatValue(new TextDecoder().decode(value)) };
    }
    if (topic.type.startsWith("struct:")) {
      try {
        const match = /^struct:(.+?)(\[\d*\])?$/.exec(topic.type);
        if (!match) throw new Error("Invalid struct type");
        const desc = schemas.get(match[1]);
        if (match[2]) {
          if (!desc.size || value.length % desc.size) throw new Error("Invalid struct array size");
          const count = value.length / desc.size;
          if (match[2] !== "[]" && Number(match[2].slice(1, -1)) !== count) {
            throw new Error("Invalid struct array size");
          }
          return structArrayNode(desc.name, count, i => structNode(desc, value, i * desc.size));
        }
        if (value.length !== desc.size) throw new Error("Invalid struct size");
        return structNode(desc, value);
      } catch (error) {
        return { text: `${formatValue(value)} (${error.message})` };
      }
    }
  }
  return { text: formatValue(value) };
}

function buildTree(topics, filter) {
  const root = { children: new Map() };
  for (const topic of topics) {
    if (!topic.name.toLowerCase().includes(filter.toLowerCase())) continue;
    let node = root;
    // Preserve empty segments: "a", "/a", and "/a/" are distinct topics.
    for (const segment of topic.name.split("/")) {
      if (!node.children.has(segment)) {
        node.children.set(segment, { children: new Map() });
      }
      node = node.children.get(segment);
    }
    node.topic = topic;
  }
  return root;
}

function startViewer() {
  const topics = new Map();
  const schemas = createStructDatabase();
  const valueCells = new Map();
  const collapsed = new Set();
  const expandedValues = new Set();
  const dirty = new Set();
  const tree = document.getElementById("tree");
  const filter = document.getElementById("filter");
  const status = document.getElementById("status");
  const empty = document.getElementById("empty");
  let rebuild = true;
  let pending = false;
  let connected = false;
  let schemasDirty = false;

  function element(tag, className, text) {
    const result = document.createElement(tag);
    result.className = className;
    if (text !== undefined) result.textContent = text;
    return result;
  }

  function updateSchema(topic, removed = false) {
    if (topic?.type !== "structschema" || !topic.name.startsWith(STRUCT_SCHEMA_PREFIX)) return;
    const name = topic.name.slice(STRUCT_SCHEMA_PREFIX.length);
    if (!removed && topic.value instanceof Uint8Array) {
      schemas.set(name, new TextDecoder().decode(topic.value));
    } else {
      schemas.remove(name);
    }
    schemasDirty = true;
  }

  function renderFields(group, children, path) {
    const signature = JSON.stringify(children.map(child => [child.name, child.type]));
    if (group.dataset.signature !== signature) {
      group.replaceChildren(...children.map(child => {
        const row = element("div", "struct-field");
        const label = element("span", "", child.name);
        label.title = child.type || "";
        row.append(label, element("div", ""));
        return row;
      }));
      group.dataset.signature = signature;
    }
    children.forEach((child, i) =>
      renderValue(group.children[i].lastElementChild, child.node, [...path, child.name]));
  }

  function renderValue(cell, node, path) {
    if (!node.children) {
      if (cell.firstElementChild || cell.textContent !== node.text) cell.textContent = node.text;
      return;
    }
    let details = cell.firstElementChild;
    if (!details) {
      details = element("details", "struct-value");
      details.append(element("summary", ""), element("div", "struct-fields"));
      const key = JSON.stringify(path);
      details.open = expandedValues.has(key);
      details.addEventListener("toggle", () => {
        if (!details.isConnected) return;
        if (details.open) {
          expandedValues.add(key);
          renderFields(details.lastElementChild, details.valueNode.children(), path);
        } else {
          expandedValues.delete(key);
          details.lastElementChild.replaceChildren();
          delete details.lastElementChild.dataset.signature;
        }
      });
      cell.replaceChildren(details);
    }
    // Retain the details element during live updates so focus and expansion
    // are stable. Only materialize fields the user has chosen to expand.
    details.valueNode = node;
    details.firstElementChild.textContent = node.text;
    if (details.open) renderFields(details.lastElementChild, node.children(), path);
  }

  function renderChildren(parent, node, segments) {
    const children = [...node.children].sort(([a], [b]) => a.localeCompare(b));
    for (const [name, child] of children) {
      const path = [...segments, name];
      const row = element("div", "row");
      const label = element("span", "name", name || (path.length === 1 ? "/" : "(empty)"));
      label.style.paddingLeft = `${segments.length * 1.2 + 1}rem`;
      label.title = path.join("/");
      row.append(label, element("span", "type", child.topic?.type || ""));
      const value = element("div", "value");
      if (child.topic) {
        renderValue(value, topicValueNode(child.topic, schemas), [child.topic.name]);
        valueCells.set(child.topic.id, value);
      }
      row.append(value);
      if (child.children.size) {
        const details = element("details", "");
        const key = JSON.stringify(path);
        details.open = filter.value !== "" || !collapsed.has(key);
        details.addEventListener("toggle", () => {
          if (!details.isConnected || filter.value !== "") return;
          if (details.open) collapsed.delete(key);
          else collapsed.add(key);
        });
        const summary = element("summary", "");
        summary.style.setProperty("--depth", segments.length);
        summary.append(row);
        const group = element("div", "children");
        renderChildren(group, child, path);
        details.append(summary, group);
        parent.append(details);
      } else {
        parent.append(row);
      }
    }
  }

  function scheduleRender(structure = false) {
    rebuild ||= structure;
    if (pending) return;
    pending = true;
    requestAnimationFrame(() => {
      pending = false;
      if (schemasDirty) {
        // A changed or late nested schema may affect any struct topic.
        for (const topic of topics.values()) {
          if (topic.type.startsWith("struct:")) dirty.add(topic.id);
        }
        schemasDirty = false;
      }
      if (rebuild) {
        valueCells.clear();
        const fragment = document.createDocumentFragment();
        renderChildren(fragment, buildTree(topics.values(), filter.value), []);
        tree.replaceChildren(fragment);
        document.getElementById("count").textContent =
          `${valueCells.size} / ${topics.size} topics`;
        empty.hidden = valueCells.size !== 0;
        empty.textContent = !connected ? "Waiting for the server…" :
          topics.size === 0 ? "No topics published." : "No matching topics.";
        rebuild = false;
      } else {
        for (const id of dirty) {
          const cell = valueCells.get(id);
          const topic = topics.get(id);
          if (cell && topic) renderValue(cell, topicValueNode(topic, schemas), [topic.name]);
        }
      }
      dirty.clear();
    });
  }
  filter.addEventListener("input", () => scheduleRender(true));

  function connect() {
    const scheme = location.protocol === "https:" ? "wss:" : "ws:";
    const socket = new WebSocket(`${scheme}//${location.host}/nt/outline-viewer`,
      "v4.1.networktables.first.wpi.edu");
    socket.binaryType = "arraybuffer";
    let heartbeat;
    let lastPong = performance.now();
    socket.onopen = () => {
      connected = true;
      status.textContent = "Connected";
      status.dataset.connected = "true";
      socket.send(JSON.stringify([{ method: "subscribe", params: {
        subuid: 1, topics: [""], options: { prefix: true, periodic: 0.1 }
      } }]));
      // Browsers cannot send WebSocket pings. Use NT timestamp requests to
      // detect a lost connection, without publishing or modifying any topics.
      lastPong = performance.now();
      heartbeat = setInterval(() => {
        if (performance.now() - lastPong > 5000) socket.close();
        else socket.send(new Uint8Array([0x94, 0xff, 0, 2, 0]));
      }, 1000);
      scheduleRender(true);
    };
    socket.onmessage = event => {
      try {
        if (typeof event.data === "string") {
          for (const { method, params } of JSON.parse(event.data)) {
            if (method === "announce") {
              const previous = topics.get(params.id);
              const same = previous?.name === params.name && previous?.type === params.type;
              if (!same) updateSchema(previous, true);
              topics.set(params.id, { ...params, value: same ? previous.value : undefined });
              scheduleRender(true);
            } else if (method === "unannounce") {
              updateSchema(topics.get(params.id), true);
              topics.delete(params.id);
              scheduleRender(true);
            }
          }
        } else {
          for (const [id, , , value] of decodeMessages(event.data)) {
            if (Number(id) === -1) {
              lastPong = performance.now();
            } else if (topics.has(Number(id))) {
              const topic = topics.get(Number(id));
              topic.value = value;
              updateSchema(topic);
              dirty.add(Number(id));
            }
          }
          scheduleRender();
        }
      } catch (error) {
        console.error("Unable to read NetworkTables data", error);
        socket.close();
      }
    };
    socket.onerror = () => socket.close();
    socket.onclose = () => {
      clearInterval(heartbeat);
      connected = false;
      status.textContent = "Disconnected · Reconnecting…";
      status.dataset.connected = "false";
      // Topic IDs are connection-specific. Never display stale values as live.
      topics.clear();
      schemas.clear();
      schemasDirty = false;
      dirty.clear();
      scheduleRender(true);
      setTimeout(connect, 1000);
    };
  }
  connect();
}

if (typeof document !== "undefined") startViewer();
</script>
</body>
</html>
)html";

}  // namespace wpi::nt
