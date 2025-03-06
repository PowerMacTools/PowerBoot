#include "../include/unixnet2mac.h"
#include "MacErrors.h"
#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include "Threads.h"
#include "internal.hpp"
#include <cstdlib>

bool finished = false;

void notifier(void *, OTEventCode, OTResult, void *);

int connect(int socket, const struct sockaddr *address, socklen_t address_len) {
  sockaddr_in *dafaq = (sockaddr_in *)address;
  OSStatus err;
  auto s = openSockets.at(socket);
  ThrowOSErr(OTBind(s->endpoint, nil, nil));
  InetAddress addr;
  OTInitInetAddress(&addr, dafaq->sin_addr.s_addr, dafaq->sin_port);
  OTMemzero(&s->sndCall, sizeof(TCall));
  s->sndCall.addr.buf = (UInt8 *)&addr;
  s->sndCall.addr.len = sizeof(addr);

  OTSetAsynchronous(s->endpoint);
  OTInstallNotifier(s->endpoint, notifier, s);

  auto OTConnectStatus = OTConnect(s->endpoint, &s->sndCall, nil);
  if (OTConnectStatus != kOTNoDataErr) {
    ThrowOSErr(OTConnectStatus);
  }

  while (!finished) {
    YieldToAnyThread();
  }
  return 0;
};

void notifier(void *usrPtr, OTEventCode code, OTResult res, void *cookie) {
  finished = true;
  ThrowOSErr(code);

  switch (code) {
  case T_LISTEN:
    mac_error_throw("Unhandled OTLook: T_LISTEN\n");
    break;
  case T_CONNECT:
    mac_error_throw("Unhandled OTLook: T_CONNECT\n");
    break;
  case T_DATA:
    mac_error_throw("Unhandled OTLook: T_DATA\n");
    break;
  case T_EXDATA:
    mac_error_throw("Unhandled OTLook: T_EXDATA\n");
    break;
  case T_DISCONNECT:
    mac_error_throw("Unhandled OTLook: T_DISCONNECT\n");
    break;
  case T_ERROR:
    mac_error_throw("Unhandled OTLook: T_ERROR\n");
    break;
  case T_UDERR:
    mac_error_throw("Unhandled OTLook: T_UDERR\n");
    break;
  case T_ORDREL:
    mac_error_throw("Unhandled OTLook: T_ORDREL\n");
    break;
  case T_GODATA:
    mac_error_throw("Unhandled OTLook: T_GODATA\n");
    break;
  case T_GOEXDATA:
    mac_error_throw("Unhandled OTLook: T_GOEXDATA\n");
    break;
  case T_REQUEST:
    mac_error_throw("Unhandled OTLook: T_REQUEST\n");
    break;
  case T_REPLY:
    mac_error_throw("Unhandled OTLook: T_REPLY\n");
    break;
  case T_PASSCON:
    mac_error_throw("Unhandled OTLook: T_PASSCON\n");
    break;
  case T_RESET:
    mac_error_throw("Unhandled OTLook: T_RESET\n");
    break;
  case T_BINDCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_BINDCOMPLETE\n");
    break;
  case T_UNBINDCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_UNBINDCOMPLETE\n");
    break;
  case T_ACCEPTCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_ACCEPTCOMPLETE\n");
    break;
  case T_REPLYCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_REPLYCOMPLETE\n");
    break;
  case T_DISCONNECTCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_DISCONNECTCOMPLETE\n");
    break;
  case T_OPTMGMTCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_OPTMGMTCOMPLETE\n");
    break;
  case T_OPENCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_OPENCOMPLETE\n");
    break;
  case T_GETPROTADDRCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_GETPROTADDRCOMPLETE\n");
    break;
  case T_RESOLVEADDRCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_RESOLVEADDRCOMPLETE\n");
    break;
  case T_GETINFOCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_GETINFOCOMPLETE\n");
    break;
  case T_SYNCCOMPLETE:
    mac_error_throw("Unhandled OTLook: T_SYNCCOMPLETE\n");
    break;
  case T_MEMORYRELEASED:
    mac_error_throw("Unhandled OTLook: T_MEMORYRELEASED\n");
    break;
  case T_REGNAMECOMPLETE:
    mac_error_throw("Unhandled OTLook: T_REGNAMECOMPLETE\n");
    break;
  case T_DELNAMECOMPLETE:
    mac_error_throw("Unhandled OTLook: T_DELNAMECOMPLETE\n");
    break;
  case T_LKUPNAMECOMPLETE:
    mac_error_throw("Unhandled OTLook: T_LKUPNAMECOMPLETE\n");
    break;
  case T_LKUPNAMERESULT:
    mac_error_throw("Unhandled OTLook: T_LKUPNAMERESULT\n");
    break;
  }
}

/*if (OTConnectStatus != noErr) {
    if (OTConnectStatus == kOTLookErr) {
      auto OTLookStatus = OTLook(s->endpoint);
      if (OTLookStatus < 0) {
        ThrowOSErr(OTLookStatus);
      } else {

      }
    } else {
      ThrowOSErr(OTConnectStatus);
    }
  }*/