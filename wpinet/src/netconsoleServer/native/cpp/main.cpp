// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifdef __APPLE__
#include <util.h>
#elif !defined(_WIN32)
#include <pty.h>
#endif

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
#include "wpinet/uv/Pipe.h"
#include "wpinet/uv/Process.h"
#include "wpinet/uv/Signal.h"
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
static void CopyUdp(uv::Stream& in, std::shared_ptr<uv::Udp> out,
                    bool broadcast) {
  sockaddr_in addr;
  if (broadcast) {
    out->SetBroadcast(true);
    uv::NameToAddr("0.0.0.0", 6666, &addr);
  } else {
    uv::NameToAddr("127.0.0.1", 6666, &addr);
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
// NOLINTEND

int main(int argc, char* argv[]) {
  // parse arguments
  int programArgc = 1;
  bool useUdp = false;
  bool broadcastUdp = false;
  bool err = false;

  while (programArgc < argc && argv[programArgc][0] == '-') {
    if (std::string_view(argv[programArgc]) == "-u") {
      useUdp = true;
    } else if (std::string_view(argv[programArgc]) == "-b") {
      useUdp = true;
      broadcastUdp = true;
    } else {
      wpi::print(stderr, "unrecognized command line option {}\n",
                 argv[programArgc]);
      err = true;
    }
    ++programArgc;
  }

  if (err || (argc - programArgc) < 1) {
    std::fputs(argv[0], stderr);
    std::fputs(
        " [-ub] program [arguments ...]\n"
        "  -u  send udp to localhost port 6666 instead of using tcp\n"
        "  -b  broadcast udp to port 6666 instead of using tcp\n",
        stderr);
    return EXIT_FAILURE;
  }

  uv::Process::DisableStdioInheritance();

  auto loop = uv::Loop::Create();
  loop->error.connect(
      [](uv::Error err) { wpi::print(stderr, "uv ERROR: {}\n", err.str()); });

  // create pipes to communicate with child
  auto stdinPipe = uv::Pipe::Create(loop);
  auto stdoutPipe = uv::Pipe::Create(loop);
  auto stderrPipe = uv::Pipe::Create(loop);

  // create tty to pass from our console to child's
  auto stdinTty = uv::Tty::Create(loop, 0, true);
  auto stdoutTty = uv::Tty::Create(loop, 1, false);
  auto stderrTty = uv::Tty::Create(loop, 2, false);

  // pass through our console to child's (bidirectional)
  if (stdinTty) {
    CopyStream(*stdinTty, stdinPipe);
  }
  if (stdoutTty) {
    CopyStream(*stdoutPipe, stdoutTty);
  }
  if (stderrTty) {
    CopyStream(*stderrPipe, stderrTty);
  }

  // when our stdin closes, also close child stdin
  if (stdinTty) {
    stdinTty->end.connect([stdinPipe] { stdinPipe->Close(); });
  }

  if (useUdp) {
    auto udp = uv::Udp::Create(loop);
    // tee stdout and stderr
    CopyUdp(*stdoutPipe, udp, broadcastUdp);
    CopyUdp(*stderrPipe, udp, broadcastUdp);
  } else {
    auto tcp = uv::Tcp::Create(loop);

    // bind to listen address and port
    tcp->Bind("", 1740);

    // when we get a connection, accept it
    tcp->connection.connect([srv = tcp.get(), stdoutPipe, stderrPipe] {
      auto tcp = srv->Accept();
      if (!tcp) {
        return;
      }

      // close on error
      tcp->error.connect([s = tcp.get()](wpi::uv::Error err) { s->Close(); });

      // tee stdout and stderr
      CopyTcp(*stdoutPipe, tcp);
      CopyTcp(*stderrPipe, tcp);
    });

    // start listening for incoming connections
    tcp->Listen();
  }

  // build process options
  wpi::SmallVector<uv::Process::Option, 8> options;

  // hook up pipes to child
  options.emplace_back(
      uv::Process::StdioCreatePipe(0, *stdinPipe, UV_READABLE_PIPE));
#ifndef _WIN32
  // create a PTY so the child does unbuffered output
  int parentfd, childfd;
  if (openpty(&parentfd, &childfd, nullptr, nullptr, nullptr) == 0) {
    stdoutPipe->Open(parentfd);
    options.emplace_back(uv::Process::StdioInherit(1, childfd));
  } else {
    options.emplace_back(
        uv::Process::StdioCreatePipe(1, *stdoutPipe, UV_WRITABLE_PIPE));
  }
#else
  options.emplace_back(
      uv::Process::StdioCreatePipe(1, *stdoutPipe, UV_WRITABLE_PIPE));
#endif
  options.emplace_back(
      uv::Process::StdioCreatePipe(2, *stderrPipe, UV_WRITABLE_PIPE));

  // pass our args as the child args (argv[1] becomes child argv[0], etc)
  for (int i = programArgc; i < argc; ++i) {
    options.emplace_back(argv[i]);
  }

  auto proc = uv::Process::SpawnArray(loop, argv[programArgc], options);
  if (!proc) {
    std::fputs("could not start subprocess\n", stderr);
    return EXIT_FAILURE;
  }
  proc->exited.connect([](int64_t status, int) { std::exit(status); });

  // start reading
  if (stdinTty) {
    stdinTty->StartRead();
  }
  stdoutPipe->StartRead();
  stderrPipe->StartRead();

  // pass various signals to child
  auto sigHandler = [proc](int signum) { proc->Kill(signum); };
  for (int signum : {SIGINT, SIGHUP, SIGTERM}) {
    auto sig = uv::Signal::Create(loop);
    sig->Start(signum);
    sig->signal.connect(sigHandler);
  }

  // run the loop!
  loop->Run();
}
