#include "controller.h"

#include <string>

#include "client/model/model.h"

namespace RitTest {

namespace Client {

Controller::Controller(Model* model) : model_(model) {}

Controller::~Controller() {}

void Controller::CloseConnection() { model_->CloseConnection(); }

Controller::ReturnCode Controller::ConnectToServer(const std::string& ip,
                                                   const std::string& port) {
  return model_->ConnectToServer(ip, port);
}

Controller::ReturnCode Controller::ParseMessage(const std::string& mess,
                                                ParseMessageRes& res) {
  return model_->ParseMessage(mess, res);
}

Controller::ReturnCode Controller::GetConnectionCount(ConnectionCountRes& res) {
  return model_->GetConnectionCount(res);
}

Controller::ReturnCode Controller::SendMessageToClient(
    const std::string& to, const std::string& text) {
  return model_->SendMessageToClient(to, text);
}

Controller::ReturnCode Controller::GetMessageFromClient(
    MessageFromClient& message) {
  return model_->GetMessageFromClient(message);
}

}  // namespace Client

}  // namespace RitTest
