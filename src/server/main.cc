#include <iostream>
#include <string>

#include "server.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: ./server port" << std::endl;
    return 1;
  }

  std::string str_port(argv[1]);
  int port;
  size_t len;
  try {
    port = std::stoi(str_port, &len);
  } catch (...) {
    std::cout << "Invalid port" << std::endl;
    return 1;
  }
  if (len != str_port.length()) {
    std::cout << "Invalid port" << std::endl;
    return 1;
  }

  RitTest::ServerNS::Server server;
  auto code = server.Start(port);
  if (code != RitTest::ServerNS::Server::ReturnCode::kCodeOk)
    std::cout << "Fail start server" << std::endl;

  return 0;
}
