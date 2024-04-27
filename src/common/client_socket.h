#ifndef RIT_TEST_COMMON_CLIENT_SOCKET_H_
#define RIT_TEST_COMMON_CLIENT_SOCKET_H_

#include "socket.h"
#include "socket_address.h"

namespace RitTest {

class ServerSocket;

class ClientSocket : public Socket {
 public:
  using ReturnCode = Socket::ReturnCode;
  friend ServerSocket;

  ClientSocket();
  ClientSocket(const SocketAddress& socket_address);
  ClientSocket(const ClientSocket& other) = delete;
  ClientSocket(ClientSocket&& other) noexcept;
  ClientSocket& operator=(const ClientSocket& other) = delete;
  ClientSocket& operator=(ClientSocket&& other) noexcept;
  virtual ~ClientSocket();

  ReturnCode Create() override;
};

}  // namespace RitTest

#endif  // RIT_TEST_COMMON_CLIENT_SOCKET_H_
