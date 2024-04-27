#include "message.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace RitTest {

Message::Message()
    : type_(Type::kTypeRequestGetConnectionCount),
      variant_(MessageRequestGetConnectionCount()) {}

Message::~Message() {}

bool Message::FromBytes(const std::string& bytes) {
  const char* pbytes = bytes.data();
  int32_t type = *reinterpret_cast<const int32_t*>(pbytes + 4);
  type_ = static_cast<Type>(type);

  switch (type_) {
    case Type::kTypeRequestParseText:
      return FromBytesRequestParseText(bytes);
      break;
    case Type::kTypeRequestGetConnectionCount:
      return FromBytesRequestGetConnectionCount(bytes);
      break;
    case Type::kTypeRequestSendMessageToClient:
      return FromBytesRequestSendMessageToClient(bytes);
      break;
    case Type::kTypeAnswerParseText:
      return FromBytesAnswerParseText(bytes);
      break;
    case Type::kTypeAnswerGetConnectionCount:
      return FromBytesAnswerGetConnectionCount(bytes);
      break;
    case Type::kTypeAnswerSendMessageToClient:
      return FromBytesAnswerSendMessageToClient(bytes);
      break;
    case Type::kTypeSendedMessage:
      return FromBytesSendedMessage(bytes);
      break;

    default:
      break;
  }

  return false;
}

std::string Message::ToBytes() const {
  switch (type_) {
    case Type::kTypeRequestParseText:
      return ToBytes(*GetRequestParseText());
      break;
    case Type::kTypeRequestGetConnectionCount:
      return ToBytes(*GetRequestGetConnectionCount());
      break;
    case Type::kTypeRequestSendMessageToClient:
      return ToBytes(*GetRequestSendMessageToClient());
      break;
    case Type::kTypeAnswerParseText:
      return ToBytes(*GetAnswerParseText());
      break;
    case Type::kTypeAnswerGetConnectionCount:
      return ToBytes(*GetAnswerGetConnectionCount());
      break;
    case Type::kTypeAnswerSendMessageToClient:
      return ToBytes(*GetAnswerSendMessageToClient());
      break;
    case Type::kTypeSendedMessage:
      return ToBytes(*GetSendedMessage());
      break;

    default:
      break;
  }

  return "";
}

Message::Type Message::GetType() const { return type_; }

Message Message::Create(const MessageRequestParseText& message) {
  Message m;
  m.type_ = Type::kTypeRequestParseText;
  m.variant_ = message;
  return m;
}

Message Message::Create(const MessageRequestGetConnectionCount& message) {
  Message m;
  m.type_ = Type::kTypeRequestGetConnectionCount;
  m.variant_ = message;
  return m;
}

Message Message::Create(const MessageRequestSendMessageToClient& message) {
  Message m;
  m.type_ = Type::kTypeRequestSendMessageToClient;
  m.variant_ = message;
  return m;
}

Message Message::Create(const MessageAnswerParseText& message) {
  Message m;
  m.type_ = Type::kTypeAnswerParseText;
  m.variant_ = message;
  return m;
}

Message Message::Create(const MessageAnswerGetConnectionCount& message) {
  Message m;
  m.type_ = Type::kTypeAnswerGetConnectionCount;
  m.variant_ = message;
  return m;
}

Message Message::Create(const MessageAnswerSendMessageToClient& message) {
  Message m;
  m.type_ = Type::kTypeAnswerSendMessageToClient;
  m.variant_ = message;
  return m;
}

Message Message::Create(const MessageSendedMessage& message) {
  Message m;
  m.type_ = Type::kTypeSendedMessage;
  m.variant_ = message;
  return m;
}

const Message::MessageRequestParseText* Message::GetRequestParseText() const {
  try {
    return &std::get<MessageRequestParseText>(variant_);
  } catch (...) {
  }

  return nullptr;
}

const Message::MessageRequestGetConnectionCount*
Message::GetRequestGetConnectionCount() const {
  try {
    return &std::get<MessageRequestGetConnectionCount>(variant_);
  } catch (...) {
  }

  return nullptr;
}

const Message::MessageRequestSendMessageToClient*
Message::GetRequestSendMessageToClient() const {
  try {
    return &std::get<MessageRequestSendMessageToClient>(variant_);
  } catch (...) {
  }

  return nullptr;
}

