#include "../../../../utils/error.hpp"
#include "../../sftp.hpp"
#include "MacTypes.h"
#include "OpenTransport.h"
#include "OpenTransportProviders.h"

#define OT_CHECK(X)                                                            \
  err = (X);                                                                   \
  if (err != noErr) {                                                          \
    throw std::runtime_error("" #X " failed");                                 \
  };

// TODO: find out how to get this to be per SFTP class.
EndpointRef endpoint;
TCall sndCall;
TCall rcvCall;
DNSAddress hostDNSAddress;

void SFTP::connect(std::string addr, uint16_t port, std::string username,
                   std::string password, std::string pubkey,
                   std::string privkey, std::string sftppath) {
  OSStatus err = noErr;

  endpoint = OTOpenEndpoint(OTCreateConfiguration("tcp"), 0, nil, &err);

  OT_CHECK(OTSetSynchronous(endpoint));
  OT_CHECK(OTSetBlocking(endpoint));
  OT_CHECK(OTUseSyncIdleEvents(endpoint, false));

  OT_CHECK(OTBind(endpoint, 0, 0));

  OTMemzero(&sndCall, sizeof(TCall));

  sndCall.addr.buf = (UInt8 *)&hostDNSAddress;
  sndCall.addr.len = OTInitDNSAddress(&hostDNSAddress, (char *)addr.c_str());

  OT_CHECK(OTConnect(endpoint, &sndCall, &rcvCall));

  /* ... start it up. This will trade welcome banners, exchange keys,
   * and setup crypto, compression, and MAC layers
   */
  while ((rc = libssh2_session_handshake(session, sock)) ==
         LIBSSH2_ERROR_EAGAIN)
    ;
  if (rc) {
    throw formatted_error("Failure establishing SSH session: {}", rc);
  }

  fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

  while (!userauthlist) {
    userauthlist =
        libssh2_userauth_list(session, username.c_str(), username.size());
    this->wait();
  }
  if (userauthlist) {
    fprintf(stdout, "Authentication methods: %s\n", userauthlist);

    if (strstr(userauthlist, "publickey")) {
      try {
        RC_ERR_HANDLE("libssh2_userauth_publickey_fromfile", this,
                      libssh2_userauth_publickey_fromfile(
                          session, username.c_str(), pubkey.c_str(),
                          privkey.c_str(), password.c_str()));
        goto success;
      } catch (std::runtime_error ex) {
        printf("%s\n", ex.what());
      }
    }
    if (strstr(userauthlist, "password")) {
      try {
        RC_ERR_HANDLE("libssh2_userauth_password", this,
                      libssh2_userauth_password(session, username.c_str(),
                                                password.c_str()));
        goto success;
      } catch (std::runtime_error ex) {
        printf("%s\n", ex.what());
      }
    }
    throw std::runtime_error("failed to connect");
  success:
    printf("Success\n");
  } else {
    throw formatted_error("Failure getting autentication methods: %s",
                          this->error_msg()->c_str());
  }

  do {
    sftp_session = libssh2_sftp_init(session);

    if (!sftp_session) {
      if (rc = libssh2_session_last_errno(session);
          rc == LIBSSH2_ERROR_EAGAIN) {
        this->wait(); /* now we wait */
      } else {
        throw formatted_error("Unable to init SFTP session: %s\n",
                              this->error_msg()->c_str());
      }
    }
  } while (!sftp_session);

  /* Request a file via SFTP */
  do {
    sftp_handle =
        libssh2_sftp_open(sftp_session, sftppath.c_str(), LIBSSH2_FXF_READ, 0);
    if (!sftp_handle) {
      if (libssh2_session_last_errno(session) != LIBSSH2_ERROR_EAGAIN) {
        throw formatted_error("Unable to open file with SFTP: %s\n",
                              this->error_msg()->c_str());
      } else {
        this->wait();
      }
    }
  } while (!sftp_handle);

  return;
}

int SFTP::wait() { return rc; }
