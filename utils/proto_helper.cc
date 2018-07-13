// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "utils/proto_helper.h"

using std::string;

namespace util {

Value ProtoFromString(const string& proto_string) { 
	return Value(proto_string); 
}

}  // namespace util