const Message::MessageAnswerParseText* Message::GetAnswerParseText() const {
  try {
    return &std::get<MessageAnswerParseText>(variant_);
  } catch (...) {
  }

  return nullptr;
}

const Message::MessageAnswerGetConnectionCount*
Message::GetAnswerGetConnectionCount() const {
  try {
    return &std::get<MessageAnswerGetConnectionCount>(variant_);
  } catch (...) {
  }

  return nullptr;
}

const Message::MessageAnswerSendMessageToClient*
Message::GetAnswerSendMessageToClient() const {
  try {
    return &std::get<MessageAnswerSendMessageToClient>(variant_);
  } catch (...) {
  }

  return nullptr;
}

const Message::MessageSendedMessage* Message::GetSendedMessage() const {
  try {
    return &std::get<MessageSendedMessage>(variant_);
  } catch (...) {
  }

  return nullptr;
}

bool Message::FromBytesRequestParseText(const std::string& bytes) {
  const char* p = bytes.data() + 8;
  MessageRequestParseText message;

  message.text_ = FromBytesString(p);

  variant_ = message;
  return true;
}

bool Message::FromBytesRequestGetConnectionCount(const std::string& bytes) {
  (void)bytes;
  MessageRequestGetConnectionCount message;

  variant_ = message;
  return true;
}

bool Message::FromBytesRequestSendMessageToClient(const std::string& bytes) {
  const char* p = bytes.data() + 8;
  MessageRequestSendMessageToClient message;

  message.to_ = FromBytesString(p);
  p += 4 + message.to_.length();

  message.text_ = FromBytesString(p);
  p += 4 + message.text_.length();

  variant_ = message;
  return true;
}

bool Message::FromBytesAnswerParseText(const std::string& bytes) {
  const char* p = bytes.data() + 8;
  MessageAnswerParseText message;

  while (p < bytes.data() + bytes.length()) {
    std::string str = FromBytesString(p);
    p += 4 + str.length();
    std::string str2 = FromBytesString(p);
    p += 4 + str2.length();
    message.answer_.push_back({str, str2});
  }

  variant_ = message;
  return true;
}

bool Message::FromBytesAnswerGetConnectionCount(const std::string& bytes) {
  const char* p = bytes.data() + 8;
  MessageAnswerGetConnectionCount message;

  while (p < bytes.data() + bytes.length()) {
    std::string str = FromBytesString(p);
    p += 4 + str.length();
    message.client_names_.push_back(str);
  }

  variant_ = message;
  return true;
}

bool Message::FromBytesAnswerSendMessageToClient(const std::string& bytes) {
  const char* p = bytes.data() + 8;
  MessageAnswerSendMessageToClient message;

  message.ok_ = static_cast<bool>(*p);
  p += 1;

  variant_ = message;
  return true;
}

bool Message::FromBytesSendedMessage(const std::string& bytes) {
  const char* p = bytes.data() + 8;
  MessageSendedMessage message;

  message.from_ = FromBytesString(p);
  p += 4 + message.from_.length();

  message.text_ = FromBytesString(p);
  p += 4 + message.text_.length();

  variant_ = message;
  return true;
}

std::string Message::ToBytes(const MessageRequestParseText& message) const {
  int32_t type = static_cast<int32_t>(Type::kTypeRequestParseText);
  std::string::size_type total_size = 8;
  total_size += 4 + message.text_.length();

  std::string bytes;
  bytes.resize(total_size);
  char* p = bytes.data();

  int32_t size = static_cast<int32_t>(total_size);
  *(reinterpret_cast<int32_t*>(p)) = size;
  p += 4;

  *(reinterpret_cast<int32_t*>(p)) = type;
  p += 4;

  ToBytesString(p, message.text_);
  p += 4 + message.text_.length();

  return bytes;
}

std::string Message::ToBytes(
    const MessageRequestGetConnectionCount& message) const {
  (void)message;
  int32_t type = static_cast<int32_t>(Type::kTypeRequestGetConnectionCount);
  std::string::size_type total_size = 8;

  std::string bytes;
  bytes.resize(total_size);
  char* p = bytes.data();

  int32_t size = static_cast<int32_t>(total_size);
  *(reinterpret_cast<int32_t*>(p)) = size;
  p += 4;

  *(reinterpret_cast<int32_t*>(p)) = type;
  p += 4;

  return bytes;
}

