#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <string>
#include <climits>

struct Blizzard {
  int x;
  int y;
  char dir;
};

class BlizzardStates {
public:
  BlizzardStates(int width, int height) : _width(width), _height(height) {
    _size = width*height;
    states.emplace(states.end()); // ensure states[0] exists
  }
  ~BlizzardStates() {
    for (auto it = stateMaps.begin(); it != stateMaps.end(); it++) {
      delete[] *it;
    }
  }
  void addBlizzard(int x, int y, char dir) {
    auto it = states[0].emplace(states[0].end());
    it->x = x;
    it->y = y;
    it->dir = dir;
  }
  void displayState(int index) {
    char* display = new char[(_width+2)*(_height+2)];
    for (int i = 0; i < _width+2; i++) {
      display[i] = (i == 1) ? '.' : '#';
      display[i+(_height+1)*(_width+2)] = (i == _width) ? '.' : '#';
    }
    for (int i = 0; i < _height; i++) {
      display[(i+1)*(_width+2)] = '#';
      display[_width + 1 + (i+1)*(_width+2)] = '#';
    }
    for (int x = 0; x < _width; x++) {
      for (int y = 0; y < _height; y++) {
        display[x + 1 + (_width+2)*(y+1)] = '.';
      }
    }
    std::vector<Blizzard>* state = getState(index);
    for (auto it = state->begin(); it != state->end(); it++) {
      char* charRef = display+(it->x + 1 + (_width+2)*(it->y + 1));
      if (*charRef != '.') {
        if (isdigit(*charRef)) {
          *charRef += 1;
        }
        else {
          *charRef = '2';
        }
      }
      else {
        *charRef = it->dir;
      }
    }
    for (int y = 0; y < _height + 2; y++) {
      for (int x = 0; x < _width + 2; x++) {
        std::cout << display[x + (_width+2)*y];
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  bool checkStateAtPos(int index, int x, int y) {
    if (x < 0 || x >= _width || y < 0 || y >= _height) {
      return true;
    }
    bool* map = getMap(index);
    bool result = map[x + _width * y];
    return result;
  }
  std::vector<Blizzard>* getState(int index) {
    while (states.size() <= index) {
      // generate states
      std::vector<Blizzard>* lastState = &*(states.end()-1);
      states.emplace(states.end());
      for (int i = 0; i < (states.end()-2)->size(); i++) {
        auto blizzard = states.back().emplace(states.back().end());
        Blizzard* last = &(*(states.end()-2))[i];
        blizzard->dir = last->dir;
        switch (blizzard->dir) {
        case '^':
          blizzard->x = last->x;
          blizzard->y = last->y - 1;
          if (blizzard->y < 0) {
            blizzard->y = _height - 1;
          }
          break;
        case 'v':
          blizzard->x = last->x;
          blizzard->y = last->y + 1;
          if (blizzard->y >= _height) {
            blizzard->y = 0;
          }
          break;
        case '<':
          blizzard->x = last->x - 1;
          blizzard->y = last->y;
          if (blizzard->x < 0) {
            blizzard->x = _width - 1;
          }
          break;
        case '>':
          blizzard->x = last->x + 1;
          blizzard->y = last->y;
          if (blizzard->x >= _width) {
            blizzard->x = 0;
          }
          break;
        }
      }
    }
    return &states[index];
  }
  bool* getMap(int index) {
    while (stateMaps.size() <= index) {
      // generate maps
      bool* map = new bool[_size];
      for (int i = 0; i < _size; i++) {
        map[i] = false;
      }
      std::vector<Blizzard>* state = getState(stateMaps.size());
      for (auto it = state->begin(); it != state->end(); it++) {
        map[it->x + _width*it->y] = true;
      }
      stateMaps.push_back(map);
    }
    return stateMaps[index];
  }
  
private:
  int _width;
  int _height;
  int _size;
  std::vector<bool*> stateMaps;
  std::vector<std::vector<Blizzard>> states;
};

typedef struct {
  int x;
  int y;
  int timeElapsed;
  int estCost;
} state_t;

class StateSet {
public:
  StateSet(int width, int height) : _width(width), _height(height) {
    _size = width * (height+2);
  }
  bool checkAndInsert(state_t state) {
    unsigned long encoded = state.x + _width * (state.y+1) + _size * state.timeElapsed;
    if (encodedSet.find(encoded) != encodedSet.end()) {
      return false;
    }
    else {
      encodedSet.insert(encoded);
      return true;
    }
  }
  void clear() {
    encodedSet.clear();
  }
private:
  std::unordered_set<unsigned long> encodedSet;
  int _width;
  int _height;
  int _size;
};

bool operator<(const state_t& a, const state_t& b) {
  return a.estCost > b.estCost;
}

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
  std::getline(ifs, str);
  int width = 0;
  while (str[width] != '\0' && str[width] != '\n' && str[width] != '\r') {
    width++;
  }
  int height = 1;
  while (std::getline(ifs, str)) {
    height++;
  }
  // this is actually 2 larger in each dimension than what we're looking for
  width -= 2;
  height -= 2;
  std::cout << "Using width " << width << " height " << height << std::endl;
  // instantiate state holder, reset file stream
  BlizzardStates states(width, height);
  ifs.clear();
  ifs.seekg(0);
  // load map
  // first line
  std::getline(ifs, str);
  // find starting position
  int startX = 0;
  while (str[startX] != '.') {startX++;}
  startX--;
  // now iterate through the proper map
  for (int y = 0; y < height; y++) {
    std::getline(ifs, str);
    for (int x = 0; x < width; x++) {
      if (str[x+1] != '.') {
        states.addBlizzard(x, y, str[x+1]);
      }
    }
  }
  std::getline(ifs, str);
  // find end position
  int destX = 0;
  while (str[destX] != '.') {destX++;}
  destX--;
  // state manager is ready, now search
  std::priority_queue<state_t> stateQueue;
  StateSet visitedStates(width, height);
  state_t curState;
  curState.x = startX;
  curState.y = -1;
  curState.timeElapsed = 0;
  curState.estCost = width+height;
  stateQueue.push(curState);
  while (1) {
    state_t newState;
    curState = stateQueue.top();
    stateQueue.pop();
    //std::cout << "Analyzing state " << curState.x << "," << curState.y << " minute " << curState.timeElapsed << " estimated cost " << curState.estCost << std::endl;
    if (curState.x == destX && curState.y == height) {
      break;
    }
    newState = curState;
    newState.timeElapsed++;
    // check five possiblities to see if it's possible
    if ((curState.x == startX && curState.y == -1) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y)) {
      // don't move
      newState.estCost++; // cost is one more than previous
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if (!states.checkStateAtPos(newState.timeElapsed, curState.x + 1, curState.y)) {
      // move right
      newState.x++;
      newState.estCost = newState.timeElapsed + abs(destX - newState.x) + abs(height - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if (!states.checkStateAtPos(newState.timeElapsed, curState.x - 1, curState.y)) {
      // move left
      newState.x = curState.x - 1;
      newState.estCost = newState.timeElapsed + abs(destX - newState.x) + abs(height - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if ((curState.x == startX && curState.y == 0) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y - 1)) {
      // move up
      newState.x = curState.x;
      newState.y = curState.y - 1;
      newState.estCost = newState.timeElapsed + abs(destX - newState.x) + abs(height - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if ((curState.x == destX && curState.y == height - 1) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y + 1)) {
      // move down
      newState.x = curState.x;
      newState.y = curState.y + 1;
      newState.estCost = newState.timeElapsed + abs(destX - newState.x) + abs(height - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
  }
  std::cout << "Found path to destination after " << curState.timeElapsed << " minutes." << std::endl;
  // path back
  while (!stateQueue.empty()) stateQueue.pop();
  visitedStates.clear();
  curState.estCost = curState.timeElapsed + width + height;
  stateQueue.push(curState);
  while (1) {
    state_t newState;
    curState = stateQueue.top();
    stateQueue.pop();
    //std::cout << "Analyzing state " << curState.x << "," << curState.y << " minute " << curState.timeElapsed << " estimated cost " << curState.estCost << std::endl;
    if (curState.x == startX && curState.y == -1) {
      break;
    }
    newState = curState;
    newState.timeElapsed++;
    // check five possiblities to see if it's possible
    if ((curState.x == destX && curState.y == height) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y)) {
      // don't move
      newState.estCost++; // cost is one more than previous
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if (!states.checkStateAtPos(newState.timeElapsed, curState.x + 1, curState.y)) {
      // move right
      newState.x++;
      newState.estCost = newState.timeElapsed + abs(startX - newState.x) + abs(-1 - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if (!states.checkStateAtPos(newState.timeElapsed, curState.x - 1, curState.y)) {
      // move left
      newState.x = curState.x - 1;
      newState.estCost = newState.timeElapsed + abs(startX - newState.x) + abs(-1 - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if ((curState.x == startX && curState.y == 0) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y - 1)) {
      // move up
      newState.x = curState.x;
      newState.y = curState.y - 1;
      newState.estCost = newState.timeElapsed + abs(startX - newState.x) + abs(-1 - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if ((curState.x == destX && curState.y == height - 1) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y + 1)) {
      // move down
      newState.x = curState.x;
      newState.y = curState.y + 1;
      newState.estCost = newState.timeElapsed + abs(startX - newState.x) + abs(-1 - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
  }
  std::cout << "Found path back to start after " << curState.timeElapsed << " minutes." << std::endl;
  // path back again
  while (!stateQueue.empty()) stateQueue.pop();
  visitedStates.clear();
  curState.estCost = curState.timeElapsed + width + height;
  stateQueue.push(curState);
  while (1) {
    state_t newState;
    curState = stateQueue.top();
    stateQueue.pop();
    //std::cout << "Analyzing state " << curState.x << "," << curState.y << " minute " << curState.timeElapsed << " estimated cost " << curState.estCost << std::endl;
    if (curState.x == destX && curState.y == height) {
      break;
    }
    newState = curState;
    newState.timeElapsed++;
    // check five possiblities to see if it's possible
    if ((curState.x == startX && curState.y == -1) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y)) {
      // don't move
      newState.estCost++; // cost is one more than previous
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if (!states.checkStateAtPos(newState.timeElapsed, curState.x + 1, curState.y)) {
      // move right
      newState.x++;
      newState.estCost = newState.timeElapsed + abs(destX - newState.x) + abs(height - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if (!states.checkStateAtPos(newState.timeElapsed, curState.x - 1, curState.y)) {
      // move left
      newState.x = curState.x - 1;
      newState.estCost = newState.timeElapsed + abs(destX - newState.x) + abs(height - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if ((curState.x == startX && curState.y == 0) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y - 1)) {
      // move up
      newState.x = curState.x;
      newState.y = curState.y - 1;
      newState.estCost = newState.timeElapsed + abs(destX - newState.x) + abs(height - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
    if ((curState.x == destX && curState.y == height - 1) || !states.checkStateAtPos(newState.timeElapsed, curState.x, curState.y + 1)) {
      // move down
      newState.x = curState.x;
      newState.y = curState.y + 1;
      newState.estCost = newState.timeElapsed + abs(destX - newState.x) + abs(height - newState.y);
      //bestAtPos[newState.x + width*newState.y] = newState.timeElapsed;
      if (visitedStates.checkAndInsert(newState)) {
        stateQueue.push(newState);
      }
    }
  }
  std::cout << "Found path to final destination after " << curState.timeElapsed << " minutes." << std::endl;
}