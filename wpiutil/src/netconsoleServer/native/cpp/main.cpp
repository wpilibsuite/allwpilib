/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifdef __APPLE__
#include <util.h>
#elif !defined(_WIN32)
#include <pty.h>
#endif

#include "wpi/MathExtras.h"
#include "wpi/SmallVector.h"
#include "wpi/raw_ostream.h"
#include "wpi/raw_uv_ostream.h"
#include "wpi/timestamp.h"
#include "wpi/uv/Loop.h"
#include "wpi/uv/Pipe.h"
#include "wpi/uv/Process.h"
#include "wpi/uv/Signal.h"
#include "wpi/uv/Tcp.h"
#include "wpi/uv/Tty.h"
#include "wpi/uv/Udp.h"
#include "wpi/uv/util.h"

namespace uv = wpi::uv;

static uint64_t startTime = wpi::Now();

static bool NewlineBuffer(std::string& rem, uv::Buffer& buf, size_t len,
                          wpi::SmallVectorImpl<uv::Buffer>& bufs, bool tcp,
                          uint16_t tcpSeq) {
  // scan for last newline
  wpi::StringRef str(buf.base, len);
  size_t idx = str.rfind('\n');
  if (idx == wpi::StringRef::npos) {
    // no newline yet, just keep appending to remainder
    rem += str;
    return false;
  }

  // build output
  wpi::raw_uv_ostream out(bufs, 4096);
  wpi::StringRef toCopy = str.slice(0, idx + 1);
  if (tcp) {
    // Header is 2 byte len, 1 byte type, 4 byte timestamp, 2 byte sequence num
    uint32_t ts = wpi::FloatToBits((wpi::Now() - startTime) * 1.0e-6);
    uint16_t len = rem.size() + toCopy.size() + 1 + 4 + 2;
    out << wpi::ArrayRef<uint8_t>({static_cast<uint8_t>((len >> 8) & 0xff),
                                   static_cast<uint8_t>(len & 0xff), 12,
                                   static_cast<uint8_t>((ts >> 24) & 0xff),
                                   static_cast<uint8_t>((ts >> 16) & 0xff),
                                   static_cast<uint8_t>((ts >> 8) & 0xff),
                                   static_cast<uint8_t>(ts & 0xff),
                                   static_cast<uint8_t>((tcpSeq >> 8) & 0xff),
                                   static_cast<uint8_t>(tcpSeq & 0xff)});
  }
  out << rem << toCopy;

  // reset remainder
  rem = str.slice(idx + 1, wpi::StringRef::npos);
  return true;
}

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
      [ rem = std::make_shared<std::string>(), outPtr = out.get(), addr ](
          uv::Buffer & buf, size_t len) {
        // build buffers
        wpi::SmallVector<uv::Buffer, 4> bufs;
        if (!NewlineBuffer(*rem, buf, len, bufs, false, 0)) return;

        // send output
        outPtr->Send(addr, bufs, [](auto bufs2, uv::Error) {
          for (auto buf : bufs2) buf.Deallocate();
        });
      },
      out);
}

static void CopyTcp(uv::Stream& in, std::shared_ptr<uv::Stream> out) {
  struct StreamData {
    std::string rem;
    uint16_t seq = 0;
  };
  in.data.connect([ data = std::make_shared<StreamData>(), outPtr = out.get() ](
                      uv::Buffer & buf, size_t len) {
    // build buffers
    wpi::SmallVector<uv::Buffer, 4> bufs;
    if (!NewlineBuffer(data->rem, buf, len, bufs, true, data->seq++)) return;

    // send output
    outPtr->Write(bufs, [](auto bufs2, uv::Error) {
      for (auto buf : bufs2) buf.Deallocate();
    });
  },
                  out);
}

static void CopyStream(uv::Stream& in, std::shared_ptr<uv::Stream> out) {
  in.data.connect([out](uv::Buffer& buf, size_t len) {
    uv::Buffer buf2 = buf.Dup();
    buf2.len = len;
    out->Write(buf2, [](auto bufs, uv::Error) {
      for (auto buf : bufs) buf.Deallocate();
    });
  });
}

int main(int argc, char* argv[]) {
  // parse arguments
  int programArgc = 1;
  bool useUdp = false;
  bool broadcastUdp = false;
  bool err = false;

  while (programArgc < argc && argv[programArgc][0] == '-') {
    if (wpi::StringRef(argv[programArgc]) == "-u") {
      useUdp = true;
    } else if (wpi::StringRef(argv[programArgc]) == "-b") {
      useUdp = true;
      broadcastUdp = true;
    } else {
      wpi::errs() << "unrecognized command line option " << argv[programArgc]
                  << '\n';
      err = true;
    }
    ++programArgc;
  }

  if (err || (argc - programArgc) < 1) {
    wpi::errs()
        << argv[0] << " [-ub] program [arguments ...]\n"
        << "  -u  send udp to localhost port 6666 instead of using tcp\n"
        << "  -b  broadcast udp to port 6666 instead of using tcp\n";
    return EXIT_FAILURE;
  }

  uv::Process::DisableStdioInheritance();

  auto loop = uv::Loop::Create();
  loop->error.connect(
      [](uv::Error err) { wpi::errs() << "uv ERROR: " << err.str() << '\n'; });

  // create pipes to communicate with child
  auto stdinPipe = uv::Pipe::Create(loop);
  auto stdoutPipe = uv::Pipe::Create(loop);
  auto stderrPipe = uv::Pipe::Create(loop);

  // create tty to pass from our console to child's
  auto stdinTty = uv::Tty::Create(loop, 0, true);
  auto stdoutTty = uv::Tty::Create(loop, 1, false);
  auto stderrTty = uv::Tty::Create(loop, 2, false);

  // pass through our console to child's (bidirectional)
  if (stdinTty) CopyStream(*stdinTty, stdinPipe);
  if (stdoutTty) CopyStream(*stdoutPipe, stdoutTty);
  if (stderrTty) CopyStream(*stderrPipe, stderrTty);

  // when our stdin closes, also close child stdin
  if (stdinTty) stdinTty->end.connect([stdinPipe] { stdinPipe->Close(); });

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
    tcp->connection.connect([ srv = tcp.get(), stdoutPipe, stderrPipe ] {
      auto tcp = srv->Accept();
      if (!tcp) return;

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
  for (int i = programArgc; i < argc; ++i) options.emplace_back(argv[i]);

  auto proc = uv::Process::SpawnArray(loop, argv[programArgc], options);
  if (!proc) {
    wpi::errs() << "could not start subprocess\n";
    return EXIT_FAILURE;
  }
  proc->exited.connect([](int64_t status, int) { std::exit(status); });

  // start reading
  if (stdinTty) stdinTty->StartRead();
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
