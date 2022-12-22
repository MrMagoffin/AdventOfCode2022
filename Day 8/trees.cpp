#include <cstdlib>
#include <cstdio>

class Tree {
private:
  bool visible;
  int height;
public:
  Tree(int _height) {
    visible = false;
    height = _height;
  }

  void setVisible() {
    // flag this tree as visible
    visible = true;
  }
  
  bool isVisible() {
    return visible;
  }
  
  int getHeight() {
    return height;
  }
};

int scenicScore(Tree*** treeMap, int width, int height, int x, int y) {
  int score = 1;
  int treeCount;
  int maxHeight = treeMap[x][y]->getHeight();
  int viewHeight;

  // check leftward
  viewHeight = -1;
  treeCount = 0;
  for (int i = x - 1; i >= 0; i--) {
    int curHeight = treeMap[i][y]->getHeight();
    treeCount++;
    if (curHeight >= maxHeight) {
      break;
    }
    else {
      viewHeight = curHeight;
    }
  }
  score *= treeCount;
  
  // check rightward
  viewHeight = -1;
  treeCount = 0;
  for (int i = x + 1; i < width; i++) {
    int curHeight = treeMap[i][y]->getHeight();
    treeCount++;
    if (curHeight >= maxHeight) {
      break;
    }
    else {
      viewHeight = curHeight;
    }
  }
  score *= treeCount;
  
  // check upward
  viewHeight = -1;
  treeCount = 0;
  for (int i = y - 1; i >= 0; i--) {
    int curHeight = treeMap[x][i]->getHeight();
    treeCount++;
    if (curHeight >= maxHeight) {
      break;
    }
    else {
      viewHeight = curHeight;
    }
  }
  score *= treeCount;
  
  // check downward
  viewHeight = -1;
  treeCount = 0;
  for (int i = y + 1; i < height; i++) {
    int curHeight = treeMap[x][i]->getHeight();
    treeCount++;
    if (curHeight >= maxHeight) {
      break;
    }
    else {
      viewHeight = curHeight;
    }
  }
  score *= treeCount;
  
  return score;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Provide input as first argument to application.\n");
    exit(1);
  }
  FILE * fp = fopen(argv[1], "r");
  if (fp == NULL) {
    printf("Failed to open ");
    printf(argv[1]);
    printf("\n");
    exit(1);
  }
  
  int mapWidth = 0;
  int mapHeight = 0;
  // determine width
  while (!feof(fp)) {
    char c;
    c = fgetc(fp);
    if (c == '\n') {
      mapHeight++;
      break;
    }
    else if (c >= '0' && c <= '9') {
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
    else if (c >= '0' && c <= '9') {
      lastWasNewline = false;
    }
  }
  if (!lastWasNewline) {
    mapHeight++;
  }
  // return to start of file
  fseek(fp, 0, SEEK_SET);
  
  // create dynamic 2D array
  Tree*** treeMap = new Tree**[mapWidth];
  for (int i = 0; i < mapWidth; i++) {
    treeMap[i] = new Tree*[mapHeight];
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
    else if (c >= '0' && c <= '9') {
      treeMap[curX][curY] = new Tree(c-48);
      curX++;
    }
  }
  
  printf("Loaded %d trees.\n", mapHeight*mapWidth);
  
  // iterate through each cardinal ray direction
  for (int dir = 0; dir < 4; dir++) {
    // 0 = down, 1 = up, 2 = right, 3 = left
    // iterate through possible rays in this direction
    for (int i = 0; i < ((dir / 2) ? mapWidth : mapHeight); i++) {
      int curHeight = -1;
      // iterate through trees on this ray
      for (int j = 0; j < ((dir / 2) ? mapHeight : mapWidth); j++) {
        Tree* tree;
        switch (dir) {
          case 0:
            tree = treeMap[i][j];
            break;
          case 1:
            tree = treeMap[i][mapHeight - j - 1];
            break;
          case 2:
            tree = treeMap[j][i];
            break;
          case 3:
            tree = treeMap[mapWidth - j - 1][i];
            break;
          default:
            break;
        }
        if (tree->getHeight() > curHeight) {
          tree->setVisible();
          curHeight = tree->getHeight();
        }
      }
    }
  }
  
  // sum visible trees
  int visibleTrees = 0;
  for (int i = 0; i < mapWidth; i++) {
    for (int j = 0; j < mapHeight; j++) {
      if (treeMap[i][j]->isVisible()) {
        visibleTrees++;
      }
    }
  }
  
  printf("%d trees visible from the outside.\n", visibleTrees);
  
  // search for optimal tree
  // iterate through trees
  int scenicMax = -1;
  int maxX = -1;
  int maxY = -1;
  for (int i = 0; i < mapWidth; i++) {
    for (int j = 0; j < mapHeight; j++) {
      int score = scenicScore(treeMap, mapWidth, mapHeight, i, j);
      if (score > scenicMax) {
        scenicMax = score;
        maxX = i;
        maxY = j;
      }
    }
  }
  
  printf("Optimal tree is at (%d,%d) with score of %d.\n", maxX, maxY, scenicMax);

  return 0;
}