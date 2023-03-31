#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

const char* rootStr = "root";
const char* humanStr = "humn";

class MonkeyEntry {
public:
  MonkeyEntry() : _name(0), _value(0), _evaluated(true), _op(0), _left(0), _right(0), _leftPointer(nullptr), _rightPointer(nullptr), _marked(false) {}
  MonkeyEntry(unsigned int name, long value) : _name(name), _value(value), _evaluated(true), _leftPointer(nullptr), _rightPointer(nullptr), _marked(false) {}
  MonkeyEntry(unsigned int name, char op, unsigned int left, unsigned int right) : _name(name), _op(op), _left(left), _right(right), _evaluated(false), _leftPointer(nullptr), _rightPointer(nullptr), _marked(false) {}
  MonkeyEntry* getLeft(std::unordered_map<unsigned int, MonkeyEntry*>& monkeyMap) {
    if (_evaluated) return nullptr;
    else return monkeyMap[_left];
  }
  MonkeyEntry* getRight(std::unordered_map<unsigned int, MonkeyEntry*>& monkeyMap) {
    if (_evaluated) return nullptr;
    else return monkeyMap[_right];
  }
  MonkeyEntry* getLeft() {
    return _leftPointer;
  }
  MonkeyEntry* getRight() {
    return _rightPointer;
  }
  void setLeft(MonkeyEntry* leftPointer) {
    _leftPointer = leftPointer;
  }
  void setRight(MonkeyEntry* rightPointer) {
    _rightPointer = rightPointer;
  }
  char getOp() {
    return _op;
  }
  void setOp(char op) {
    _op = op;
  }
  void setMarked(bool marked) {
    _marked = marked;
  }
  bool isMarked() {
    return _marked;
  }
  bool isEvaluated() {
    return _evaluated;
  }
  unsigned int getName() {
    return _name;
  }
  std::string getNameStr() {
    std::string str((char *)(&_name), 4);
    return str;
  }
  long evaluate() {
    if (!_evaluated) {
      switch (_op) {
      case '+':
        _value = _leftPointer->evaluate() + _rightPointer->evaluate();
        break;
      case '-':
        _value = _leftPointer->evaluate() - _rightPointer->evaluate();
        break;
      case '*':
        _value = _leftPointer->evaluate() * _rightPointer->evaluate();
        break;
      case '/':
        _value = _leftPointer->evaluate() / _rightPointer->evaluate();
        break;
      default:
        break;
      }
      _evaluated = true;
    }
    return _value;
  }
private:
  long _value;
  bool _evaluated;
  bool _marked; // is this node dependent on human?
  char _op;
  unsigned int _name; // encoded name
  unsigned int _left; // encoded left name
  unsigned int _right; // encoded right name
  MonkeyEntry* _leftPointer;
  MonkeyEntry* _rightPointer;
};

// constructs tree, returning if the human is in the tree
bool constructTreeRecursive(MonkeyEntry* entry, std::unordered_map<unsigned int, MonkeyEntry*>& monkeyMap) {
  if (entry->getName() == *(unsigned int *)humanStr) {
    entry->setMarked(true);
    return true;
  }
  if (entry->isEvaluated()) return false;
  MonkeyEntry* leftEntry = entry->getLeft(monkeyMap);
  MonkeyEntry* rightEntry = entry->getRight(monkeyMap);
  entry->setLeft(leftEntry);
  entry->setRight(rightEntry);
  bool hasHuman = false;
  hasHuman |= constructTreeRecursive(leftEntry, monkeyMap);
  hasHuman |= constructTreeRecursive(rightEntry, monkeyMap);
  entry->setMarked(hasHuman);
  return hasHuman;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: monkeymath input_file" << std::endl;
    exit(1);
  }
  std::ifstream fs(argv[1]);
  std::string str;
  std::unordered_map<unsigned int, MonkeyEntry*> monkeyMap;
  while (std::getline(fs, str)) {
    unsigned int name = *(unsigned int *)(str.c_str());
    if (isdigit(str[6])) {
      long value = strtol(str.c_str()+6, nullptr, 10);
      MonkeyEntry* monkeyEntry = new MonkeyEntry(name, value);
      monkeyMap[name] = monkeyEntry;
    }
    else {
      unsigned int left = *(unsigned int *)(str.c_str()+6);
      unsigned int right = *(unsigned int *)(str.c_str()+13);
      char op = str[11];
      MonkeyEntry* monkeyEntry = new MonkeyEntry(name, op, left, right);
      monkeyMap[name] = monkeyEntry;
    }
  }
  std::cout << "Loaded " << monkeyMap.size() << " monkeys." << std::endl;
  MonkeyEntry* rootEntry = monkeyMap[*(unsigned int *)rootStr];
  
  // convert into two trees for each side of the equation and ensure the human is on the left
  MonkeyEntry* leftEntry = rootEntry->getLeft(monkeyMap);
  MonkeyEntry* rightEntry = rootEntry->getRight(monkeyMap);
  bool leftHasHuman = constructTreeRecursive(leftEntry, monkeyMap);
  bool rightHasHuman = constructTreeRecursive(rightEntry, monkeyMap);
  if (!(leftHasHuman ^ rightHasHuman)) {
    std::cout << "Both sides have human, cannot solve this tree." << std::endl;
  }
  if (rightHasHuman) {
    std::swap(leftEntry, rightEntry);
  }
  
  // now manipulate the left tree, moving nodes to the right until the human is exposed
  while (leftEntry->getName() != *(unsigned int *)humanStr) {
    MonkeyEntry* leftBranch = leftEntry->getLeft();
    MonkeyEntry* rightBranch = leftEntry->getRight();
    
    /*std::cout << leftBranch->getNameStr();
    std::cout << ' ' << leftEntry->getOp() << '(' << leftEntry->getNameStr() << ") ";
    std::cout << rightBranch->getNameStr();
    std::cout << '=' << rightEntry->getLeft()->getNameStr();
    std::cout << ' ' << rightEntry->getOp() << '(' << rightEntry->getNameStr() << ") ";
    std::cout << rightEntry->getRight()->getNameStr() << std::endl;*/
    
    if (leftBranch->isMarked()) {
      // branch with x is on the left
      switch (leftEntry->getOp()) {
      case '+':
        leftEntry->setOp('-');
        break;
      case '-':
        leftEntry->setOp('+');
        break;
      case '*':
        leftEntry->setOp('/');
        break;
      case '/':
        leftEntry->setOp('*');
        break;
      }
      leftEntry->setLeft(rightEntry);
      rightEntry = leftEntry;
      leftEntry = leftBranch;
    }
    else {
      // branch with x is on the right
      switch (leftEntry->getOp()) {
      case '+':
        leftEntry->setOp('-');
        leftEntry->setLeft(rightEntry);
        leftEntry->setRight(leftBranch);
        break;
      case '-':
        // op, left is already set correctly
        leftEntry->setRight(rightEntry);
        break;
      case '*':
        leftEntry->setOp('/');
        leftEntry->setLeft(rightEntry);
        leftEntry->setRight(leftBranch);
        break;
      case '/':
        // op, left is already set correctly
        leftEntry->setRight(rightEntry);
        break;
      }
      rightEntry = leftEntry;
      leftEntry = rightBranch;
    }
  }
  
  // evaluate the right side
  std::cout << "Human: " << rightEntry->evaluate() << std::endl;

  return 0;
}