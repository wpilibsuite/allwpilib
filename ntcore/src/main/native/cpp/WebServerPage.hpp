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
<p class="muted">Values update live. Select a table to expand or collapse it.</p>
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
  const valueCells = new Map();
  const collapsed = new Set();
  const dirty = new Set();
  const tree = document.getElementById("tree");
  const filter = document.getElementById("filter");
  const status = document.getElementById("status");
  const empty = document.getElementById("empty");
  let rebuild = true;
  let pending = false;
  let connected = false;

  function element(tag, className, text) {
    const result = document.createElement(tag);
    result.className = className;
    if (text !== undefined) result.textContent = text;
    return result;
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
      const value = element("span", "value");
      if (child.topic) {
        value.textContent = formatValue(child.topic.value);
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
          if (cell) cell.textContent = formatValue(topics.get(id)?.value);
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
              topics.set(params.id, { ...params, value: topics.get(params.id)?.value });
              scheduleRender(true);
            } else if (method === "unannounce") {
              topics.delete(params.id);
              scheduleRender(true);
            }
          }
        } else {
          for (const [id, , , value] of decodeMessages(event.data)) {
            if (Number(id) === -1) {
              lastPong = performance.now();
            } else if (topics.has(Number(id))) {
              topics.get(Number(id)).value = value;
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
