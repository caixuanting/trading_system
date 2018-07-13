// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef UTILS_FILE_H_
#define UTILS_FILE_H_

#include <fstream>
#include <string>
#include <vector>

namespace util {

std::string ReadFile(const std::string& file_name);

std::vector<std::string> ReadFileLines(const std::string& file_name);

}  // namespace util

#endif  // UTILS_FILE_H_
