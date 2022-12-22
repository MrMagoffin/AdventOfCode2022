#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <queue>

typedef enum {
  dirUp,
  dirDown,
  dirLeft,
  dirRight,
  dirNone
} dir_t;

typedef struct {
  int curX;
  int curY;
  int cost;
  int estCost;
  dir_t lastDir;
} path_t;

bool operator<(const path_t& a, const path_t& b) {
  return a.estCost > b.estCost;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: rope input_file" << std::endl;
    exit(1);
  }
  FILE * fp = fopen(argv[1], "r");
  if (fp == NULL) {
    std::cout << "Failed to open " << argv[1] << std::endl;
    exit(1);
  }
  
  int mapWidth = 0;
  int mapHeight = 0;
  int startX = -1;
  int startY = -1;
  int destX = -1;
  int destY = -1;
  // determine width
  while (!feof(fp)) {
    char c;
    c = fgetc(fp);
    if (c == '\n') {
      mapHeight++;
      break;
    }
    else if (c >= 'A' && c <= 'z') {
      mapWidth++;
    }
  }
  // determine height
  bool lastWasNewline = true;
  while (!feof(fp)) {
    char c;
    c = fgetc(fp);
    if (c == '\n') {
      mapHeight++;
      lastWasNewline = true;
    }
    else if (c >= 'A' && c <= 'z') {
      lastWasNewline = false;
    }
  }
  if (!lastWasNewline) {
    mapHeight++;
  }
  // return to start of file
  fseek(fp, 0, SEEK_SET);
  
  // create dynamic 2D array
  char** map = new char*[mapWidth];
  int** bestAtPos = new int*[mapWidth];
  for (int i = 0; i < mapWidth; i++) {
    map[i] = new char[mapHeight];
    bestAtPos[i] = new int[mapHeight];
    for (int j = 0; j < mapHeight; j++) {
      bestAtPos[i][j] = 10000000;
    }
  }
  int curX = 0;
  int curY = 0;
  while (!feof(fp)) {
    char c;
    c = fgetc(fp);
    if (c == '\n') {
      curX = 0;
      curY++;
    }
    else if (c >= 'A' && c <= 'z') {
      if (c == 'S') {
        map[curX][curY] = 'a';
        startX = curX;
        startY = curY;
      }
      else if (c == 'E') {
        map[curX][curY] = 'z';
        destX = curX;
        destY = curY;
      }
      else {
        map[curX][curY] = c;
      }
      curX++;
    }
  }
  
  std::cout << "Loaded " << mapWidth << " x " << mapHeight << " map." << std::endl;
  
  std::priority_queue<path_t> paths;
  path_t curPath;
  curPath.curX = startX;
  curPath.curY = startY;
  curPath.cost = 0;
  curPath.estCost = 0;
  curPath.lastDir = dirNone;
  bestAtPos[startX][startY] = 0;
  paths.push(curPath);
  while (1) {
    path_t newPath;
    curPath = paths.top();
    paths.pop();
    if (curPath.curX == destX && curPath.curY == destY) {
      break;
    }
    char curC = map[curPath.curX][curPath.curY];
    // check four directions to see if it's possible
    if (curPath.lastDir != dirRight && curPath.curX > 0 && map[curPath.curX - 1][curPath.curY] - curC < 2 && bestAtPos[curPath.curX - 1][curPath.curY] > curPath.cost + 1) {
      newPath.curX = curPath.curX - 1;
      newPath.curY = curPath.curY;
      newPath.cost = curPath.cost + 1;
      newPath.estCost = curPath.cost + 1 + std::max('z' - map[newPath.curX][newPath.curY], abs(newPath.curX - destX) + abs(newPath.curY - destY));
      newPath.lastDir = dirLeft;
      bestAtPos[newPath.curX][newPath.curY] = newPath.cost;
      paths.push(newPath);
    }
    if (curPath.lastDir != dirLeft && curPath.curX < mapWidth - 1 && map[curPath.curX + 1][curPath.curY] - curC < 2 && bestAtPos[curPath.curX + 1][curPath.curY] > curPath.cost + 1) {
      newPath.curX = curPath.curX + 1;
      newPath.curY = curPath.curY;
      newPath.cost = curPath.cost + 1;
      newPath.estCost = curPath.cost + 1 + std::max('z' - map[newPath.curX][newPath.curY], abs(newPath.curX - destX) + abs(newPath.curY - destY));
      newPath.lastDir = dirRight;
      bestAtPos[newPath.curX][newPath.curY] = newPath.cost;
      paths.push(newPath);
    }
    if (curPath.lastDir != dirDown && curPath.curY > 0 && map[curPath.curX][curPath.curY - 1] - curC < 2 && bestAtPos[curPath.curX][curPath.curY - 1] > curPath.cost + 1) {
      newPath.curX = curPath.curX;
      newPath.curY = curPath.curY - 1;
      newPath.cost = curPath.cost + 1;
      newPath.estCost = curPath.cost + 1 + std::max('z' - map[newPath.curX][newPath.curY], abs(newPath.curX - destX) + abs(newPath.curY - destY));
      newPath.lastDir = dirUp;
      bestAtPos[newPath.curX][newPath.curY] = newPath.cost;
      paths.push(newPath);
    }
    if (curPath.lastDir != dirUp && curPath.curY < mapHeight - 1 && map[curPath.curX][curPath.curY + 1] - curC < 2 && bestAtPos[curPath.curX][curPath.curY + 1] > curPath.cost + 1) {
      newPath.curX = curPath.curX;
      newPath.curY = curPath.curY + 1;
      newPath.cost = curPath.cost + 1;
      newPath.estCost = curPath.cost + 1 + std::max('z' - map[newPath.curX][newPath.curY], abs(newPath.curX - destX) + abs(newPath.curY - destY));
      newPath.lastDir = dirDown;
      bestAtPos[newPath.curX][newPath.curY] = newPath.cost;
      paths.push(newPath);
    }
  }
  
  std::cout << "Found path to destination with " << curPath.cost << " steps." << std::endl;
  
  // Part 2
  while (!paths.empty()) {
    paths.pop();
  }
  for (int i = 0; i < mapWidth; i++) {
    for (int j = 0; j < mapHeight; j++) {
      if (map[i][j] == 'a') {
        curPath.curX = i;
        curPath.curY = j;
        curPath.cost = 0;
        curPath.estCost = 0;
        curPath.lastDir = dirNone;
        bestAtPos[curPath.curX][curPath.curY] = 0;
        paths.push(curPath);
      }
    }
  }
  while (1) {
    path_t newPath;
    curPath = paths.top();
    paths.pop();
    if (curPath.curX == destX && curPath.curY == destY) {
      break;
    }
    char curC = map[curPath.curX][curPath.curY];
    // check four directions to see if it's possible
    if (curPath.lastDir != dirRight && curPath.curX > 0 && map[curPath.curX - 1][curPath.curY] - curC < 2 && bestAtPos[curPath.curX - 1][curPath.curY] > curPath.cost + 1) {
      newPath.curX = curPath.curX - 1;
      newPath.curY = curPath.curY;
      newPath.cost = curPath.cost + 1;
      newPath.estCost = curPath.cost + 1 + std::max('z' - map[newPath.curX][newPath.curY], abs(newPath.curX - destX) + abs(newPath.curY - destY));
      newPath.lastDir = dirLeft;
      bestAtPos[newPath.curX][newPath.curY] = newPath.cost;
      paths.push(newPath);
    }
    if (curPath.lastDir != dirLeft && curPath.curX < mapWidth - 1 && map[curPath.curX + 1][curPath.curY] - curC < 2 && bestAtPos[curPath.curX + 1][curPath.curY] > curPath.cost + 1) {
      newPath.curX = curPath.curX + 1;
      newPath.curY = curPath.curY;
      newPath.cost = curPath.cost + 1;
      newPath.estCost = curPath.cost + 1 + std::max('z' - map[newPath.curX][newPath.curY], abs(newPath.curX - destX) + abs(newPath.curY - destY));
      newPath.lastDir = dirRight;
      bestAtPos[newPath.curX][newPath.curY] = newPath.cost;
      paths.push(newPath);
    }
    if (curPath.lastDir != dirDown && curPath.curY > 0 && map[curPath.curX][curPath.curY - 1] - curC < 2 && bestAtPos[curPath.curX][curPath.curY - 1] > curPath.cost + 1) {
      newPath.curX = curPath.curX;
      newPath.curY = curPath.curY - 1;
      newPath.cost = curPath.cost + 1;
      newPath.estCost = curPath.cost + 1 + std::max('z' - map[newPath.curX][newPath.curY], abs(newPath.curX - destX) + abs(newPath.curY - destY));
      newPath.lastDir = dirUp;
      bestAtPos[newPath.curX][newPath.curY] = newPath.cost;
      paths.push(newPath);
    }
    if (curPath.lastDir != dirUp && curPath.curY < mapHeight - 1 && map[curPath.curX][curPath.curY + 1] - curC < 2 && bestAtPos[curPath.curX][curPath.curY + 1] > curPath.cost + 1) {
      newPath.curX = curPath.curX;
      newPath.curY = curPath.curY + 1;
      newPath.cost = curPath.cost + 1;
      newPath.estCost = curPath.cost + 1 + std::max('z' - map[newPath.curX][newPath.curY], abs(newPath.curX - destX) + abs(newPath.curY - destY));
      newPath.lastDir = dirDown;
      bestAtPos[newPath.curX][newPath.curY] = newPath.cost;
      paths.push(newPath);
    }
  }
  
  std::cout << "Found trail to destination with " << curPath.cost << " steps." << std::endl;

  fclose(fp);

  return 0;
}