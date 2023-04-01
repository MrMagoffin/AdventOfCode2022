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

size_t gcd(size_t a, size_t b) {
   while (b != 0) {
     size_t temp = b;
     b = a % b;
     a = temp;
   }
   return a;
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
  size_t cubeSize = gcd(width, height);
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
  std::cout << "Cube size: " << cubeSize << std::endl;
  while (*instructionCur != '\0' && *instructionCur != '\n' && *instructionCur != '\r') {
    if (isdigit(*instructionCur)) {
      long dist = strtol(instructionCur, &instructionCur, 10);
      for (int i = 0; i < dist; i++) {
        size_t candidateCur;
        direction candidateDir = dir;
        switch (dir) {
        case up:
          candidateCur = goUp(monkeyCur, width, mapSize);
          break;
        case right:
          candidateCur = goRight(monkeyCur, width);
          break;
        case down:
          candidateCur = goDown(monkeyCur, width, mapSize);
          break;
        case left:
          candidateCur = goLeft(monkeyCur, width);
          break;
        }
        if (monkeyMap[candidateCur] == ' ') {
          // changing faces
          size_t col = monkeyCur % width;
          size_t row = monkeyCur / width;
          size_t candidateCol;
          size_t candidateRow;
          switch (dir) {
          case up:
            if (row == cubeSize*2 && col >= 0 && col < cubeSize) {
              // edge E
              std::cout << "Traveled across edge E ";
              candidateCol = cubeSize;
              candidateRow = cubeSize + col;
              candidateDir = right;
            }
            else if (row == 0 && col >= cubeSize && col < cubeSize*2) {
              // edge G
              std::cout << "Traveled across edge G ";
              candidateCol = 0;
              candidateRow = col + 2*cubeSize;
              candidateDir = right;
            }
            else if (row == 0 && col >= cubeSize*2 && col < cubeSize*3) {
              // edge A
              std::cout << "Traveled across edge A ";
              candidateCol = col - 2*cubeSize;
              candidateRow = cubeSize*4 - 1;
            }
            else {
              std::cout << "Unknown edge row " << row << " column " << col << " dir " << dir << std::endl;
              return EXIT_FAILURE;
            }
            break;
          case right:
            if (col == cubeSize*3 - 1 && row >= 0 && row < cubeSize) {
              // edge B
              std::cout << "Traveled across edge B ";
              candidateCol = cubeSize*2 - 1;
              candidateRow = cubeSize*3 - row - 1;
              candidateDir = left;
            }
            else if (col == cubeSize*2 - 1 && row >= cubeSize && row < cubeSize*2) {
              // edge C
              std::cout << "Traveled across edge C ";
              candidateCol = row + cubeSize;
              candidateRow = cubeSize - 1;
              candidateDir = up;
            }
            else if (col == cubeSize*2 - 1 && row >= cubeSize*2 && row < cubeSize*3) {
              // edge B
              std::cout << "Traveled across edge B ";
              candidateCol = cubeSize*3 - 1;
              candidateRow = cubeSize*3 - row - 1;
              candidateDir = left;
            }
            else if (col == cubeSize - 1 && row >= cubeSize*3 && row < cubeSize*4) {
              // edge D
              std::cout << "Traveled across edge D ";
              candidateCol = row - cubeSize*2;
              candidateRow = cubeSize*3 - 1;
              candidateDir = up;
            }
            else {
              std::cout << "Unknown edge row " << row << " column " << col << " dir " << dir << std::endl;
              return EXIT_FAILURE;
            }
            break;
          case down:
            if (row == cubeSize*4 - 1 && col >= 0 && col < cubeSize) {
              // edge A
              std::cout << "Traveled across edge A ";
              candidateCol = col + 2*cubeSize;
              candidateRow = 0;
            }
            else if (row == cubeSize*3 - 1 && col >= cubeSize && col < cubeSize*2) {
              // edge D
              std::cout << "Traveled across edge D ";
              candidateCol = cubeSize - 1;
              candidateRow = col + cubeSize*2;
              candidateDir = left;
            }
            else if (row == cubeSize - 1 && col >= cubeSize*2 && col < cubeSize*3) {
              // edge C
              std::cout << "Traveled across edge C ";
              candidateCol = cubeSize*2 - 1;
              candidateRow = col - cubeSize;
              candidateDir = left;
            }
            else {
              std::cout << "Unknown edge row " << row << " column " << col << " dir " << dir << std::endl;
              return EXIT_FAILURE;
            }
            break;
          case left:
            if (col == cubeSize && row >= 0 && row < cubeSize) {
              // edge F
              std::cout << "Traveled across edge F ";
              candidateCol = 0;
              candidateRow = cubeSize*3 - row - 1;
              candidateDir = right;
            }
            else if (col == cubeSize && row >= cubeSize && row < cubeSize*2) {
              // edge E
              std::cout << "Traveled across edge E ";
              candidateCol = row - cubeSize;
              candidateRow = cubeSize*2;
              candidateDir = down;
            }
            else if (col == 0 && row >= cubeSize*2 && row < cubeSize*3) {
              // edge F
              std::cout << "Traveled across edge F ";
              candidateCol = cubeSize;
              candidateRow = cubeSize*3 - row - 1;
              candidateDir = right;
            }
            else if (col == 0 && row >= cubeSize*3 && row < cubeSize*4) {
              // edge G
              std::cout << "Traveled across edge G ";
              candidateCol = row - 2*cubeSize;
              candidateRow = 0;
              candidateDir = down;
            }
            else {
              std::cout << "Unknown edge row " << row << " column " << col << " dir " << dir << std::endl;
              return EXIT_FAILURE;
            }
            break;
          }
          std::cout << "from row "<< row << " column " << col << " dir " << dir << " to row " << candidateRow << " column " << candidateCol << " dir " << candidateDir << std::endl;
          candidateCur = candidateRow*width + candidateCol;
        }
        if (monkeyMap[candidateCur] == '#') {
          break;
        }
        monkeyCur = candidateCur;
        dir = candidateDir;
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