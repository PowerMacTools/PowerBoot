#include "MacErrors.h"
#include "MacMemory.h"
#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include "Threads.h"
#include "internal.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>

#include "netinet/in.h"
#include "sys/socket.h"

bool finished = false;

void notifier(void *, OTEventCode, OTResult, void *);
void *connect_thread(void *arg);

typedef struct {
  int socket;
  const struct sockaddr *address;
  socklen_t address_len;
} connectThreadParams;

extern "C" int connect(int socket, const struct sockaddr *address,
                       socklen_t address_len) {
  ThreadID connect_thread_id = 0;

  connectThreadParams params = connectThreadParams{
      .socket = socket,
      .address = address,
      .address_len = address_len,
  };

  MaxApplZone();

  for (int i = 0; i < 10; i++) {
    MoreMasters();
  }

  OSErr err = NewThread(kCooperativeThread, connect_thread, &params, 100000,
                        kCreateIfNeeded, NULL, &connect_thread_id);

  while (!finished) {
    YieldToAnyThread();
  }

  DisposeThread(connect_thread_id, NULL, false);
  YieldToAnyThread();

  return 0;
};

void *connect_thread(void *arg) {
  auto params = (connectThreadParams *)arg;

  int socket = params->socket;
  sockaddr_in *address = (sockaddr_in *)params->address;

  socklen_t address_len = params->address_len;

  OSStatus err;
  DNSAddress hostDNSAddress;

  auto s = openSockets.at(socket);
  YieldToAnyThread();

  s->halting = true;

  ThrowOSErr(OTSetBlocking(s->endpoint));
  ThrowOSErr(OTSetSynchronous(s->endpoint));

  ThrowOSErr(OTBind(s->endpoint, nil, nil));

  ThrowOSErr(OTSetNonBlocking(s->endpoint));
  ThrowOSErr(OTSetAsynchronous(s->endpoint));

  ThrowOSErr(OTInstallNotifier(s->endpoint, notifier, s));

  YieldToAnyThread();
  InetAddress addr;

  OTInitInetAddress(&addr, address->sin_port, address->sin_addr.s_addr);
  YieldToAnyThread();

  OTMemzero(&s->sndCall, sizeof(TCall));
  s->sndCall.addr.buf = (UInt8 *)&addr;
  s->sndCall.addr.len = sizeof(addr);

  finished = false;
  s->halting = true;

  auto OTConnectStatus = OTConnect(s->endpoint, &s->sndCall, nil);
  YieldToAnyThread();
  if (OTConnectStatus != kOTNoDataErr) {
    ThrowOSErr(OTConnectStatus);
  }

  YieldToAnyThread();

  return 0;
}

void notifier(void *usrPtr, OTEventCode code, OTResult res, void *cookie) {
  YieldToAnyThread();

  finished = true;
  ThrowOSErr(res);

  Socket *s = ((Socket *)usrPtr);
  EndpointRef endpoint = s->endpoint;

  TCall *upperCall = (TCall *)cookie;

  TCall *call;
  TDiscon *discon;
  char *buf;
  EndpointRef worker;
  int recieved;
  OTByteCount size;

  /*switch (endpoint->GetEndpointState()) {
  case T_UNINIT:
    mac_error_throw("Unhandled Endpoint State: T_UNINIT\n");
    break;
  case T_UNBND:
    mac_error_throw("Unhandled Endpoint State: T_UNBND\n");
    break;
  case T_IDLE:
    YieldToAnyThread();
    mac_error_throw("Unhandled Endpoint State: T_IDLE\n");
    break;
  case T_OUTCON:
    OTAccept(endpoint, worker, call);
    break;
  case T_INCON:
    mac_error_throw("Unhandled Endpoint State: T_INCON\n");
    break;
  case T_DATAXFER:
    mac_error_throw("Unhandled Endpoint State: T_DATAXFER\n");
    break;
  case T_OUTREL:
    mac_error_throw("Unhandled Endpoint State: T_OUTREL\n");
    break;
  case T_INREL:
    mac_error_throw("Unhandled Endpoint State: T_INREL\n");
    break;
  }*/

  switch (code) {
  case T_LISTEN:
    mac_error_throw("Unhandled OTLook: T_LISTEN\n");
    break;
  case T_CONNECT:
    // printf("Connected to %s\n", upperCall->addr.buf);
    call = (TCall *)malloc(sizeof(TCall));
    buf = (char *)malloc(255);
    OTMemzero(buf, 255);

    ThrowOSErr(OTRcvConnect(endpoint, call));
    OTInetAddressToName(s->inetsvc, (InetHost)call->addr.buf, buf);
    printf("Connected: %s\n", buf);
    break;
  case T_DATA:
  case T_EXDATA:
    // got data
    // we always try to read, so ignore this event
    break;
  case T_DISCONNECT:
    discon = (TDiscon *)malloc(sizeof(TDiscon));
    OTRcvDisconnect(endpoint, discon);
    mac_error_throw("Disconnected: %s\n", discon->reason);
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
  case kOTProviderIsDisconnected:
    mac_error_throw("Unhandled OTLook: kOTProviderIsDisconnected\n");
    break;
  case kOTSyncIdleEvent:
    mac_error_throw("Unhandled OTLook: kOTSyncIdleEvent\n");
    break;
  case kOTProviderIsReconnected:
    mac_error_throw("Unhandled OTLook: kOTProviderIsReconnected\n");
    break;
  case kOTProviderWillClose:
    mac_error_throw("Unhandled OTLook: kOTProviderWillClose\n");
    break;
  case kOTProviderIsClosed:
    mac_error_throw("Unhandled OTLook: kOTProviderIsClosed\n");
    break;
  }

  s->halting = false;

  YieldToAnyThread();
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