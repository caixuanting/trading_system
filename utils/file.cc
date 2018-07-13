// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include <glog/logging.h>

#include "utils/file.h"

using std::ifstream;
using std::istreambuf_iterator;
using std::string;
using std::vector;

namespace util {

string ReadFile(const string& file_name) {
  ifstream file(file_name);

  if (!file.is_open()) {
    LOG(ERROR) << "Fail to open file " << file_name;
    return "";
  }

  LOG(INFO) << "Opened file <" << file_name << ">";

  string content((istreambuf_iterator<char>(file)),
                 istreambuf_iterator<char>());

  return content;
}

vector<string> ReadFileLines(const string& file_name) {
  vector<string> lines;

  ifstream file(file_name);

  if (!file.is_open()) {
    LOG(ERROR) << "Fail to open file " << file_name;
    return lines;
  }

  LOG(INFO) << "Opened file <" << file_name << ">";

  string line;

  while (getline(file, line)) {
    lines.push_back(line);
  }

  return lines;
}

}  // namespace util
