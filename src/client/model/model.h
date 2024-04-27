#ifndef RIT_TEST_CLIENT_MODEL_MODEL_H_
#define RIT_TEST_CLIENT_MODEL_MODEL_H_

#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "common/client_socket.h"
#include "common/message.h"

namespace RitTest {

namespace Client {

class Model {
 public:
  enum ReturnCode {
    kCodeOk,
    kCodeNotHaveMessages,
    kCodeFailConnection,
    kCodeInvalidData,
    kCodeAleradyConnected,
    kCodeFailSend,
    kCodeServerNotResponse,
  };

  using MessageFromClient = Message::MessageSendedMessage;
  using ParseMessageRes = Message::MessageAnswerParseText;
  using ConnectionCountRes = Message::MessageAnswerGetConnectionCount;

  Model();
  Model(const Model& other) = delete;
  Model(Model&& other) noexcept = delete;
  Model& operator=(const Model& other) = delete;
  Model& operator=(Model&& other) noexcept = delete;
  virtual ~Model();

  ReturnCode ConnectToServer(const std::string& ip, const std::string& port);

  void CloseConnection();

  ReturnCode ParseMessage(const std::string& mess, ParseMessageRes& res);

  ReturnCode GetConnectionCount(ConnectionCountRes& res);

  ReturnCode SendMessageToClient(const std::string& to,
                                 const std::string& text);

  ReturnCode GetMessageFromClient(MessageFromClient& message);

 private:
  ReturnCode GetAllMessages();
  int StrToInt(const std::string& str, bool& success);
  bool IsMessageFromClient(const Message& message);
  MessageFromClient ToMessageFromClient(const Message& message);

  bool GetRes(Message::Type type, Message& ans);

  ClientSocket socket_;

  std::queue<Message> answers_from_server_;
  std::queue<MessageFromClient> messages_from_clients_;
};

}  // namespace Client

}  // namespace RitTest

#endif  // RIT_TEST_CLIENT_MODEL_MODEL_H_
