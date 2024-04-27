#ifndef RIT_TEST_CLIENT_CONTROLLER_CONTROLLER_H_
#define RIT_TEST_CLIENT_CONTROLLER_CONTROLLER_H_

#include <string>

#include "client/model/model.h"

namespace RitTest {

namespace Client {

class Controller {
 public:
  using ReturnCode = Model::ReturnCode;

  using MessageFromClient = Model::MessageFromClient;
  using ParseMessageRes = Model::ParseMessageRes;
  using ConnectionCountRes = Model::ConnectionCountRes;

  Controller(Model* model);
  Controller(const Controller& other) = default;
  Controller(Controller&& other) noexcept = default;
  Controller& operator=(const Controller& other) = default;
  Controller& operator=(Controller&& other) noexcept = default;
  virtual ~Controller();

  ReturnCode ConnectToServer(const std::string& ip, const std::string& port);

  void CloseConnection();

  ReturnCode ParseMessage(const std::string& mess, ParseMessageRes& res);

  ReturnCode GetConnectionCount(ConnectionCountRes& res);

  ReturnCode SendMessageToClient(const std::string& to,
                                 const std::string& text);

  ReturnCode GetMessageFromClient(MessageFromClient& message);

 private:
  Model* model_;
};

}  // namespace Client

}  // namespace RitTest

#endif  // RIT_TEST_CLIENT_CONTROLLER_CONTROLLER_H_
