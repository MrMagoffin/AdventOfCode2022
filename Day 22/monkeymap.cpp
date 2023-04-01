#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

typedef enum direction {
  right = 0,
  down,
  left,
  up
} direction;

size_t goUp(size_t cur, size_t width, size_t fullSize) {
  if (cur < width) {
    return cur + fullSize - width;
  }
  else {
    return cur - width;
  }
}

size_t goRight(size_t cur, size_t width) {
  size_t newCur = cur + 1;
  if ((newCur % width) == 0) {
    return newCur - width;
  }
  else {
    return newCur;
  }
}

size_t goDown(size_t cur, size_t width, size_t fullSize) {
  size_t newCur = cur + width;
  if (newCur > fullSize) {
    return newCur - fullSize;
  }
  else {
    return newCur;
  }
}

size_t goLeft(size_t cur, size_t width) {
  if ((cur % width) == 0) {
    return cur + width - 1;
  }
  else {
    return cur - 1;
  }
}

void displayMap(char* map, size_t width, size_t height) {
  size_t displayCur = 0;
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      if (map[displayCur] == '\0' || map[displayCur] == '\n')
        std::cout << ' ';
      else
        std::cout << map[displayCur];
      displayCur++;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: monkeymap input_file" << std::endl;
    return EXIT_FAILURE;
  }
  std::ifstream ifs(argv[1]);
  if (!ifs.is_open()) {
    std::cout << "Failed to open" << argv[1] << std::endl;
    return EXIT_FAILURE;
  }
  // first pass, determine size of field
  std::string str;
  size_t width = 0;
  size_t height = 0;
  while (std::getline(ifs, str)) {
    if (str.length() == 1 || isdigit(str[0])) {
      break;
    }
    if (str.length() > width) {
      width = str.length() - 1;
    }
    height++;
  }
  // instantiate array of chars, reset file position
  size_t mapSize = width*height;
  char* monkeyMap = new char[mapSize];
  memset(monkeyMap, ' ', sizeof(char)*mapSize);
  char* mapCur = monkeyMap;
  ifs.seekg(0);
  // load map
  for (int i = 0; i < height; i++) {
    mapCur = monkeyMap + width*i;
    std::getline(ifs, str);
    char * cur = (char*)str.c_str();
    while (*cur != '\0' && *cur != '\r' && *cur != '\n') {
      *mapCur = *cur;
      mapCur++;
      cur++;
    }
  }
  std::getline(ifs, str);
  std::getline(ifs, str);
  // now str has the instructions
  // find starting spot
  size_t monkeyCur = 0;
  while (monkeyMap[monkeyCur] == ' ') {
    monkeyCur++;
  }
  direction dir = right;
  char* instructionCur = (char*)str.c_str();
  char* pathMap = new char[mapSize];
  memcpy(pathMap, monkeyMap, mapSize*sizeof(char));
  pathMap[monkeyCur] = '>';
  while (*instructionCur != '\0' && *instructionCur != '\n' && *instructionCur != '\r') {
    if (isdigit(*instructionCur)) {
      long dist = strtol(instructionCur, &instructionCur, 10);
      for (int i = 0; i < dist; i++) {
        size_t candidateCur;
        switch (dir) {
        case up:
          candidateCur = goUp(monkeyCur, width, mapSize);
          while (monkeyMap[candidateCur] == ' ') {
            candidateCur = goUp(candidateCur, width, mapSize);
          }
          break;
        case right:
          candidateCur = goRight(monkeyCur, width);
          while (monkeyMap[candidateCur] == ' ') {
            candidateCur = goRight(candidateCur, width);
          }
          break;
        case down:
          candidateCur = goDown(monkeyCur, width, mapSize);
          while (monkeyMap[candidateCur] == ' ') {
            candidateCur = goDown(candidateCur, width, mapSize);
          }
          break;
        case left:
          candidateCur = goLeft(monkeyCur, width);
          while (monkeyMap[candidateCur] == ' ') {
            candidateCur = goLeft(candidateCur, width);
          }
          break;
        }
        if (monkeyMap[candidateCur] == '#') {
          break;
        }
        monkeyCur = candidateCur;
        switch (dir) {
        case up:
          pathMap[monkeyCur] = '^';
          break;
        case right:
          pathMap[monkeyCur] = '>';
          break;
        case down:
          pathMap[monkeyCur] = 'v';
          break;
        case left:
          pathMap[monkeyCur] = '<';
          break;
        }
        //displayMap(pathMap, width, height);
      }
    }
    else if (*instructionCur == 'R') {
      instructionCur++;
      dir = (direction)((dir + 1) & 3);
      switch (dir) {
      case up:
        pathMap[monkeyCur] = '^';
        break;
      case right:
        pathMap[monkeyCur] = '>';
        break;
      case down:
        pathMap[monkeyCur] = 'v';
        break;
      case left:
        pathMap[monkeyCur] = '<';
        break;
      }
      //displayMap(pathMap, width, height);
    }
    else if (*instructionCur == 'L') {
      instructionCur++;
      dir = (direction)((dir - 1) & 3);
      switch (dir) {
      case up:
        pathMap[monkeyCur] = '^';
        break;
      case right:
        pathMap[monkeyCur] = '>';
        break;
      case down:
        pathMap[monkeyCur] = 'v';
        break;
      case left:
        pathMap[monkeyCur] = '<';
        break;
      }
      //displayMap(pathMap, width, height);
    }
    else {
      std::cout << "Unknown instruction " << *instructionCur << std::endl;
      instructionCur++;
    }
  }
  displayMap(pathMap, width, height);
  size_t row = (monkeyCur / width) + 1;
  size_t column = (monkeyCur % width) + 1;
  std::cout << "Row " << row << " Column " << column << " Dir " << dir << " Password " << 1000*row + 4*column + dir << std::endl;
  return EXIT_SUCCESS;
}