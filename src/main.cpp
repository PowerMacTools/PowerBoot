#include "transfer/sftp/sftp.hpp"
#include "transfer/transfer.hpp"
#include <cstdio>
#include <optional>
#include <stdexcept>

int main(int argc, char *argv[]) {
  try {
    TransferProtocol *sftp = new SFTP();
    sftp->connect("192.168.1.145", 22, "gavin", "UnitedSenora02", "", "",
                  "/home/gavin/");

    auto dir = sftp->opendir(".");

    std::optional<File> file = dir->next();

    for (; file.has_value(); file = dir->next()) {
      auto val = file.value();
      auto st = val.longentry();
      printf("%s\n", st->c_str());
    }

    dir->close();
  } catch (std::runtime_error *ex) {
    printf("%s\n", ex->what());
  } catch (std::runtime_error ex) {
    printf("%s\n", ex.what());
  }
}