#include <iostream>
#include <fstream>
#include <cstdlib>
#include <queue>
#include <vector>
#include <string>

unsigned long ipow(unsigned long base, unsigned long exp) {
  unsigned long result = 1;
  for (;;) {
    if (exp & 1)
      result *= base;
    exp >>= 1;
    if (!exp)
      break;
    base *= base;
  }
  return result;
}

typedef enum {
  op_add,
  op_mul,
  op_pow
} op_t;

class Monkey {
private:
  op_t op;
  unsigned long operand;
  unsigned long divisor;
  unsigned long trueTarget;
  unsigned long falseTarget;
  std::queue<unsigned long> items;
  unsigned long inspectCount;
  
public:
  Monkey(op_t _op, unsigned long _operand, unsigned long _divisor, unsigned long _trueTarget, unsigned long _falseTarget) {
    op = _op; 
    operand = _operand;
    divisor = _divisor;
    trueTarget = _trueTarget;
    falseTarget = _falseTarget;
    inspectCount = 0;
  }
  
  // inspects item at front of queue and provides new worry level and new owner
  // returns false if queue is empty
  bool inspectItem(unsigned long lcm, unsigned long &newWorry, unsigned long &newOwner) {
    if (items.empty()) {
      return false;
    }
    // retrieve item from queue
    newWorry = items.front();
    
    // perform operation
    switch (op) {
      case op_add:
        newWorry += operand;
        break;
      case op_mul:
        newWorry *= operand;
        break;
      case op_pow:
        newWorry = ipow(newWorry, operand);
        break;
      default:
        std::cout << "Unknown operation requested in inspectItem" << std::endl;
        exit(1);
    }
    
    // determine where item is headed next
    if (newWorry % divisor == 0) {
      newOwner = trueTarget;
    }
    else {
      newOwner = falseTarget;
    }
    
    items.pop();
    inspectCount++;
    newWorry = newWorry % lcm;
    return true;
  }
  
  void addItem(unsigned long _item) {
    items.push(_item);
  }
  
  void displayItems() {
    for (int i = 0; i < items.size(); i++) {
      if (i != 0) std::cout << ", ";
      std::cout << items.front();
      items.push(items.front());
      items.pop();
    }
  }
  
  unsigned long getInspectCount() {
    return inspectCount;
  }
};
  
int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: monkey input_file round_count" << std::endl;
    exit(1);
  }
  
  std::ifstream fs(argv[1]);
  unsigned long totalRounds = atoi(argv[2]);
  
  std::vector<Monkey> monkeys;
  unsigned long lcm = 1;
  while (!fs.eof()) {
    std::string str;
    op_t op;
    unsigned long operand;
    unsigned long divisor;
    unsigned long trueTarget;
    unsigned long falseTarget;
    std::queue<unsigned long> items;
    fs >> str; // "Monkey"
    if (fs.eof()) {
      break;
    }
    fs >> str; // "id:"
    fs >> str; // "Starting"
    fs >> str; // "items:"
    do {
      unsigned long num;
      fs >> num;
      items.push(num);
      fs >> str; // "," or "Operation:"
    } while (str == ",");
    fs >> str; // "new"
    fs >> str; // "="
    fs >> str; // "old"
    fs >> str;
    if (str == "+") {
      op = op_add;
      fs >> operand;
    }
    else if (str == "*") {
      fs >> str;
      if (str == "old") {
        op = op_pow;
        operand = 2;
      }
      else {
        op = op_mul;
        operand = std::stoi(str);
      }
    }
    else {
      std::cout << "Unknown operation " << str << std::endl;
      exit(1);
    }
    fs >> str; // "Test:"
    fs >> str; // "divisible"
    fs >> str; // "by"
    fs >> divisor;
    lcm *= divisor;
    std::cout << "LCM " << lcm << std::endl;
    fs >> str; // "If"
    fs >> str; // "true:"
    fs >> str; // "throw"
    fs >> str; // "to"
    fs >> str; // "monkey"
    fs >> trueTarget;
    fs >> str; // "If"
    fs >> str; // "false:"
    fs >> str; // "throw"
    fs >> str; // "to"
    fs >> str; // "monkey"
    fs >> falseTarget;
    
    monkeys.emplace_back(op, operand, divisor, trueTarget, falseTarget);
    while (!items.empty()) {
      monkeys.back().addItem(items.front());
      items.pop();
    }
  }
  std::cout << "Initial state:" << std::endl;
  for (auto it = monkeys.begin(); it != monkeys.end(); it++) {
    it->displayItems();
    std::cout << std::endl;
  }
  
  for (unsigned long round = 0; round < totalRounds; round++) {
    for (auto it = monkeys.begin(); it != monkeys.end(); it++) {
      unsigned long newWorry;
      unsigned long newOwner;
      while (it->inspectItem(lcm, newWorry, newOwner)) {
        monkeys[newOwner].addItem(newWorry);
      }
    }
    if (round % 1000 == 999) {
      std::cout << "End of round " << round+1 << ":" << std::endl;
      for (auto it = monkeys.begin(); it != monkeys.end(); it++) {
        it->displayItems();
        std::cout << std::endl;
      }
    }
  }
  
  unsigned long max[2] = {0, 0};
  for (int i = 0; i < monkeys.size(); i++) {
    unsigned long inspectCount = monkeys[i].getInspectCount();
    std::cout << "Monkey " << i << " inspected items " << inspectCount << " times." << std::endl;
    if (inspectCount > max[0]) {
      max[1] = max[0];
      max[0] = inspectCount;
    }
    else if (inspectCount > max[1]) {
      max[1] = inspectCount;
    }
  }
  std::cout << "Monkey business: " << max[0]*max[1] << std::endl;
 
  return 0; 
}