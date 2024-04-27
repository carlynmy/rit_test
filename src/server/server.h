#ifndef RIT_TEST_SERVER_SERVER_H_
#define RIT_TEST_SERVER_SERVER_H_

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "common/client_socket.h"
#include "common/server_socket.h"

namespace RitTest {

namespace ServerNS {

class Server {
 public:
  enum ReturnCode {
    kCodeOk,
    kCodeFailStartServer,
  };

  Server();
  Server(const Server& other);
  Server(Server&& other) noexcept;
  Server& operator=(const Server& other);
  Server& operator=(Server&& other) noexcept;
  virtual ~Server();

  ReturnCode Start(int port);

 private:
  using ParseTextRes = std::vector<std::pair<std::string, std::string>>;

  std::unique_ptr<std::thread> StartListenTasks();
  void EndListenTasks(std::unique_ptr<std::thread>& thread);

  void ListenTasks();
  void ListenUserInput();

  void DoTask(const Message& message, ClientSocket& client);
  void DoTaskGetConnectionCount(ClientSocket& client);
  void DoTaskParserText(ClientSocket& client, const Message& message);
  void DoTaskSendMessage(ClientSocket& client, const Message& message);

  ClientSocket* FindClient(const std::string& name);
  ParseTextRes ParseText(const std::string& text) const;

  static const std::string kExitWord;

  ServerSocket socket_;

  std::atomic<bool> server_work_;

  std::list<ClientSocket> clients_;
};

}  // namespace ServerNS

}  // namespace RitTest

#endif  // RIT_TEST_SERVER_SERVER_H_
