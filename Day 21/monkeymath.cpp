#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

class Monkey {
public:
  Monkey() : _value(0), _evaluated(true), _op(0), _left(0), _right(0) {}
  Monkey(long& value) : _value(value), _evaluated(true) {}
  Monkey(char& op, unsigned int& left, unsigned int& right) : _op(op), _left(left), _right(right), _evaluated(false) {}
  long getValue(std::unordered_map<unsigned int, Monkey>& monkeyMap) {
    if (!_evaluated) {
      switch (_op) {
      case '+':
        _value = monkeyMap[_left].getValue(monkeyMap) + monkeyMap[_right].getValue(monkeyMap);
        break;
      case '-':
        _value = monkeyMap[_left].getValue(monkeyMap) - monkeyMap[_right].getValue(monkeyMap);
        break;
      case '*':
        _value = monkeyMap[_left].getValue(monkeyMap) * monkeyMap[_right].getValue(monkeyMap);
        break;
      case '/':
        _value = monkeyMap[_left].getValue(monkeyMap) / monkeyMap[_right].getValue(monkeyMap);
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
  char _op;
  unsigned int _left; // encoded left name
  unsigned int _right; // encoded right name
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: monkeymath input_file" << std::endl;
    exit(1);
  }
  std::ifstream fs(argv[1]);
  std::string str;
  std::unordered_map<unsigned int, Monkey> monkeyMap;
  while (std::getline(fs, str)) {
    unsigned int name = *(unsigned int *)(str.c_str());
    if (isdigit(str[6])) {
      long value = strtol(str.c_str()+6, nullptr, 10);
      monkeyMap.emplace(std::make_pair(name, Monkey(value)));
      std::cout << "Inserted monkey with name " << name << " value " << value << std::endl;
    }
    else {
      unsigned int left = *(unsigned int *)(str.c_str()+6);
      unsigned int right = *(unsigned int *)(str.c_str()+13);
      char op = str[11];
      Monkey monkey(op, left, right);
      monkeyMap.emplace(std::make_pair(name, monkey));
      std::cout << "Inserted monkey with name " << name << " op " << op << " left " << left << " right " << right << std::endl;
    }
  }
  const char* rootStr = "root";
  std::cout << "Loaded " << monkeyMap.size() << " monkeys." << std::endl;
  std::cout << "root: " << monkeyMap[*(unsigned int *)rootStr].getValue(monkeyMap) << std::endl;
  return 0;
}