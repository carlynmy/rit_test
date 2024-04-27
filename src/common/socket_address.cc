
#include "socket_address.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include <string>

namespace RitTest {

SocketAddress::SocketAddress()
    : SocketAddress(Family::kFamilyInet, 0, kAddressInAddrAny) {}

SocketAddress::SocketAddress(Family family, int port, Address ip)
    : address_(), address_len_(sizeof(sockaddr_in)) {
  SetFamily(family);
  SetPort(port);
  SetAddress(ip);
}

SocketAddress::~SocketAddress() {}

void SocketAddress::SetFamily(Family family) {
  address_.sin_family = FamilyToSaFamily(family);
}

void SocketAddress::SetPort(int port) { address_.sin_port = HostToNet(port); }

void SocketAddress::SetAddress(Address address) {
  address_.sin_addr.s_addr = address;
}

SocketAddress::Family SocketAddress::GetFamily() const {
  return SaFamilyToFamily(address_.sin_family);
}

int SocketAddress::GetPort() const { return NetToHost(address_.sin_port); }

SocketAddress::Address SocketAddress::GetAddress() const {
  return address_.sin_addr.s_addr;
}

const sockaddr_in& SocketAddress::GetSockaddrInStruct() const {
  return address_;
}

socklen_t SocketAddress::GetSocklen() const { return address_len_; }

SocketAddress::Address SocketAddress::StringToAddress(const std::string& str) {
  hostent* host = gethostbyname(str.c_str());
  return inet_addr(inet_ntoa(*reinterpret_cast<in_addr*>(*host->h_addr_list)));
}

void SocketAddress::SetSockaddrInStruct(const sockaddr_in& addr) {
  address_ = addr;
}

void SocketAddress::SetSocklen(socklen_t len) { address_len_ = len; }

const SocketAddress::Address SocketAddress::kAddressInAddrAny = INADDR_ANY;

sa_family_t SocketAddress::FamilyToSaFamily(Family family) {
  switch (family) {
    case Family::kFamilyInet:
      return AF_INET;
      break;
    default:
      break;
  }
  return sa_family_t();
}

SocketAddress::Family SocketAddress::SaFamilyToFamily(sa_family_t sa_family) {
  switch (sa_family) {
    case AF_INET:
      return Family::kFamilyInet;
      break;
    default:
      break;
  }
  return Family();
}

int SocketAddress::HostToNet(int host) { return htons(host); }

int SocketAddress::NetToHost(int net) { return ntohs(net); }

}  // namespace RitTest
