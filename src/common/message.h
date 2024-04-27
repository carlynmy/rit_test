#ifndef RIT_TEST_COMMON_MESSAGE_H_
#define RIT_TEST_COMMON_MESSAGE_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace RitTest {

class Message {
 public:
  enum Type {
    kTypeRequestParseText,
    kTypeRequestGetConnectionCount,
    kTypeRequestSendMessageToClient,
    kTypeAnswerParseText,
    kTypeAnswerGetConnectionCount,
    kTypeAnswerSendMessageToClient,
    kTypeSendedMessage,
  };

  struct MessageRequestParseText {
    std::string text_;
  };

  struct MessageRequestGetConnectionCount {};

  struct MessageRequestSendMessageToClient {
    std::string to_;
    std::string text_;
  };

  struct MessageAnswerParseText {
    std::vector<std::pair<std::string, std::string>> answer_;
  };

  struct MessageAnswerGetConnectionCount {
    std::vector<std::string> client_names_;
  };

  struct MessageAnswerSendMessageToClient {
    bool ok_;
  };

  struct MessageSendedMessage {
    std::string from_;
    std::string text_;
  };

  using Variant =
      std::variant<MessageRequestParseText, MessageRequestGetConnectionCount,
                   MessageRequestSendMessageToClient, MessageAnswerParseText,
                   MessageAnswerGetConnectionCount,
                   MessageAnswerSendMessageToClient, MessageSendedMessage>;

  Message();
  Message(const Message& other) = default;
  Message(Message&& other) noexcept = default;
  Message& operator=(const Message& other) = default;
  Message& operator=(Message&& other) noexcept = default;
  virtual ~Message();

  /// @return true if ok, false otherwise.
  bool FromBytes(const std::string& bytes);
  std::string ToBytes() const;

  Type GetType() const;

  static Message Create(const MessageRequestParseText& message);
  static Message Create(const MessageRequestGetConnectionCount& message);
  static Message Create(const MessageRequestSendMessageToClient& message);
  static Message Create(const MessageAnswerParseText& message);
  static Message Create(const MessageAnswerGetConnectionCount& message);
  static Message Create(const MessageAnswerSendMessageToClient& message);
  static Message Create(const MessageSendedMessage& message);

  const MessageRequestParseText* GetRequestParseText() const;
  const MessageRequestGetConnectionCount* GetRequestGetConnectionCount() const;
  const MessageRequestSendMessageToClient* GetRequestSendMessageToClient()
      const;
  const MessageAnswerParseText* GetAnswerParseText() const;
  const MessageAnswerGetConnectionCount* GetAnswerGetConnectionCount() const;
  const MessageAnswerSendMessageToClient* GetAnswerSendMessageToClient() const;
  const MessageSendedMessage* GetSendedMessage() const;

 private:
  bool FromBytesRequestParseText(const std::string& bytes);
  bool FromBytesRequestGetConnectionCount(const std::string& bytes);
  bool FromBytesRequestSendMessageToClient(const std::string& bytes);
  bool FromBytesAnswerParseText(const std::string& bytes);
  bool FromBytesAnswerGetConnectionCount(const std::string& bytes);
  bool FromBytesAnswerSendMessageToClient(const std::string& bytes);
  bool FromBytesSendedMessage(const std::string& bytes);

  std::string ToBytes(const MessageRequestParseText& message) const;
  std::string ToBytes(const MessageRequestGetConnectionCount& message) const;
  std::string ToBytes(const MessageRequestSendMessageToClient& message) const;
  std::string ToBytes(const MessageAnswerParseText& message) const;
  std::string ToBytes(const MessageAnswerGetConnectionCount& message) const;
  std::string ToBytes(const MessageAnswerSendMessageToClient& message) const;
  std::string ToBytes(const MessageSendedMessage& message) const;

  void ToBytesString(char* bytes, const std::string& str) const;
  std::string FromBytesString(const char* bytes) const;

  Type type_;
  Variant variant_;
};

}  // namespace RitTest

#endif  // RIT_TEST_COMMON_MESSAGE_H_
