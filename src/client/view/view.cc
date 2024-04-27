
#include "view.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "client/controller/controller.h"

namespace RitTest {

namespace Client {

View::View(Controller* controller)
    : controller_(controller), need_close_(true) {}

View::~View() {}

int View::Exec(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Usage: ./client ip port" << std::endl;
    return 1;
  }

  std::string ip(argv[1]);
  std::string port(argv[2]);
  ip_ = ip;
  port_ = port;

  Controller::ReturnCode code = controller_->ConnectToServer(ip, port);
  if (code != Controller::ReturnCode::kCodeOk) {
    PrintErrByCode(code);
    return 1;
  }

  PrintHelloMsg();
  need_close_ = false;
  return GameLoop();
}

int View::GameLoop() {
  while (!need_close_) {
    DoGetInMsgs(false);

    PrintSelectAction();

    Action action = NextAction();
    switch (action) {
      case Action::kActionGetInMsgs:
        DoGetInMsgs(true);
        break;
      case Action::kActionParseText:
        DoParseText();
        break;
      case Action::kActionSendMsg:
        DoSendMsg();
        break;
      case Action::kActionGetConnectionsCount:
        DooGetConnectionCount();
        break;
      case Action::kActionExit:
        DoExit();
        break;
      default:
        PrintUndefinedAction();
        break;
    }
  }

  return 0;
}

void View::Recconect() {
  const int count = 5;
  const int timout = 2000;
  for (int i = 1; i <= count; ++i) {
    std::cout << "Trying recconect (" << i << "/" << count << ")" << std::endl;
    auto code = controller_->ConnectToServer(ip_, port_);
    if (code == Controller::ReturnCode::kCodeOk) {
      PrintHelloMsg();
      break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(timout));
  }

  std::cout << "Fail recconect to server" << std::endl;
  this->need_close_ = true;
}

View::Action View::NextAction() const {
  if (!std::cin) return Action::kActionExit;

  std::string inp;
  std::cin >> inp;

  if (inp == "1") return Action::kActionGetInMsgs;
  if (inp == "2") return Action::kActionGetConnectionsCount;
  if (inp == "3") return Action::kActionSendMsg;
  if (inp == "4") return Action::kActionParseText;
  if (inp == "5") return Action::kActionExit;

  return Action::kActionUndefined;
}

void View::DoGetInMsgs(bool say_if_no) {
  Controller::MessageFromClient msg;
  Controller::ReturnCode code = controller_->GetMessageFromClient(msg);

  if (code == Controller::ReturnCode::kCodeServerNotResponse) {
    Recconect();
    return;
  } else if (code != Controller::ReturnCode::kCodeOk) {
    if (say_if_no) PrintErrByCode(code);
    return;
  }

  do {
    PrintMsgFrom(msg.from_, msg.text_);

    code = controller_->GetMessageFromClient(msg);

  } while (code == Controller::ReturnCode::kCodeOk);
}

void View::DooGetConnectionCount() {
  Controller::ConnectionCountRes res;
  Controller::ReturnCode code = controller_->GetConnectionCount(res);

  if (code == Controller::ReturnCode::kCodeServerNotResponse) {
    Recconect();
    return;
  } else if (code != Controller::ReturnCode::kCodeOk) {
    PrintErrByCode(code);
    return;
  }

  PrintConnectionCount(res);
}

void View::DoSendMsg() {
  PrintAskClientName();
  std::string to;
  std::getline(std::cin, to);
  std::getline(std::cin, to);

  PrintAskText();
  std::string text;
  std::getline(std::cin, text);

  Controller::ReturnCode code = controller_->SendMessageToClient(to, text);
  if (code == Controller::ReturnCode::kCodeServerNotResponse) {
    Recconect();
    return;
  } else if (code != Controller::ReturnCode::kCodeOk) {
    PrintErrByCode(code);
    return;
  }

  PrintMsgSendSuccess();
}

void View::DoParseText() {
  PrintAskText();
  std::string text;
  std::getline(std::cin, text);
  std::getline(std::cin, text);

  Controller::ParseMessageRes res;
  Controller::ReturnCode code = controller_->ParseMessage(text, res);

  if (code == Controller::ReturnCode::kCodeServerNotResponse) {
    Recconect();
    return;
  } else if (code != Controller::ReturnCode::kCodeOk) {
    PrintErrByCode(code);
    return;
  }

  PrintParseText(res);
}

void View::DoExit() { need_close_ = true; }

void View::PrintConnectionCount(const Controller::ConnectionCountRes& res) {
  std::cout << std::endl;
  std::cout << "Count: " << res.client_names_.size() << std::endl;
  std::cout << "Names:" << std::endl;
  std::cout << "  " << res.client_names_[0] << " -- you" << std::endl;

  auto i = res.client_names_.begin();
  ++i;
  for (; i != res.client_names_.end(); ++i)
    std::cout << "  " << *i << std::endl;
}

void View::PrintAskText() { std::cout << "Enter text" << std::endl; }

void View::PrintAskClientName() {
  std::cout << "Enter the recipient's name" << std::endl;
}

void View::PrintParseText(const Controller::ParseMessageRes& res) {
  std::cout << std::endl;

  int size = res.answer_[0].first.size();

  for (auto i : res.answer_) {
    std::cout << "  " << i.first << " ";
    for (int j = i.first.size(); j < size; ++j) std::cout << " ";
    std::cout << i.second << std::endl;
  }
}

void View::PrintMsgSendSuccess() {
  std::cout << std::endl;
  std::cout << "Message send successful" << std::endl;
}

void View::PrintHelloMsg() {
  std::cout << "Connection to server successful" << std::endl;
}

void View::PrintSelectAction() {
  std::cout << std::endl;
  std::cout << "Choose an action:" << std::endl;
  std::cout << "  1: Check incoming messages" << std::endl;
  std::cout << "  2: Get number of connections and client names" << std::endl;
  std::cout << "  3: Send a message to the client" << std::endl;
  std::cout << "  4: Parse text" << std::endl;
  std::cout << "  5: Exit" << std::endl;
}

void View::PrintErrByCode(Controller::ReturnCode code) {
  switch (code) {
    case Controller::ReturnCode::kCodeNotHaveMessages:
      std::cout << "No incoming messages" << std::endl;
      break;
    case Controller::ReturnCode::kCodeFailConnection:
      std::cout << "Fail connect to server" << std::endl;
      break;
    case Controller::ReturnCode::kCodeInvalidData:
      std::cout << "Wrong data" << std::endl;
      break;
    case Controller::ReturnCode::kCodeAleradyConnected:
      std::cout << "wee" << std::endl;
      break;
    case Controller::ReturnCode::kCodeFailSend:
      std::cout << "Fail sending message" << std::endl;
      break;
    case Controller::ReturnCode::kCodeServerNotResponse:
      std::cout << "Server not response" << std::endl;
      break;

    default:
      break;
  }
}

void View::PrintUndefinedAction() { std::cout << "Wrong action." << std::endl; }

void View::PrintMsgFrom(const std::string from, const std::string text) {
  std::cout << "Incoming message from: " << from << std::endl;
  std::cout << "Text: " << text << std::endl;
}

}  // namespace Client

}  // namespace RitTest
