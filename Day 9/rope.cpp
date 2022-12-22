#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

bool debug = false;

int displayField(std::ostream &out, bool* playingField, int width, int height, int* knotX, int* knotY, int knotCount) {
  int visited = 0;
  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (x == knotX[0] && y == knotY[0]) {
        out << "H";
      }
      else {
        bool foundKnot = false;
        for (int i = 1; i < knotCount; i++) {
          if (x == knotX[i] && y == knotY[i]) {
            out << i;
            foundKnot = true;
            break;
          }
        }
        if (!foundKnot) {
          if (playingField[height*x + y]) {
            out << "#";
          }
          else {
            out << ".";
          }
        }
      }
      
      if (playingField[height*x + y]) {
        visited++;
      }
    }
    out << std::endl;
  }
  out << "Visited " << visited << " locations." << std::endl;
  
  return visited;
}

void updateTail(int headX, int headY, int &tailX, int &tailY) {
  if (headX - tailX > 1 || tailX - headX > 1 || headY - tailY > 1 || tailY - headY > 1) {
    // tail needs to move
    if (headX > tailX) {
      tailX++;
    }
    else if (tailX > headX) {
      tailX--;
    }
    if (headY > tailY) {
      tailY++;
    }
    else if (headY < tailY) {
      tailY--;
    }   
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: rope input_file knot_count [debug]" << std::endl;
    exit(1);
  }
  int knotCount = atoi(argv[2]);
  if (knotCount < 1) {
    std::cout << "knot_count must be at least 1" << std::endl;
    exit(1);
  }
  std::fstream fs(argv[1], std::fstream::in);
  if (fs.fail()) {
    std::cout << "Failed to open " << argv[1] << std::endl;
    exit(1);
  }
  if (argc > 3) {
    debug = true;
  }
  
  int currentWidth = 1;
  int currentHeight = 1;
  int* knotX = new int[knotCount];
  int* knotY = new int[knotCount];
  for (int i = 0; i < knotCount; i++) {
    knotX[i] = 0;
    knotY[i] = 0;
  }
  bool* playingField = new bool[1];
  // visit starting position
  playingField[0] = true;
  while (!fs.eof()) {
    std::string dir;
    int count;
    fs >> dir;
    if (fs.eof()) {
      break;
    }
    fs >> count;
    if (dir == "U") {
      // ensure playing field is large enough
      int expandAmount = count - knotY[0];
      if (expandAmount > 0) {
        // expand playing field
        int newHeight = currentHeight + expandAmount;
        bool* newField = new bool[currentWidth*newHeight];
        for (int x = 0; x < currentWidth; x++) {
          for (int y = 0; y < expandAmount; y++) {
            // initialize new area
            newField[newHeight*x + y] = false;
          }
          for (int y = 0; y < currentHeight; y++) {
            // copy old area
            newField[newHeight*x + y + expandAmount] = playingField[currentHeight*x + y];
          }
        }
        delete[] playingField;
        playingField = newField;
        currentHeight = newHeight;
        // update knot coordinates to match new playing field
        for (int i = 0; i < knotCount; i++) {
          knotY[i] += expandAmount;
        }
      }
      for (int i = 0; i < count; i++) {
        // move head
        knotY[0]--;
        // update knots
        for (int j = 0; j < knotCount - 1; j++) {
          updateTail(knotX[j], knotY[j], knotX[j+1], knotY[j+1]);
        }
        // visit tail location
        playingField[currentHeight*knotX[knotCount-1] + knotY[knotCount-1]] = true;
        if (debug) {
          std::cout << "\x1B[2J\x1B[H";
          displayField(std::cout, playingField, currentWidth, currentHeight, knotX, knotY, knotCount);
          getchar();
        }
      }
    }
    else if (dir == "D") {
      // ensure playing field is large enough
      int expandAmount = count + knotY[0] - (currentHeight - 1);
      if (expandAmount > 0) {
        // expand playing field
        int newHeight = currentHeight + expandAmount;
        bool* newField = new bool[currentWidth*newHeight];
        for (int x = 0; x < currentWidth; x++) {
          for (int y = 0; y < currentHeight; y++) {
            // copy old area
            newField[newHeight*x + y] = playingField[currentHeight*x + y];
          }
          for (int y = currentHeight; y < newHeight; y++) {
            // initialize new area
            newField[newHeight*x + y] = false;
          }
        }
        delete[] playingField;
        playingField = newField;
        currentHeight = newHeight;
        // no need to update knot coordinates to match new playing field
      }
      for (int i = 0; i < count; i++) {
        // move head
        knotY[0]++;
        // update knots
        for (int j = 0; j < knotCount - 1; j++) {
          updateTail(knotX[j], knotY[j], knotX[j+1], knotY[j+1]);
        }
        // visit tail location
        playingField[currentHeight*knotX[knotCount-1] + knotY[knotCount-1]] = true;
        if (debug) {
          std::cout << "\x1B[2J\x1B[H";
          displayField(std::cout, playingField, currentWidth, currentHeight, knotX, knotY, knotCount);
          getchar();
        }
      }
    }
    else if (dir == "L") {
      // ensure playing field is large enough
      int expandAmount = count - knotX[0];
      if (expandAmount > 0) {
        // expand playing field
        int newWidth = currentWidth + expandAmount;
        bool* newField = new bool[newWidth*currentHeight];
        for (int x = 0; x < expandAmount; x++) {
          for (int y = 0; y < currentHeight; y++) {
            // initialize new area
            newField[currentHeight*x + y] = false;
          }
        }
        for (int x = 0; x < currentWidth; x++) {
          for (int y = 0; y < currentHeight; y++) {
            // copy old area
            newField[currentHeight*(x + expandAmount) + y] = playingField[currentHeight*x + y];
          }
        }
        delete[] playingField;
        playingField = newField;
        currentWidth = newWidth;
        // update knot coordinates to match new playing field
        for (int i = 0; i < knotCount; i++) {
          knotX[i] += expandAmount;
        }
      }
      for (int i = 0; i < count; i++) {
        // move head
        knotX[0]--;
        // update knots
        for (int j = 0; j < knotCount - 1; j++) {
          updateTail(knotX[j], knotY[j], knotX[j+1], knotY[j+1]);
        }
        // visit tail location
        playingField[currentHeight*knotX[knotCount-1] + knotY[knotCount-1]] = true;
        if (debug) {
          std::cout << "\x1B[2J\x1B[H";
          displayField(std::cout, playingField, currentWidth, currentHeight, knotX, knotY, knotCount);
          getchar();
        }
      }
    }
    else if (dir == "R") {
      // ensure playing field is large enough
      int expandAmount = count + knotX[0] - (currentWidth - 1);
      if (expandAmount > 0) {
        // expand playing field
        int newWidth = currentWidth + expandAmount;
        bool* newField = new bool[newWidth*currentHeight];
        for (int x = 0; x < currentWidth; x++) {
          for (int y = 0; y < currentHeight; y++) {
            // copy old area
            newField[currentHeight*x + y] = playingField[currentHeight*x + y];
          }
        }
        for (int x = currentWidth; x < newWidth; x++) {
          for (int y = 0; y < currentHeight; y++) {
            // initialize new area
            newField[currentHeight*x + y] = false;
          }
        }
        delete[] playingField;
        playingField = newField;
        currentWidth = newWidth;
        // no need to update knot coordinates to match new playing field
      }
      for (int i = 0; i < count; i++) {
        // move head
        knotX[0]++;
        // update knots
        for (int j = 0; j < knotCount - 1; j++) {
          updateTail(knotX[j], knotY[j], knotX[j+1], knotY[j+1]);
        }
        // visit tail location
        playingField[currentHeight*knotX[knotCount-1] + knotY[knotCount-1]] = true;
        if (debug) {
          std::cout << "\x1B[2J\x1B[H";
          displayField(std::cout, playingField, currentWidth, currentHeight, knotX, knotY, knotCount);
          getchar();
        }
      }
    }
    else {
      std::cout << "Unrecognized direction " << dir << std::endl;
      exit(1);
    }
  }
  
  fs.close();
  
  std::fstream ofs("rope_out.txt", std::fstream::out);
  
  displayField(ofs, playingField, currentWidth, currentHeight, knotX, knotY, knotCount);

  return 0;
}