// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <bit>
#include <cstdio>
#include <memory>
#include <string>

#include <fmt/format.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>
#include <wpi/print.h>
#include <wpi/timestamp.h>

#include "wpinet/raw_uv_ostream.h"
#include "wpinet/uv/Loop.h"
#include "wpinet/uv/Tcp.h"
#include "wpinet/uv/Tty.h"
#include "wpinet/uv/Udp.h"
#include "wpinet/uv/util.h"

namespace uv = wpi::uv;

static uint64_t startTime = wpi::Now();

static bool NewlineBuffer(std::string& rem, uv::Buffer& buf, size_t len,
                          wpi::SmallVectorImpl<uv::Buffer>& bufs, bool tcp,
                          uint16_t tcpSeq) {
  // scan for last newline
  std::string_view str(buf.base, len);
  size_t idx = str.rfind('\n');
  if (idx == std::string_view::npos) {
    // no newline yet, just keep appending to remainder
    rem += str;
    return false;
  }

  // build output
  wpi::raw_uv_ostream out(bufs, 4096);
  std::string_view toCopy = wpi::slice(str, 0, idx + 1);
  if (tcp) {
    // Header is 2 byte len, 1 byte type, 4 byte timestamp, 2 byte sequence num
    uint32_t ts =
        std::bit_cast<uint32_t, float>((wpi::Now() - startTime) * 1.0e-6);
    uint16_t len = rem.size() + toCopy.size() + 1 + 4 + 2;
    const uint8_t header[] = {static_cast<uint8_t>((len >> 8) & 0xff),
                              static_cast<uint8_t>(len & 0xff),
                              12,
                              static_cast<uint8_t>((ts >> 24) & 0xff),
                              static_cast<uint8_t>((ts >> 16) & 0xff),
                              static_cast<uint8_t>((ts >> 8) & 0xff),
                              static_cast<uint8_t>(ts & 0xff),
                              static_cast<uint8_t>((tcpSeq >> 8) & 0xff),
                              static_cast<uint8_t>(tcpSeq & 0xff)};
    out << std::span<const uint8_t>(header);
  }
  out << rem << toCopy;

  // reset remainder
  rem = wpi::slice(str, idx + 1, std::string_view::npos);
  return true;
}

// FIXME: clang-tidy reports a false positive for leaking a captured shared_ptr
//        (clang-analyzer-cplusplus.NewDeleteLeaks)

// NOLINTBEGIN
static void CopyUdp(uv::Stream& in, std::shared_ptr<uv::Udp> out, int port,
                    bool broadcast) {
  sockaddr_in addr;
  if (broadcast) {
    out->SetBroadcast(true);
    uv::NameToAddr("0.0.0.0", port, &addr);
  } else {
    uv::NameToAddr("127.0.0.1", port, &addr);
  }

  in.data.connect(
      [rem = std::make_shared<std::string>(), outPtr = out.get(), addr](
          uv::Buffer& buf, size_t len) {
        // build buffers
        wpi::SmallVector<uv::Buffer, 4> bufs;
        if (!NewlineBuffer(*rem, buf, len, bufs, false, 0)) {
          return;
        }

        // send output
        outPtr->Send(addr, bufs, [](auto bufs2, uv::Error) {
          for (auto buf : bufs2) {
            buf.Deallocate();
          }
        });
      },
      out);
}

static void CopyTcp(uv::Stream& in, std::shared_ptr<uv::Stream> out) {
  struct StreamData {
    std::string rem;
    uint16_t seq = 0;
  };
  in.data.connect(
      [data = std::make_shared<StreamData>(), outPtr = out.get()](
          uv::Buffer& buf, size_t len) {
        // build buffers
        wpi::SmallVector<uv::Buffer, 4> bufs;
        if (!NewlineBuffer(data->rem, buf, len, bufs, true, data->seq++)) {
          return;
        }

        // send output
        outPtr->Write(bufs, [](auto bufs2, uv::Error) {
          for (auto buf : bufs2) {
            buf.Deallocate();
          }
        });
      },
      out);
}
// NOLINTEND

static void CopyStream(uv::Stream& in, std::shared_ptr<uv::Stream> out) {
  in.data.connect([out](uv::Buffer& buf, size_t len) {
    uv::Buffer buf2 = buf.Dup();
    buf2.len = len;
    out->Write({buf2}, [](auto bufs, uv::Error) {
      for (auto buf : bufs) {
        buf.Deallocate();
      }
    });
  });
}

int main(int argc, char* argv[]) {
  // parse arguments
  int arg = 1;
  bool useUdp = false;
  bool broadcastUdp = false;
  bool err = false;
  int port = -1;

  while (arg < argc && argv[arg][0] == '-') {
    if (std::string_view(argv[arg]) == "-u") {
      useUdp = true;
    } else if (std::string_view(argv[arg]) == "-b") {
      useUdp = true;
      broadcastUdp = true;
    } else if (std::string_view(argv[arg]) == "-p") {
      ++arg;
      std::optional<int> portValue;
      if (arg >= argc || argv[arg][0] == '-' ||
          !(portValue = wpi::parse_integer<int>(argv[arg], 10))) {
        std::fputs("-p must be followed by port number\n", stderr);
        err = true;
      } else if (portValue) {
        port = portValue.value();
      }
    } else {
      wpi::print(stderr, "unrecognized command line option {}\n", argv[arg]);
      err = true;
    }
    ++arg;
  }

  if (err) {
    std::fputs(argv[0], stderr);
    std::fputs(
        " [-ub] [-p PORT]\n"
        "  -u       send udp to localhost port 6666 instead of using tcp\n"
        "  -b       broadcast udp to port 6666 instead of using tcp\n"
        "  -p PORT  use port PORT instead of 6666 (udp) or 1740 (tcp)\n",
        stderr);
    return EXIT_FAILURE;
  }

  auto loop = uv::Loop::Create();
  loop->error.connect(
      [](uv::Error err) { wpi::print(stderr, "uv ERROR: {}\n", err.str()); });

  // create ttys
  auto stdinTty = uv::Tty::Create(loop, 0, true);
  auto stdoutTty = uv::Tty::Create(loop, 1, false);

  // don't bother continuing if we don't have a stdin
  if (!stdinTty) {
    return EXIT_SUCCESS;
  }

  // pass through our input to output
  if (stdoutTty) {
    CopyStream(*stdinTty, stdoutTty);
  }

  // when our stdin closes, exit
  stdinTty->end.connect([] { std::exit(EXIT_SUCCESS); });

  if (useUdp) {
    auto udp = uv::Udp::Create(loop);
    // tee
    CopyUdp(*stdinTty, udp, port < 0 ? 6666 : port, broadcastUdp);
  } else {
    auto tcp = uv::Tcp::Create(loop);

    // bind to listen address and port
    tcp->Bind("", port < 0 ? 1740 : port);

    // when we get a connection, accept it
    tcp->connection.connect([srv = tcp.get(), stdinTty] {
      auto tcp = srv->Accept();
      if (!tcp) {
        return;
      }

      // close on error
      tcp->error.connect([s = tcp.get()](wpi::uv::Error err) { s->Close(); });

      // tee
      CopyTcp(*stdinTty, tcp);
    });

    // start listening for incoming connections
    tcp->Listen();
  }

  // start reading
  if (stdinTty) {
    stdinTty->StartRead();
  }

  // run the loop!
  loop->Run();
}
