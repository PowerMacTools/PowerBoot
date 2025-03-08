#include "../../console/console.hpp"
#include "sftp.hpp"
#include <libssh2.h>

#ifdef __RETRO__
#include "Threads.h"
ThreadID read_thread_id = 0;
#endif

void *read_thread(void *arg) {
  auto args = (ReadThreadArgs *)arg;
  auto sftp = args->sftp;
  auto options = args->options;
  return sftp->read_thread(options);
}

void *SFTP::read_thread(ConnectionOptions options) {
  int ok = 1;
  int rc = LIBSSH2_ERROR_NONE;

  // yield until given a command
  // while (this->read_thread_command == ReadThreadCommand::Wait) {
  //   YieldToAnyThread();
  // }

  if (this->read_thread_command == ReadThreadCommand::Exit) {
    return 0;
  }

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == LIBSSH2_INVALID_SOCKET) {
    read_error_throw("failed to create socket.");
  }

  hostaddr = inet_addr(options.addr.c_str());
  sin.sin_family = AF_INET;
  sin.sin_port = htons(options.port);
  sin.sin_addr.s_addr = hostaddr;
  if (::connect(sock, (struct sockaddr *)(&sin), sizeof(struct sockaddr_in))) {
    read_error_throw("failed to connect.");
  }

  while ((rc = libssh2_session_handshake(session, 0)) == LIBSSH2_ERROR_EAGAIN) {
#ifdef __RETRO__
    YieldToAnyThread();
#endif
  }

  if (rc) {
    read_error_throw("Failure establishing SSH session: %s",
                     error_msg()->c_str());
  }

  fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

  while (!userauthlist) {
    userauthlist = libssh2_userauth_list(session, options.username.c_str(),
                                         options.username.size());
    this->wait();
  }

  if (userauthlist) {
    fprintf(stdout, "Authentication methods: %s\n", userauthlist);

    if (strstr(userauthlist, "publickey")) {
      if (libssh2_userauth_publickey_fromfile_ex(
              (session), (options.username.c_str()),
              (unsigned int)strlen(options.username.c_str()),
              (options.pubkey.c_str()), (options.privkey.c_str()),
              (options.password.c_str())) >= 0) {
        goto success;
      } else {
        printf("PublicKey Auth failed: %s\n", this->error_msg(rc)->c_str());
      }
    }
    if (strstr(userauthlist, "password")) {
      if (libssh2_userauth_password(session, options.username.c_str(),
                                    options.password.c_str()) >= 0) {
        goto success;
      } else {
        printf("Password Auth failed: %s\n", this->error_msg(rc)->c_str());
      }
    }
    read_error_throw("failed to connect");
  success:
    printf("Success\n");
  } else {
    read_error_throw("Failure getting autentication methods: %s",
                     this->error_msg()->c_str());
  }

  do {
    sftp_session = libssh2_sftp_init(session);

    if (!sftp_session) {
      if (rc = libssh2_session_last_errno(session);
          rc == LIBSSH2_ERROR_EAGAIN) {
        this->wait();
      } else {
        read_error_throw("Unable to init SFTP session: %s\n",
                         this->error_msg()->c_str());
      }
    }
  } while (!sftp_session);

  channel = libssh2_channel_open_session(session);

  read_thread_state = ReadThreadState::Open;

  // read until failure, command to EXIT, or remote EOF
  while (read_thread_command == ReadThreadCommand::Read &&
         read_thread_state == ReadThreadState::Open &&
         libssh2_channel_eof(channel) == 0) {

    ssize_t rc = libssh2_channel_read(channel, recv_buffer, 1024 * 4);

    if (rc <= 0) {
      read_error_throw("channel read error\n");
    }

#ifdef __RETRO__
    YieldToAnyThread();
#endif
  }

  read_error_throw("Disconnected by server.\n");

  return 0;
}