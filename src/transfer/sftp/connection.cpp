#include "../../utils/error.hpp"
#include "sftp.hpp"

void SFTP::connect(std::string addr, uint16_t port, std::string username,
                   std::string password, std::string pubkey,
                   std::string privkey, std::string sftppath) {
  /*
   * The application code is responsible for creating the socket
   * and establishing the connection
   */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == LIBSSH2_INVALID_SOCKET) {
    throw formatted_error("failed to create socket.");
  }

  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = hostaddr;
  if (::connect(sock, (struct sockaddr *)(&sin), sizeof(struct sockaddr_in))) {
    throw formatted_error("failed to connect.");
  }

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

  if (auth_pw) {
    /* We could authenticate via password */
    while ((rc = libssh2_userauth_password(session, username.c_str(),
                                           password.c_str())) ==
           LIBSSH2_ERROR_EAGAIN)
      ;
    if (rc) {
      throw formatted_error("Authentication by password failed.\n");
    }
  } else {
    /* Or by public key */
    while ((rc = libssh2_userauth_publickey_fromfile(
                session, username.c_str(), pubkey.c_str(), privkey.c_str(),
                password.c_str())) == LIBSSH2_ERROR_EAGAIN)
      ;
    if (rc) {
      throw formatted_error("Authentication by public key failed.\n");
    }
  }

  do {
    sftp_session = libssh2_sftp_init(session);

    if (!sftp_session) {
      if (libssh2_session_last_errno(session) == LIBSSH2_ERROR_EAGAIN) {
        this->wait(); /* now we wait */
      } else {
        throw formatted_error("Unable to init SFTP session: %s\n",
                              this->error_msg(rc)->c_str());
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
                              this->error_msg(rc)->c_str());
      } else {
        this->wait();
      }
    }
  } while (!sftp_handle);

  return;
}