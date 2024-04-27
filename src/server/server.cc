#include "server.h"

#include <atomic>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <utility>

#include "common/client_socket.h"
#include "common/server_socket.h"
#include "common/socket_address.h"

namespace RitTest {

namespace ServerNS {

Server::Server() {}
Server::~Server() {}

Server::ReturnCode Server::Start(int port) {
  SocketAddress addr(SocketAddress::Family::kFamilyInet, port,
                     SocketAddress::kAddressInAddrAny);
  socket_.SetSocketAddress(addr);
  ServerSocket::ReturnCode code = socket_.Create();
  if (code != ServerSocket::ReturnCode::kCodeOk)
    return ReturnCode::kCodeFailStartServer;

  server_work_.store(true);
  std::unique_ptr<std::thread> task_thread = StartListenTasks();

  ListenUserInput();

  EndListenTasks(task_thread);

  return ReturnCode::kCodeOk;
}

std::unique_ptr<std::thread> Server::StartListenTasks() {
  return std::unique_ptr<std::thread>(
      new std::thread(&Server::ListenTasks, this));
}

void Server::EndListenTasks(std::unique_ptr<std::thread>& thread) {
  thread->join();
}

void Server::ListenTasks() {
  socket_.ReadyToConnect();

  while (server_work_.load()) {
    ClientSocket client;
    ServerSocket::ReturnCode code = socket_.Connect(client);
    if (code == ServerSocket::ReturnCode::kCodeOk) {
      clients_.push_back(std::move(client));
    }

    for (auto i = clients_.begin(); i != clients_.end(); ++i) {
      Message message;
      ClientSocket::ReturnCode code = i->Recv(message);
      if (code == ClientSocket::ReturnCode::kCodeNoMessages) continue;
      if (code != ClientSocket::ReturnCode::kCodeOk) {
        clients_.erase(i);
        break;
      }

      DoTask(message, *i);
    }
  }
}

void Server::ListenUserInput() {
  std::cout << "Server is start" << std::endl;
  std::string str;
  while (std::cin) {
    std::cout << "Enter \"exit\" to exit." << std::endl;
    std::cin >> str;
    if (str == kExitWord) break;
  }

  server_work_.store(false);
}

void Server::DoTask(const Message& message, ClientSocket& client) {
  switch (message.GetType()) {
    case Message::Type::kTypeRequestGetConnectionCount:
      DoTaskGetConnectionCount(client);
      break;
    case Message::Type::kTypeRequestParseText:
      DoTaskParserText(client, message);
      break;
    case Message::Type::kTypeRequestSendMessageToClient:
      DoTaskSendMessage(client, message);
      break;

    default:
      break;
  }
}

void Server::DoTaskGetConnectionCount(ClientSocket& client) {
  Message::MessageAnswerGetConnectionCount ans;

  ans.client_names_.push_back(client.GetName());
  for (auto i = clients_.begin(); i != clients_.end(); ++i) {
    if (&*i == &client) continue;

    ans.client_names_.push_back(i->GetName());
  }

  Message rmes = Message::Create(ans);
  client.Send(rmes);
}

void Server::DoTaskParserText(ClientSocket& client, const Message& message) {
  auto message_detail = message.GetRequestParseText();
  Message::MessageAnswerParseText ans;
  ans.answer_ = ParseText(message_detail->text_);
  Message rmes = Message::Create(ans);
  client.Send(rmes);
}

void Server::DoTaskSendMessage(ClientSocket& client, const Message& message) {
  auto message_detail = message.GetRequestSendMessageToClient();
  ClientSocket* to_client = FindClient(message_detail->to_);

  if (to_client == nullptr) {
    Message::MessageAnswerSendMessageToClient ans;
    ans.ok_ = false;
    Message rmes = Message::Create(ans);
    client.Send(rmes);
    return;
  }

  Message::MessageSendedMessage ans;
  ans.from_ = client.GetName();
  ans.text_ = message_detail->text_;
  Message rmes = Message::Create(ans);
  auto code = to_client->Send(rmes);

  Message::MessageAnswerSendMessageToClient ans2;
  ans2.ok_ = code == Socket::ReturnCode::kCodeOk;
  Message rmes2 = Message::Create(ans2);
  client.Send(rmes2);
}

ClientSocket* Server::FindClient(const std::string& name) {
  for (auto i = clients_.begin(); i != clients_.end(); ++i) {
    if (i->GetName() == name) return &*i;
  }
  return nullptr;
}

Server::ParseTextRes Server::ParseText(const std::string& text) const {
  std::vector<char> tmp_pos;
  std::map<char, int> tmp_map;

  for (int i = 0; i < static_cast<int>(text.length()); ++i) {
    auto map_iter = tmp_map.find(text[i]);
    if (map_iter == tmp_map.end()) {
      tmp_pos.push_back(text[i]);
      tmp_map.insert({text[i], 1});

    } else {
      ++map_iter->second;
    }
  }

  ParseTextRes res;
  res.push_back({"Message", text});

  for (auto i : tmp_pos) {
    int count = tmp_map.find(i)->second;
    std::string ch;
    ch.push_back(i);
    res.push_back({ch, std::to_string(count)});
  }

  return res;
}

const std::string Server::kExitWord = "exit";

}  // namespace ServerNS

}  // namespace RitTest
