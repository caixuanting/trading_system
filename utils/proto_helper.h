// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef UTILS_PROTO_HELPER_H_
#define UTILS_PROTO_HELPER_H_

#include <google/protobuf/text_format.h>

namespace util {

struct Value {
  Value(const std::string& value) : value_(value) {}

  ~Value() {}

  template <typename ProtoType>
  operator ProtoType() const {
    ProtoType proto_type;

    google::protobuf::TextFormat::ParseFromString(value_, &proto_type);
    return proto_type;
  }

  std::string value_;
};

Value ProtoFromString(const std::string& proto_string);

template<class ProtoType> std::string ToLineString(const ProtoType& message) {
        google::protobuf::TextFormat::Printer printer;
  printer.SetSingleLineMode(true);

  std::string proto_in_line_string;
  printer.PrintToString(message, &proto_in_line_string);

  return proto_in_line_string;
}

}  // namespace util

#endif  // UTILS_PROTO_HELPER_H_
