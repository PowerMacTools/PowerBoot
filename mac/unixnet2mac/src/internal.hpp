
#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"
#include <cstdio>
#include <unordered_map>

#ifndef __INTERNAL_HPP
#define __INTERNAL_HPP

// On 68k, and only for me for some fucking reason...
// These functions are in all caps.
extern "C" {
EXTERN_API(OSStatus)
OTCONNECT(EndpointRef ref, TCall *sndCall, TCall *rcvCall);
EXTERN_API(OSStatus)
OTBIND(EndpointRef ref, TBind *reqAddr, /* can be NULL */
       TBind *retAddr);                 /* can be NULL */
EXTERN_API(OTByteCount)
OTINITDNSADDRESS(DNSAddress *addr, char *str);
EXTERN_API(OTConfigurationRef) OTCREATECONFIGURATION(const char *path);
EXTERN_API(EndpointRef)
OTOPENENDPOINT(OTConfigurationRef cfig, OTOpenFlags oflag, TEndpointInfo *info,
               OSStatus *err);

EXTERN_API(OSStatus) INITOPENTRANSPORT();
}

class Socket {

public:
  EndpointRef endpoint;
  OTConfigurationRef cfg;
  TEndpointInfo info;

  DNSAddress hostDNSAddress;
  TCall *sndCall = NULL;
  TCall *rcvCall = NULL;

  TBind *retAddr;

  ~Socket();
};

extern std::unordered_map<size_t, Socket *> openSockets;

void __throw_os_err(const char *file, int line, const char *func, OSErr err);
#define ThrowOSErr(err) __throw_os_err(__FILE__, __LINE__, #err, err)

#define OPEN_SOCKET_INSERT(x, y)                                               \
  openSockets.insert(std::pair<size_t, Socket *>(x, y))

void mac_error_throw(const char *format, ...);
#endif