#include "server_socket.h"

#include <sys/socket.h>
#include <unistd.h>

#include <utility>

#include "client_socket.h"
#include "socket.h"

namespace RitTest {

ServerSocket::ServerSocket() : ServerSocket(SocketAddress()) {}

ServerSocket::ServerSocket(const SocketAddress& socket_address)
    : Socket(socket_address) {}

ServerSocket::ServerSocket(ServerSocket&& other) noexcept
    : Socket(std::move(other)) {}

ServerSocket& ServerSocket::operator=(ServerSocket&& other) noexcept {
  Socket::operator=(std::move(other));
  return *this;
}

ServerSocket::~ServerSocket() {}

ServerSocket::ReturnCode ServerSocket::Create() {
  if (IsCreated()) return ReturnCode::kCodeAlreadyCreate;

  int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_descriptor < 0) return ReturnCode::kCodeFailCreateSocket;

  const sockaddr* addr = reinterpret_cast<const sockaddr*>(
      &GetSocketAddress().GetSockaddrInStruct());
  socklen_t len = GetSocketAddress().GetSocklen();

  int bind_status = bind(socket_descriptor, addr, len);
  if (bind_status < 0) {
    close(socket_descriptor);
    return ReturnCode::kCodeFailCreateSocket;
  }

  SetSocketDesriptor(socket_descriptor);
  return ReturnCode::kCodeOk;
}

ServerSocket::ReturnCode ServerSocket::ReadyToConnect() {
  if (!IsCreated()) return ReturnCode::kCodeSocketNotCreated;
  listen(GetSocketDescriptor(), 5);
  return ReturnCode::kCodeOk;
}

ServerSocket::ReturnCode ServerSocket::Connect(ClientSocket& client) {
  if (!IsCreated()) return ReturnCode::kCodeSocketNotCreated;
  if (!HasInputBytes()) return ReturnCode::kCodeNoConnectionRequest;

  sockaddr_in client_addr;
  sockaddr* pclient_addr = reinterpret_cast<sockaddr*>(&client_addr);
  socklen_t client_addr_len = sizeof(client_addr);

  int client_socket_descriptor =
      accept(GetSocketDescriptor(), pclient_addr, &client_addr_len);

  if (client_socket_descriptor < 0) return ReturnCode::kCodeFailConnect;

  ClientSocket client_socket;
  SocketAddress client_socket_address;
  client_socket_address.SetSockaddrInStruct(client_addr);
  client_socket_address.SetSocklen(client_addr_len);

  client_socket.SetSocketAddress(client_socket_address);
  client_socket.SetSocketDesriptor(client_socket_descriptor);
  client = std::move(client_socket);

  return ReturnCode::kCodeOk;
}

}  // namespace RitTest
