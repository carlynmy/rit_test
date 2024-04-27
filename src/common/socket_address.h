#ifndef RIT_TEST_COMMON_SOCKET_ADRESS_H_
#define RIT_TEST_COMMON_SOCKET_ADRESS_H_

#include <netinet/in.h>

#include <string>

namespace RitTest {

class SocketAddress {
 public:
  enum Family { kFamilyInet };
  using Address = in_addr_t;

  SocketAddress();
  SocketAddress(Family family, int port, Address ip);
  SocketAddress(const SocketAddress& other) = default;
  SocketAddress(SocketAddress&& other) noexcept = default;
  SocketAddress& operator=(const SocketAddress& other) = default;
  SocketAddress& operator=(SocketAddress&& other) noexcept = default;
  virtual ~SocketAddress();

  void SetFamily(Family family);
  void SetPort(int port);
  void SetAddress(Address address);

  Family GetFamily() const;
  int GetPort() const;
  Address GetAddress() const;

  const sockaddr_in& GetSockaddrInStruct() const;
  socklen_t GetSocklen() const;
  void SetSockaddrInStruct(const sockaddr_in& addr);
  void SetSocklen(socklen_t len);

  static Address StringToAddress(const std::string& str);

  static const Address kAddressInAddrAny;

 private:
  static sa_family_t FamilyToSaFamily(Family family);
  static Family SaFamilyToFamily(sa_family_t sa_family);
  static int HostToNet(int host);
  static int NetToHost(int net);

  sockaddr_in address_;
  socklen_t address_len_;
};

}  // namespace RitTest

#endif  // RIT_TEST_COMMON_SOCKET_ADRESS_H_
