/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/MathExtras.h"
#include "wpi/SmallVector.h"
#include "wpi/raw_ostream.h"
#include "wpi/raw_uv_ostream.h"
#include "wpi/timestamp.h"
#include "wpi/uv/Loop.h"
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
  int arg = 1;
  bool useUdp = false;
  bool broadcastUdp = false;
  bool err = false;

  while (arg < argc && argv[arg][0] == '-') {
    if (wpi::StringRef(argv[arg]) == "-u") {
      useUdp = true;
    } else if (wpi::StringRef(argv[arg]) == "-b") {
      useUdp = true;
      broadcastUdp = true;
    } else {
      wpi::errs() << "unrecognized command line option " << argv[arg] << '\n';
      err = true;
    }
    ++arg;
  }

  if (err) {
    wpi::errs()
        << argv[0] << " [-ub]\n"
        << "  -u  send udp to localhost port 6666 instead of using tcp\n"
        << "  -b  broadcast udp to port 6666 instead of using tcp\n";
    return EXIT_FAILURE;
  }

  auto loop = uv::Loop::Create();
  loop->error.connect(
      [](uv::Error err) { wpi::errs() << "uv ERROR: " << err.str() << '\n'; });

  // create ttys
  auto stdinTty = uv::Tty::Create(loop, 0, true);
  auto stdoutTty = uv::Tty::Create(loop, 1, false);

  // don't bother continuing if we don't have a stdin
  if (!stdinTty) return EXIT_SUCCESS;

  // pass through our input to output
  if (stdoutTty) CopyStream(*stdinTty, stdoutTty);

  // when our stdin closes, exit
  stdinTty->end.connect([] { std::exit(EXIT_SUCCESS); });

  if (useUdp) {
    auto udp = uv::Udp::Create(loop);
    // tee
    CopyUdp(*stdinTty, udp, broadcastUdp);
  } else {
    auto tcp = uv::Tcp::Create(loop);

    // bind to listen address and port
    tcp->Bind("", 1740);

    // when we get a connection, accept it
    tcp->connection.connect([ srv = tcp.get(), stdinTty ] {
      auto tcp = srv->Accept();
      if (!tcp) return;

      // close on error
      tcp->error.connect([s = tcp.get()](wpi::uv::Error err) { s->Close(); });

      // tee
      CopyTcp(*stdinTty, tcp);
    });

    // start listening for incoming connections
    tcp->Listen();
  }

  // start reading
  if (stdinTty) stdinTty->StartRead();

  // run the loop!
  loop->Run();
}
