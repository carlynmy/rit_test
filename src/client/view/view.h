#ifndef RIT_TEST_CLIENT_VIEW_VIEW_H_
#define RIT_TEST_CLIENT_VIEW_VIEW_H_

#include <string>

#include "client/controller/controller.h"

namespace RitTest {

namespace Client {

class View {
 public:
  View(Controller* controller);
  View(const View& other) = default;
  View(View&& other) noexcept = default;
  View& operator=(const View& other) = default;
  View& operator=(View&& other) noexcept = default;
  virtual ~View();

  int Exec(int argc, char** argv);

 private:
  enum Action {
    kActionUndefined,
    kActionExit,
    kActionGetInMsgs,
    kActionGetConnectionsCount,
    kActionSendMsg,
    kActionParseText,
  };

  int GameLoop();
  void Recconect();
  Action NextAction() const;
  void DoGetInMsgs(bool say_if_no);
  void DooGetConnectionCount();
  void DoSendMsg();
  void DoParseText();
  void DoExit();

  void PrintConnectionCount(const Controller::ConnectionCountRes& res);
  void PrintAskText();
  void PrintAskClientName();
  void PrintParseText(const Controller::ParseMessageRes& res);
  void PrintMsgSendSuccess();

  void PrintHelloMsg();
  void PrintSelectAction();
  void PrintErrByCode(Controller::ReturnCode code);
  void PrintUndefinedAction();
  void PrintMsgFrom(const std::string from, const std::string text);

  Controller* controller_;
  bool need_close_;
  std::string ip_;
  std::string port_;
};

}  // namespace Client

}  // namespace RitTest

#endif  // RIT_TEST_CLIENT_VIEW_VIEW_H_
