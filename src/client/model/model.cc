#include "model.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "common/client_socket.h"
#include "common/message.h"
#include "common/socket_address.h"

namespace RitTest {

namespace Client {

Model::Model() : socket_(), answers_from_server_(), messages_from_clients_() {}

Model::~Model() {
  if (socket_.IsCreated()) CloseConnection();
}

Model::ReturnCode Model::ConnectToServer(const std::string& ip,
                                         const std::string& port) {
  if (socket_.IsCreated()) return ReturnCode::kCodeAleradyConnected;

  bool success = false;
  int int_port = StrToInt(port, success);
  if (!success) return ReturnCode::kCodeInvalidData;

  socket_ = std::move(ClientSocket());
  SocketAddress addr(SocketAddress::Family::kFamilyInet, int_port,
                     SocketAddress::StringToAddress(ip));

  socket_.SetSocketAddress(addr);
  ClientSocket::ReturnCode code = socket_.Create();
  if (code != ClientSocket::ReturnCode::kCodeOk)
    return ReturnCode::kCodeFailConnection;

  return ReturnCode::kCodeOk;
}

void Model::CloseConnection() { socket_.Delete(); }

Model::ReturnCode Model::ParseMessage(const std::string& mess,
                                      ParseMessageRes& res) {
  if (GetAllMessages() == ReturnCode::kCodeServerNotResponse) {
    CloseConnection();
    return ReturnCode::kCodeServerNotResponse;
  }

  Message::MessageRequestParseText reqm;
  reqm.text_ = mess;
  Message req = Message::Create(reqm);
  ClientSocket::ReturnCode code = socket_.Send(req);
  if (code != ClientSocket::ReturnCode::kCodeOk)
    return ReturnCode::kCodeFailSend;

  Message ans;
  bool status = GetRes(Message::Type::kTypeAnswerParseText, ans);
  if (!status) return ReturnCode::kCodeServerNotResponse;

  res = *ans.GetAnswerParseText();

  return ReturnCode::kCodeOk;
}

Model::ReturnCode Model::GetConnectionCount(ConnectionCountRes& res) {
  if (GetAllMessages() == ReturnCode::kCodeServerNotResponse) {
    CloseConnection();
    return ReturnCode::kCodeServerNotResponse;
  }

  Message::MessageRequestGetConnectionCount reqm;
  Message req = Message::Create(reqm);
  ClientSocket::ReturnCode code = socket_.Send(req);
  if (code != ClientSocket::ReturnCode::kCodeOk)
    return ReturnCode::kCodeFailSend;

  Message ans;
  bool status = GetRes(Message::Type::kTypeAnswerGetConnectionCount, ans);
  if (!status) return ReturnCode::kCodeServerNotResponse;

  res = *ans.GetAnswerGetConnectionCount();
  return ReturnCode::kCodeOk;
}

Model::ReturnCode Model::SendMessageToClient(const std::string& to,
                                             const std::string& text) {
  if (GetAllMessages() == ReturnCode::kCodeServerNotResponse) {
    CloseConnection();
    return ReturnCode::kCodeServerNotResponse;
  }

  Message::MessageRequestSendMessageToClient reqm;
  reqm.to_ = to;
  reqm.text_ = text;
  Message req = Message::Create(reqm);
  ClientSocket::ReturnCode code = socket_.Send(req);
  if (code != ClientSocket::ReturnCode::kCodeOk)
    return ReturnCode::kCodeFailSend;

  Message ans;
  bool status = GetRes(Message::Type::kTypeAnswerSendMessageToClient, ans);
  if (!status) return ReturnCode::kCodeServerNotResponse;

  auto res = *ans.GetAnswerSendMessageToClient();
  if (!res.ok_) return ReturnCode::kCodeFailSend;

  return ReturnCode::kCodeOk;
}

Model::ReturnCode Model::GetMessageFromClient(MessageFromClient& message) {
  if (GetAllMessages() == ReturnCode::kCodeServerNotResponse) {
    CloseConnection();
    return ReturnCode::kCodeServerNotResponse;
  }

  if (messages_from_clients_.empty()) {
    return ReturnCode::kCodeNotHaveMessages;
  }

  message = messages_from_clients_.front();
  messages_from_clients_.pop();
  return ReturnCode::kCodeOk;
}

Model::ReturnCode Model::GetAllMessages() {
  ClientSocket::ReturnCode code = ClientSocket::ReturnCode::kCodeOk;
  while (code == ClientSocket::ReturnCode::kCodeOk) {
    Message message;
    code = socket_.Recv(message);
    if (code == ClientSocket::ReturnCode::kCodeFailRecv)
      return ReturnCode::kCodeServerNotResponse;
    ;
    if (code != ClientSocket::ReturnCode::kCodeOk) continue;

    if (IsMessageFromClient(message)) {
      MessageFromClient message_from_client = ToMessageFromClient(message);
      messages_from_clients_.push(std::move(message_from_client));

    } else {
      answers_from_server_.push(std::move(message));
    }
  }
  return ReturnCode::kCodeOk;
}

int Model::StrToInt(const std::string& str, bool& success) {
  int res;
  size_t len;
  try {
    res = std::stoi(str, &len);
  } catch (...) {
    success = false;
    return 0;
  }

  if (len != str.length()) {
    success = false;
    return 0;
  }

  success = true;
  return res;
}

bool Model::IsMessageFromClient(const Message& message) {
  return message.GetType() == Message::Type::kTypeSendedMessage;
}

Model::MessageFromClient Model::ToMessageFromClient(const Message& message) {
  return *message.GetSendedMessage();
}

bool Model::GetRes(Message::Type type, Message& ans) {
  int trys = 0;
  while (trys != 5) {
    if (GetAllMessages() == ReturnCode::kCodeServerNotResponse) {
      CloseConnection();
      return false;
    }

    while (!answers_from_server_.empty()) {
      Message& msg = answers_from_server_.front();
      if (msg.GetType() == type) {
        ans = msg;
        answers_from_server_.pop();
        return true;
      }
      answers_from_server_.pop();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ++trys;
  }

  return false;
}

}  // namespace Client

}  // namespace RitTest
