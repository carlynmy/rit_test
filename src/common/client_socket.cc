#include "client_socket.h"

#include <sys/socket.h>
#include <unistd.h>

#include <utility>

#include "socket.h"
#include "socket_address.h"

namespace RitTest {

ClientSocket::ClientSocket() : ClientSocket(SocketAddress()) {}

ClientSocket::ClientSocket(const SocketAddress& socket_address)
    : Socket(socket_address) {}

ClientSocket::ClientSocket(ClientSocket&& other) noexcept
    : Socket(std::move(other)) {}

ClientSocket& ClientSocket::operator=(ClientSocket&& other) noexcept {
  Socket::operator=(std::move(other));
  return *this;
}

ClientSocket::~ClientSocket() {}

ClientSocket::ReturnCode ClientSocket::Create() {
  if (IsCreated()) return ReturnCode::kCodeAlreadyCreate;

  int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_descriptor < 0) return ReturnCode::kCodeFailCreateSocket;

  const sockaddr* addr = reinterpret_cast<const sockaddr*>(
      &GetSocketAddress().GetSockaddrInStruct());
  socklen_t len = GetSocketAddress().GetSocklen();

  int connect_status = connect(socket_descriptor, addr, len);

  if (connect_status < 0) {
    close(socket_descriptor);
    return ReturnCode::kCodeFailCreateSocket;
  }

  SetSocketDesriptor(socket_descriptor);
  return ReturnCode::kCodeOk;
}

}  // namespace RitTest
