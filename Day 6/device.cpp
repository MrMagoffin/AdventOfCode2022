#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

const int WINDOW = 14;

int jumpCount(std::string str, int pos) {
  for (int j = 1; j < WINDOW; j++) {
    for (int k = 0; k < j; k++) {
      if (str[pos - k] == str[pos - j]) {
        return WINDOW - j;
      }
    }
  }
  return 0;
}

int main(int argc, char* argv[]) {
  std::fstream fs("device.txt", std::fstream::in);
  std::string charStream;
  while (!fs.eof()) {
    fs >> charStream;
    if (fs.eof()) {
      break;
    }
    // find the correct starting point
    int i = WINDOW - 1;
    while (i < charStream.length()) {
      int jump = jumpCount(charStream, i);
      if (jump == 0) {
        std::cout << i+1 << std::endl;
        break;
      }
      else {
        i += jump;
      }
      /*if (charStream[i] == charStream[i-1]) {
        i += 3;
      }
      else if (charStream[i] == charStream[i-2] || charStream[i-1] == charStream[i-2]) {
        i += 2;
      }
      else if (charStream[i] == charStream[i-3] || charStream[i-1] == charStream[i-3] || charStream[i-2] == charStream[i-3]) {
        i += 1;
      }
      else {
        std::cout << i+1 << std::endl;
        break;
      }*/
    }
    if (i >= charStream.length()) {
      std::cout << "No marker found." << std::endl;
    }
  }
  fs.close();
}