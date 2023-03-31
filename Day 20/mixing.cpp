#include <iostream>
#include <fstream>
#include <vector>

const long long DECRYPTION_KEY = 811589153;

class Node {
public:
  Node(long long val) : _val(val) {}
  
  void addNext(Node * next) {
    _next = next;
    next->_prev = this;
  }
  
  void advanceSingle() {
    _prev->_next = _next;
    _next->_prev = _prev;
    _next->_next->_prev = this;
    _prev = _next;
    Node * temp = _next->_next;
    _next->_next = this;
    _next = temp;
  }
  
  void retreatSingle() {
    _next->_prev = _prev;
    _prev->_next = _next;
    _prev->_prev->_next = this;
    _next = _prev;
    Node * temp = _prev->_prev;
    _prev->_prev = this;
    _prev = temp;
  }
    
  void advance(long long count) {
    if (count > 0) {
      for (long long i = 0; i < count; i++) {
        advanceSingle();
      }
    }
    else {
      for (long long i = 0; i < -count; i++) {
        retreatSingle();
      }
    }
  }
  
  Node * toIndex(long long index) {
    Node * cur = this;
    if (index > 0) {
      for (long long i = 0; i < index; i++) {
        cur = cur->_next;
      }
    }
    else {
      for (long long i = 0; i < index; i++) {
        cur = cur->_prev;
      }
    }
    return cur;
  }
  
  long long getVal() {
    return _val;
  }
  
private:
  long long _val;
  Node * _next;
  Node * _prev;
};

void printOrder(Node * zeroNode) {
  std::cout << "Order:" << std::endl;
  Node* cur = zeroNode;
  std::cout << cur->getVal() << std::endl;
  cur = cur->toIndex(1);
  while (cur != zeroNode) {
    std::cout << cur->getVal() << std::endl;
    cur = cur->toIndex(1);
  }
}
  

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: mixing input.txt [part2]" << std::endl;
    return EXIT_FAILURE;
  }
  bool part2 = (argc > 2) ? true : false;
  std::ifstream ifs(argv[1]);
  std::vector<Node*> nodes;
  long long value;
  ifs >> value;
  Node* prev;
  Node* cur;
  Node* zeroNode;
  prev = nullptr;
  cur = nullptr;
  zeroNode = nullptr;
  while (!ifs.fail()) {
    cur = new Node(part2 ? value*DECRYPTION_KEY : value);
    if (value == 0) {
      zeroNode = cur;
    }
    nodes.push_back(cur);
    if (prev != nullptr) {
      prev->addNext(cur);
    }
    prev = cur;
    ifs >> value;
  }
  cur->addNext(nodes[0]);
  
  long long modVal = nodes.size() - 1;
  long long modVal_2 = modVal / 2;
  std::cout << "Loaded " << nodes.size() << " nodes." << std::endl;
  for (int i = 0; i < (part2 ? 10 : 1); i++) {
    // Mix the nodes
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
      long long valSlide = (*it)->getVal() + modVal_2;
      valSlide = valSlide % modVal;
      valSlide = valSlide - modVal_2;
      (*it)->advance(valSlide);
    }
  }
  
  long long sum = 0;
  std::cout << "Values:" << std::endl;
  cur = zeroNode;
  for (int i = 0; i < 3; i++) {
    cur = cur->toIndex(1000);
    std::cout << cur->getVal() << std::endl;
    sum += cur->getVal();
  }
  std::cout << "Sum: " << sum << std::endl;
  
  // clean up
  for (auto it = nodes.begin(); it != nodes.end(); it++) {
    delete (*it);
  }
}