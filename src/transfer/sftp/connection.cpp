
#include "../../utils/error.hpp"
#include "sftp.hpp"
#include <cstdio>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <stdexcept>

static void kbd_callback(const char *name, int name_len,
                         const char *instruction, int instruction_len,
                         int num_prompts,
                         const LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         void **abstract) {
  int i;
  size_t n;
  char buf[1024];
  (void)abstract;

  fprintf(stdout, "Authentication name: '");
  fwrite(name, 1, (size_t)name_len, stdout);
  fprintf(stdout, "'\n");

  fprintf(stdout, "Authentication instruction: '");
  fwrite(instruction, 1, (size_t)instruction_len, stdout);
  fprintf(stdout, "'\n");

  fprintf(stdout, "Number of prompts: %d\n\n", num_prompts);

  for (i = 0; i < num_prompts; i++) {
    fprintf(stdout, "Prompt %d from server: '", i);
    fwrite(prompts[i].text, 1, prompts[i].length, stdout);
    fprintf(stdout, "'\n");

    fprintf(stdout, "Please type response: ");
    fgets(buf, sizeof(buf), stdin);
    n = strlen(buf);
    while (n > 0 && strchr("\r\n", buf[n - 1]))
      n--;
    buf[n] = 0;

    responses[i].text = strdup(buf);
    responses[i].length = (unsigned int)n;

    fprintf(stdout, "Response %d from user is '", i);
    fwrite(responses[i].text, 1, responses[i].length, stdout);
    fprintf(stdout, "'\n\n");
  }
}

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

  hostaddr = inet_addr(addr.c_str());
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = hostaddr;
  if (::connect(sock, (struct sockaddr *)(&sin), sizeof(struct sockaddr_in))) {
    throw formatted_error("failed to connect.");
  }

  printf("[addr]\t%s (%d)\n", addr.c_str(), hostaddr);
  printf("[port]\t%d\n", port);
  printf("[username]\t%s\n", username.c_str());
  printf("[password]\t%s\n", password.c_str());
  printf("[pubkey]\t%s\n", pubkey.c_str());
  printf("[privkey]\t%s\n", privkey.c_str());
  printf("[sftppath]\t%s\n", sftppath.c_str());

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

  /* check what authentication methods are available */

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
    if (strstr(userauthlist, "keyboard-interactive")) {
      try {
        RC_ERR_HANDLE("libssh2_userauth_keyboard_interactive", this,
                      libssh2_userauth_keyboard_interactive(
                          session, username.c_str(), &kbd_callback));
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
  return;
}

int SFTP::wait() {
  struct timeval timeout;
  int rc;
  fd_set fd;
  fd_set *writefd = NULL;
  fd_set *readfd = NULL;
  int dir;

  timeout.tv_sec = 0;
  timeout.tv_usec = 100;

  FD_ZERO(&fd);

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
  FD_SET(sock, &fd);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

  /* now make sure we wait in the correct direction */
  // dir = libssh2_session_block_directions(session);

  if (dir & LIBSSH2_SESSION_BLOCK_INBOUND)
    readfd = &fd;

  if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
    writefd = &fd;

  rc = select((int)(sock + 1), readfd, writefd, NULL, &timeout);

  return rc;
}
