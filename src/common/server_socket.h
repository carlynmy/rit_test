#ifndef RIT_TEST_COMMON_SERVER_SOCKET_H_
#define RIT_TEST_COMMON_SERVER_SOCKET_H_

#include "client_socket.h"
#include "socket.h"

namespace RitTest {

class ServerSocket : public Socket {
 public:
  using ReturnCode = Socket::ReturnCode;

  ServerSocket();
  ServerSocket(const SocketAddress& socket_address);
  ServerSocket(const ServerSocket& other) = delete;
  ServerSocket(ServerSocket&& other) noexcept;
  ServerSocket& operator=(const ServerSocket& other) = delete;
  ServerSocket& operator=(ServerSocket&& other) noexcept;
  virtual ~ServerSocket();

  ReturnCode Create() override;

  ReturnCode ReadyToConnect();
  ReturnCode Connect(ClientSocket& client);
};

}  // namespace RitTest

#endif  // RIT_TEST_COMMON_SERVER_SOCKET_H_
