#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

long fromSnafu(std::string str) {
  const char* chr = str.c_str()+str.size()-1;
  long curBase = 1;
  long value;
  while (chr >= str.c_str()) {
    if (*chr >= 32) {
      switch (*chr) {
      case '=':
        value -= curBase*2;
        break;
      case '-':
        value -= curBase;
        break;
      case '0':
        break;
      case '1':
        value += curBase;
        break;
      case '2':
        value += curBase*2;
        break;
      default:
        std::cout << "Unsupported character " << chr << std::endl;
        break;
      }
      curBase *= 5;
    }
    chr--;
  }
  return value;
}

std::string toSnafu(long value) {
  std::string str;
  while (value != 0) {
    long modValue = value % 5;
    switch (modValue) {
    case 0:
      str.push_back('0');
      break;
    case 1:
      str.push_back('1');
      value--;
      break;
    case 2:
      str.push_back('2');
      value -= 2;
      break;
    case 3:
      str.push_back('=');
      value += 2;
      break;
    case 4:
      str.push_back('-');
      value += 1;
      break;
    default:
      break;
    }
    value = value / 5;
  }
  std::reverse(str.begin(), str.end());
  return str;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: snafu input_file" << std::endl;
  }
  std::ifstream ifs(argv[1]);
  if (!ifs.is_open()) {
    std::cout << "Failed to open " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }
  std::string str;
  long sum = 0;
  while (std::getline(ifs, str)) {
    long value = fromSnafu(str);
    std::cout << value << std::endl;
    sum += value;
  }
  std::cout << "Sum: " << sum << std::endl;
  std::cout << "Snafu: " << toSnafu(sum) << std::endl;
  
  return EXIT_SUCCESS;
}