std::string Message::ToBytes(
    const MessageRequestSendMessageToClient& message) const {
  int32_t type = static_cast<int32_t>(Type::kTypeRequestSendMessageToClient);
  std::string::size_type total_size = 8;
  total_size += 4 + message.to_.length();
  total_size += 4 + message.text_.length();

  std::string bytes;
  bytes.resize(total_size);
  char* p = bytes.data();

  int32_t size = static_cast<int32_t>(total_size);
  *(reinterpret_cast<int32_t*>(p)) = size;
  p += 4;

  *(reinterpret_cast<int32_t*>(p)) = type;
  p += 4;

  ToBytesString(p, message.to_);
  p += 4 + message.to_.length();

  ToBytesString(p, message.text_);
  p += 4 + message.text_.length();

  return bytes;
}

std::string Message::ToBytes(const MessageAnswerParseText& message) const {
  int32_t type = static_cast<int32_t>(Type::kTypeAnswerParseText);
  std::string::size_type total_size = 8;
  for (auto i : message.answer_) {
    total_size += 4 + i.first.length();
    total_size += 4 + i.second.length();
  }

  std::string bytes;
  bytes.resize(total_size);
  char* p = bytes.data();

  int32_t size = static_cast<int32_t>(total_size);
  *(reinterpret_cast<int32_t*>(p)) = size;
  p += 4;

  *(reinterpret_cast<int32_t*>(p)) = type;
  p += 4;

  for (auto i : message.answer_) {
    ToBytesString(p, i.first);
    p += 4 + i.first.length();
    ToBytesString(p, i.second);
    p += 4 + i.second.length();
  }

  return bytes;
}

std::string Message::ToBytes(
    const MessageAnswerGetConnectionCount& message) const {
  int32_t type = static_cast<int32_t>(Type::kTypeAnswerGetConnectionCount);
  std::string::size_type total_size = 8;
  for (auto i : message.client_names_) {
    total_size += 4 + i.length();
  }

  std::string bytes;
  bytes.resize(total_size);
  char* p = bytes.data();

  int32_t size = static_cast<int32_t>(total_size);
  *(reinterpret_cast<int32_t*>(p)) = size;
  p += 4;

  *(reinterpret_cast<int32_t*>(p)) = type;
  p += 4;

  for (auto i : message.client_names_) {
    ToBytesString(p, i);
    p += 4 + i.length();
  }

  return bytes;
}

std::string Message::ToBytes(
    const MessageAnswerSendMessageToClient& message) const {
  int32_t type = static_cast<int32_t>(Type::kTypeAnswerSendMessageToClient);
  std::string::size_type total_size = 9;

  std::string bytes;
  bytes.resize(total_size);
  char* p = bytes.data();

  int32_t size = static_cast<int32_t>(total_size);
  *(reinterpret_cast<int32_t*>(p)) = size;
  p += 4;

  *(reinterpret_cast<int32_t*>(p)) = type;
  p += 4;

  *p = static_cast<char>(message.ok_);
  p += 1;

  return bytes;
}

std::string Message::ToBytes(const MessageSendedMessage& message) const {
  int32_t type = static_cast<int32_t>(Type::kTypeSendedMessage);
  std::string::size_type total_size = 8;
  total_size += 4 + message.from_.length();
  total_size += 4 + message.text_.length();

  std::string bytes;
  bytes.resize(total_size);
  char* p = bytes.data();

  int32_t size = static_cast<int32_t>(total_size);
  *(reinterpret_cast<int32_t*>(p)) = size;
  p += 4;

  *(reinterpret_cast<int32_t*>(p)) = type;
  p += 4;

  ToBytesString(p, message.from_);
  p += 4 + message.from_.length();

  ToBytesString(p, message.text_);
  p += 4 + message.text_.length();

  return bytes;
}

void Message::ToBytesString(char* bytes, const std::string& str) const {
  int32_t size = static_cast<int32_t>(str.length());
  *reinterpret_cast<int32_t*>(bytes) = size;
  bytes += 4;

  for (auto i : str) {
    *bytes = i;
    ++bytes;
  }
}

std::string Message::FromBytesString(const char* bytes) const {
  int32_t size = *reinterpret_cast<const int32_t*>(bytes);
  bytes += 4;

  std::string str;
  str.resize(size);
  for (int32_t i = 0; i < size; ++i) {
    str[i] = *bytes;
    ++bytes;
  }

  return str;
}

}  // namespace RitTest
