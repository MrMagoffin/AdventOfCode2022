#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

// represents an element in an input, which can be either a list or an integer
// each list itself contains elements which are either lists or integers, also represented by ListElement
class ListElement {
private:
  bool isList; // is this element a list
  std::vector<ListElement*> list; // implementation of list element
  int value; // integer value of non-list element
  ListElement* parent;
public:
  // a list element created without an integer value argument is a list
  ListElement(ListElement* _parent = nullptr) {
    isList = true; 
    parent = _parent;
  }
  // a list element created with an integer value argument is a value
  ListElement(int _value, ListElement* _parent = nullptr) {
    isList = false;
    value = _value;
  }
  // ListElement takes ownership of elements added to its list, so destruct children
  ~ListElement() {
    for (auto it = list.begin(); it != list.end(); it++) {
      delete (*it);
    }
  }
  // add an element to this list element
  void addElement(ListElement* element) {
    list.push_back(element);
  }
  // return parent
  ListElement* getParent() {
    return parent;
  }
  std::string toString() {
    if (isList) {
      std::string str = "[";
      for (auto it = list.begin(); it != list.end(); it++) {
        str.append((*it)->toString());
        str.push_back(',');
      }
      if (list.size() > 0) {
        str.pop_back();
      }
      str.push_back(']');
      return str;
    }
    else {
      return std::to_string(value);
    }
  }
  // comparison operators shall be implemented
  friend bool operator<(const ListElement& lhs, const ListElement& rhs);
  friend bool operator>(const ListElement& lhs, const ListElement& rhs);
  friend bool operator==(const ListElement& lhs, const ListElement& rhs);
  friend bool operator!=(const ListElement& lhs, const ListElement& rhs);
};

bool operator<(const ListElement& lhs, const ListElement& rhs) {
  if (!lhs.isList && !rhs.isList) {
    // just comparing two integers
    return (lhs.value < rhs.value);
  }
  else if (lhs.isList && rhs.isList) {
    // compare values in list
    for (int i = 0; i < std::min(lhs.list.size(), rhs.list.size()); i++) {
      if (*(lhs.list[i]) < *(rhs.list[i])) {
        return true;
      }
      else if (*(lhs.list[i]) > *(rhs.list[i])) {
        return false;
      }
    }
    // list values were in the right order, compare sizes
    if (lhs.list.size() < rhs.list.size()) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    // comparing an integer to a list, convert integer to list
    if (lhs.isList) {
      ListElement newList;
      ListElement* newInt = new ListElement(rhs.value);
      newList.addElement(newInt);
      return (lhs < newList);
    }
    else {
      ListElement newList;
      ListElement* newInt = new ListElement(lhs.value);
      newList.addElement(newInt);
      return (newList < rhs);
    }
  }
}

bool operator>(const ListElement& lhs, const ListElement& rhs) {
  return (rhs < lhs);
}

bool operator==(const ListElement& lhs, const ListElement& rhs) {
  if (!lhs.isList && !rhs.isList) {
    // just comparing two integers
    return (lhs.value == rhs.value);
  }
  else if (lhs.isList && rhs.isList) {
    // compare values in list
    for (int i = 0; i < std::min(lhs.list.size(), rhs.list.size()); i++) {
      if (*(lhs.list[i]) != *(rhs.list[i])) {
        return false;
      }
    }
    // list values were in the right order, compare sizes
    if (lhs.list.size() == rhs.list.size()) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    // comparing an integer to a list, convert integer to list
    if (lhs.isList) {
      ListElement newList;
      ListElement* newInt = new ListElement(rhs.value);
      newList.addElement(newInt);
      return (lhs == newList);
    }
    else {
      ListElement newList;
      ListElement* newInt = new ListElement(lhs.value);
      newList.addElement(newInt);
      return (rhs == newList);
    }
  }
}

bool operator!=(const ListElement& lhs, const ListElement& rhs) {
  return !(lhs == rhs);
}

