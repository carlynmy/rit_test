#include "socket.h"

#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <string>
#include <utility>

#include "message.h"
#include "socket_address.h"

namespace RitTest {

Socket::Socket() : Socket(SocketAddress()) {}

Socket::Socket(const SocketAddress& socket_address)
    : socket_address_(socket_address), socket_descriptor_(-1) {}

Socket::Socket(Socket&& other) noexcept : Socket() {
  socket_descriptor_ = other.socket_descriptor_;
  other.socket_descriptor_ = -1;
  socket_address_ = std::move(other.socket_address_);
}

Socket& Socket::operator=(Socket&& other) noexcept {
  if (this == &other) return *this;

  Delete();
  socket_descriptor_ = other.socket_descriptor_;
  other.socket_descriptor_ = -1;
  socket_address_ = std::move(other.socket_address_);

  return *this;
}

Socket::~Socket() { Delete(); }

void Socket::SetSocketAddress(const SocketAddress& socket_address) {
  socket_address_ = socket_address;
}

const SocketAddress& Socket::GetSocketAddress() const {
  return socket_address_;
}

bool Socket::IsCreated() const { return socket_descriptor_ >= 0; }

void Socket::Delete() {
  if (IsCreated()) {
    close(socket_descriptor_);
    socket_descriptor_ = -1;
  }
}

Socket::ReturnCode Socket::Send(const Message& message) {
  if (!IsCreated()) return ReturnCode::kCodeSocketNotCreated;

  std::string bytes = message.ToBytes();
  int send_status = send(socket_descriptor_, bytes.c_str(), bytes.length(), 0);

  if (send_status != static_cast<int>(bytes.length()))
    return ReturnCode::kCodeFailSend;

  return ReturnCode::kCodeOk;
}

Socket::ReturnCode Socket::Recv(Message& message) {
  if (!IsCreated()) return ReturnCode::kCodeSocketNotCreated;
  if (!HasInputBytes()) return ReturnCode::kCodeNoMessages;

  int32_t size;
  int recv_status = recv(socket_descriptor_, &size, 4, MSG_PEEK);
  if (recv_status != 4) return ReturnCode::kCodeFailRecv;

  std::string bytes;
  bytes.resize(size);
  recv_status = recv(socket_descriptor_, bytes.data(), size, 0);
  if (recv_status != size) return ReturnCode::kCodeFailRecv;

  message.FromBytes(bytes);
  return ReturnCode::kCodeOk;
}

std::string Socket::GetName() const {
  return std::to_string(socket_address_.GetPort());
}

bool Socket::HasInputBytes() {
  pollfd poll_fd;
  poll_fd.fd = socket_descriptor_;
  poll_fd.events = POLLIN;
  poll_fd.revents = 0;

  int status = poll(&poll_fd, 1, 100);
  if (status != 1) return false;
  if (!(poll_fd.events & POLLIN)) return false;
  return true;
}

void Socket::SetSocketDesriptor(int socket_descriptor) {
  socket_descriptor_ = socket_descriptor;
}

int Socket::GetSocketDescriptor() const { return socket_descriptor_; }

}  // namespace RitTest
