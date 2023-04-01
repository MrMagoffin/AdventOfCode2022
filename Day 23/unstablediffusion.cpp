#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

typedef enum direction {
  North = 0,
  South,
  West,
  East
} direction;

struct Elf {
  int _x;
  int _y;
  int _lastX;
  int _lastY;
  bool _willRevert;
  bool _hasMoved;
};

class PlayingField {
public:
  PlayingField(int width, int height) : _width(width), _height(height), _xOffset(0), _yOffset(0) {
    _elfField = new Elf*[width*height];
    for (int i = 0; i < width*height; i++) {
      _elfField[i] = nullptr;
    }
  }
  ~PlayingField() {
    delete[] _elfField;
  }
  void testAndExpand(int xCoord, int yCoord) {
    // ensure there's a one-thick border of emptiness around this set of coordinates
    if (xCoord <= -_xOffset) {
      // expand left
      int expandAmount = -_xOffset-xCoord+1;
      int newWidth = _width+expandAmount;
      Elf** newField =  new Elf*[newWidth*_height];
      for (int x = 0; x < expandAmount; x++) {
        for (int y = 0; y < _height; y++) {
          // initialize new area
          newField[x+newWidth*y] = nullptr;
        }
      }
      for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {
          // copy old area
          newField[x+expandAmount+newWidth*y] = _elfField[x+_width*y];
        }
      }
      delete[] _elfField;
      _elfField = newField;
      _width = newWidth;
      _xOffset = -xCoord + 1;
    }
    else if (xCoord >= _width-_xOffset-1) {
      // expand right
      int expandAmount = xCoord+_xOffset-_width+2;
      int newWidth = _width+expandAmount;
      Elf** newField = new Elf*[newWidth*_height];
      for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {
          // copy old area
          newField[x+newWidth*y] = _elfField[x+_width*y];
        }
      }
      for (int x = _width; x < newWidth; x++) {
        for (int y = 0; y < _height; y++) {
          // initialize new area
          newField[x+newWidth*y] = nullptr;
        }
      }
      delete[] _elfField;
      _elfField = newField;
      _width = newWidth;
    }
    if (yCoord <= -_yOffset) {
      // expand up
      int expandAmount = -_yOffset-yCoord+1;
      int newHeight = _height+expandAmount;
      Elf** newField =  new Elf*[_width*newHeight];
      for (int x = 0; x < _width; x++) {
        for (int y = 0; y < expandAmount; y++) {
          // initialize new area
          newField[x+_width*y] = nullptr;
        }
        for (int y = 0; y < _height; y++) {
          // copy old area
          newField[x+_width*(y+expandAmount)] = _elfField[x+_width*y];
        }
      }
      delete[] _elfField;
      _elfField = newField;
      _height = newHeight;
      _yOffset = -yCoord + 1;
    }
    else if (yCoord >= _height-_yOffset-1) {
      // expand down
      int expandAmount = yCoord+_yOffset-_height+2;
      int newHeight = _height+expandAmount;
      Elf** newField =  new Elf*[_width*newHeight];
      for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {
          // copy old area
          newField[x + y*_width] = _elfField[x + _width*y];
        }
        for (int y = _height; y < newHeight; y++) {
          // initialize new area
          newField[x + y*_width] = nullptr;
        }
      }
      delete[] _elfField;
      _elfField = newField;
      _height = newHeight;
    }
  }
  Elf** fieldAt(int x, int y) {
    return &_elfField[x+_xOffset+_width*(y+_yOffset)];
  }
  void displayField() {
    for (int y = 0; y < _height; y++) {
      for (int x = 0; x < _width; x++) {
        if (_elfField[x+_width*y] == nullptr) {
          std::cout << '.';
        }
        else {
          std::cout << '#';
        }
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  void addElf(int x, int y) {
    auto it = _elfVector.emplace(_elfVector.end());
    it->_x = x;
    it->_y = y;
    it->_lastX = x;
    it->_lastY = y;
    it->_willRevert = false;
    it->_hasMoved = false;
    // does the field need to expand?
    testAndExpand(x, y);
    *fieldAt(x,y) = &(*it);
  }
  // returns whether an elf moved
  bool doRound(int round) {
    bool elfMoved = false;
    // choose starting direction dependent on round
    direction dir = (direction)(round % 4);
    // iterate over elves to make a decision
    for (auto it = _elfVector.begin(); it != _elfVector.end(); it++) {
      if (*fieldAt(it->_x-1, it->_y-1) == nullptr && 
          *fieldAt(it->_x, it->_y-1) == nullptr && 
          *fieldAt(it->_x+1, it->_y-1) == nullptr &&
          *fieldAt(it->_x+1, it->_y) == nullptr &&
          *fieldAt(it->_x+1, it->_y+1) == nullptr &&
          *fieldAt(it->_x, it->_y+1) == nullptr &&
          *fieldAt(it->_x-1, it->_y+1) == nullptr &&
          *fieldAt(it->_x-1, it->_y) == nullptr) {
        // no elf in the vicinity, no need to move
        continue;
      }
      // iterate over each direction choice
      for (int i = 0; i < 4; i++) {
        if (!it->_hasMoved) {
          switch (dir) {
          case North:
            if (*fieldAt(it->_x-1, it->_y-1) == nullptr && 
                *fieldAt(it->_x, it->_y-1) == nullptr && 
                *fieldAt(it->_x+1, it->_y-1) == nullptr) {
              it->_y--;
              it->_hasMoved = true;
            }
            break;
          case South:
            if (*fieldAt(it->_x-1, it->_y+1) == nullptr && 
                *fieldAt(it->_x, it->_y+1) == nullptr && 
                *fieldAt(it->_x+1, it->_y+1) == nullptr) {
              it->_y++;
              it->_hasMoved = true;
            }
            break;
          case West:
            if (*fieldAt(it->_x-1, it->_y-1) == nullptr && 
                *fieldAt(it->_x-1, it->_y) == nullptr && 
                *fieldAt(it->_x-1, it->_y+1) == nullptr) {
              it->_x--;
              it->_hasMoved = true;
            }
            break;
          case East:
            if (*fieldAt(it->_x+1, it->_y-1) == nullptr && 
                *fieldAt(it->_x+1, it->_y) == nullptr && 
                *fieldAt(it->_x+1, it->_y+1) == nullptr) {
              it->_x++;
              it->_hasMoved = true;
            }
            break;
          }
        }
        dir = (direction)((dir + 1) % 4);
      }
    }
    // now all elves that could make a choice have made one
    // iterate through again and execute them, reverting if need be
    for (auto it = _elfVector.begin(); it != _elfVector.end(); it++) {
      if (it->_hasMoved) {
        if (*fieldAt(it->_x, it->_y) == nullptr) {
          *fieldAt(it->_x, it->_y) = &(*it);
        }
        else {
          (*fieldAt(it->_x, it->_y))->_willRevert = true;
          it->_willRevert = true;
        }
      }
    }
    // now one final time go through and finalize the positions, plus reset the Elf structs
    for (auto it = _elfVector.begin(); it != _elfVector.end(); it++) {
      if (it->_hasMoved) {
        if (it->_willRevert) {
          // move failed
          *fieldAt(it->_x, it->_y) = nullptr;
          it->_x = it->_lastX;
          it->_y = it->_lastY;
          it->_willRevert = false;
        }
        else {
          // commit move
          *fieldAt(it->_lastX, it->_lastY) = nullptr;
          it->_lastX = it->_x;
          it->_lastY = it->_y;
          it->_willRevert = false;
          // potentially expand field
          testAndExpand(it->_x, it->_y);
          elfMoved = true;
        }
        it->_hasMoved = false;
      }
    }
    return elfMoved;
  }
  int getWidth() {
    return _width;
  }
  int getHeight() {
    return _height;
  }
  int getElfCount() {
    return _elfVector.size();
  }
private:
  Elf** _elfField; // dynamic array of pointers to Elf structs, referenced by x+width*y
  std::vector<Elf> _elfVector; // vector of Elves
  // differences between coordinate and pointer offset
  int _xOffset; 
  int _yOffset;
  int _width; // current width
  int _height; // current height
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: unstablediffusion input_file" << std::endl;
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
  PlayingField field(width, height);
  ifs.clear();
  ifs.seekg(0);
  // load map
  for (int y = 0; y < height; y++) {
    std::getline(ifs, str);
    for (int x = 0; x < width; x++) {
      if (str[x] == '#') {
        field.addElf(x, y);
      }
    }
  }
  field.displayField();
  // execute rounds
  for (int i = 0; i < 10; i++) {
    field.doRound(i);
    field.displayField();
  }
  std::cout << "Size of rectangle: " << field.getWidth()-2 << " by " << field.getHeight()-2 << std::endl;
  std::cout << "Elves: " << field.getElfCount() << std::endl;
  std::cout << "Empty ground: " << (field.getWidth()-2)*(field.getHeight()-2) - field.getElfCount() << std::endl;
  // continue executing
  bool elfMoved = true;
  int i = 10;
  while (elfMoved) {
    elfMoved = field.doRound(i);
    field.displayField();
    i++;
  }
  std::cout << "No elf moved on round " << i << std::endl;
}