ListElement * parseList(FILE * fp) {
  ListElement * topList;
  ListElement * curList;
  int depth = 0;
  char c = fgetc(fp);
  if (c != '[') {
    printf("Unknown format, expected '[' and got %c\n", c);
    exit(1);
  }
  depth++;
  topList = new ListElement();
  curList = topList;
  while (depth > 0) {
    char c = fgetc(fp);
    if (c == '\n') {
      // unexpected end of line
      printf("Unexpected end of line.\n");
      exit(1);
    }
    else if (c == '[') {
      // start of list
      ListElement * newList;
      newList = new ListElement(curList);
      curList->addElement(newList);
      curList = newList;
      depth++;
    }
    else if (c == ']') {
      // end of list
      curList = curList->getParent();
      depth--;
    }
    else if (c >= '0' && c <= '9') {
      // integer value
      std::string intStr = "";
      while (c != ',' && c != ']') {
        if (c < '0' || c > '9') {
          printf("Expected integer, got %c\n", c);
          exit(1);
        }
        intStr.push_back(c);
        c = fgetc(fp);
      }
      // now either ',' or ']' is in c, and intStr has an integer
      ListElement * newElement;
      try {
        newElement = new ListElement(std::stoi(intStr));
      }
      catch (std::invalid_argument e) {
        printf("stoi choked on %s\n", intStr);
        exit(1);
      }
      curList->addElement(newElement);
      if (c == ']') {
        // end of list
        curList = curList->getParent();
        depth--;
      }
    }
    else if (c == ',') {
      // ignore separator
    }
    else {
      // parsing error
      printf("Unexpected character %c encountered during parsing\n");
      printf("Next 5 characters:");
      for (int i = 0; i < 5; i++) {
        char c = fgetc(fp);
        printf("%c", c);
      }
      printf("\n");
      exit(1);
    }
  }
  // find newline
  c = fgetc(fp);
  while (!feof(fp) && c != '\n') {
    c = fgetc(fp);
  }
  return topList;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: distress input_file\n");
  }
  FILE * fp = fopen(argv[1], "r");
  
  // part 1
  int pairIndex = 1;
  int sum = 0;
  std::vector<ListElement *> packets;
  while (!feof(fp)) {
    ListElement * lhs = parseList(fp);
    ListElement * rhs = parseList(fp);
    if ((*lhs) > (*rhs)) {
      // wrong order
      printf("Index %d: wrong order!\n", pairIndex);
    }
    else {
      // correct order
      printf("Index %d: correct order!\n", pairIndex);
      sum += pairIndex;
    }
    // skip past extra newline
    char c = fgetc(fp);
    while (!feof(fp) && c != '\n') {
      c = fgetc(fp);
    }
    pairIndex++;
    packets.push_back(lhs);
    packets.push_back(rhs);
  }
  printf("Sum: %d\n", sum);
  
  // part 2
  ListElement * dividerPacket[2];
  int dividerIndex[2];
  int i;
  dividerPacket[0] = new ListElement;
  ListElement * subPacket = new ListElement(dividerPacket[0]);
  dividerPacket[0]->addElement(subPacket);
  ListElement * numPacket = new ListElement(2, subPacket);
  subPacket->addElement(numPacket);
  packets.push_back(dividerPacket[0]);
  dividerPacket[1] = new ListElement;
  subPacket = new ListElement(dividerPacket[1]);
  dividerPacket[1]->addElement(subPacket);
  numPacket = new ListElement(6, subPacket);
  subPacket->addElement(numPacket);
  packets.push_back(dividerPacket[1]);
  
  std::sort(packets.begin(), packets.end(), [](ListElement* lhs, ListElement* rhs){
    return (*lhs) < (*rhs);
  });
  
  i = 1;
  for (auto it = packets.begin(); it != packets.end(); it++) {
    if (*it == dividerPacket[0]) dividerIndex[0] = i;
    if (*it == dividerPacket[1]) dividerIndex[1] = i;
    delete (*it);
    i++;
  }
  printf("Indices: %d, %d\n", dividerIndex[0], dividerIndex[1]);
  printf("Decoder key: %d", dividerIndex[0]*dividerIndex[1]);
  
  return 0;
}