#ifndef RIT_TEST_COMMON_SOCKET_H_
#define RIT_TEST_COMMON_SOCKET_H_

#include <string>

#include "message.h"
#include "socket_address.h"

namespace RitTest {

class Socket {
 public:
  enum ReturnCode {
    kCodeOk,
    kCodeSocketNotCreated,
    kCodeFailCreateSocket,
    kCodeAlreadyCreate,
    kCodeFailSend,
    kCodeFailRecv,
    kCodeNoMessages,
    kCodeNoConnectionRequest,
    kCodeFailConnect,
  };

  Socket();
  Socket(const SocketAddress& socket_address);
  Socket(const Socket& other) = delete;
  Socket(Socket&& other) noexcept;
  Socket& operator=(const Socket& other) = delete;
  Socket& operator=(Socket&& other) noexcept;
  virtual ~Socket();

  void SetSocketAddress(const SocketAddress& socket_address);
  const SocketAddress& GetSocketAddress() const;

  bool IsCreated() const;
  virtual ReturnCode Create() = 0;
  void Delete();

  ReturnCode Send(const Message& message);
  ReturnCode Recv(Message& message);

  std::string GetName() const;

 protected:
  bool HasInputBytes();
  void SetSocketDesriptor(int socket_descriptor);
  int GetSocketDescriptor() const;

 private:
  SocketAddress socket_address_;
  int socket_descriptor_;
};

}  // namespace RitTest

#endif  // RIT_TEST_COMMON_SOCKET_H_
